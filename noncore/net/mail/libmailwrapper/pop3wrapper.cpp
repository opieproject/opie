#include <stdlib.h>
#include "pop3wrapper.h"
#include "mailtypes.h"
#include "logindialog.h"
#include <libetpan/libetpan.h>
#include <qpe/global.h>
#include <qfile.h>

/* we don't fetch messages larger than 5 MB */
#define HARD_MSG_SIZE_LIMIT 5242880

POP3wrapper::POP3wrapper( POP3account *a )
    : Genericwrapper()
{
    account = a;
    m_pop3 = NULL;
    msgTempName = a->getFileName()+"_msg_cache";
    last_msg_id = 0;
}

POP3wrapper::~POP3wrapper()
{
    logout();
    QFile msg_cache(msgTempName);
    if (msg_cache.exists()) {
        msg_cache.remove();
    }
}

void POP3wrapper::pop3_progress( size_t current, size_t maximum )
{
    //qDebug( "POP3: %i of %i", current, maximum );
}

RecBody POP3wrapper::fetchBody( const RecMail &mail )
{
    int err = MAILPOP3_NO_ERROR;
    char *message;
    size_t length = 0;
    
    login();
    if ( !m_pop3 ) {
        return RecBody();
    }

    RecBody body;
    mailmessage * msg = 0;

    QFile msg_cache(msgTempName);

    if (mail.Msgsize()>HARD_MSG_SIZE_LIMIT) {
        qDebug("Message to large: %i",mail.Msgsize());
        return body;
    }
    cleanMimeCache();
    if (mail.getNumber()!=last_msg_id) {
        if (msg_cache.exists()) {
            msg_cache.remove();
        }
        msg_cache.open(IO_ReadWrite|IO_Truncate);
        last_msg_id = mail.getNumber();
        err = mailpop3_retr( m_pop3, mail.getNumber(), &message, &length );    
        if ( err != MAILPOP3_NO_ERROR ) {
            qDebug( "POP3: error retrieving body with index %i", mail.getNumber() );
            last_msg_id = 0;
            return RecBody();
        }
        msg_cache.writeBlock(message,length);
    } else {    
        QString msg="";
        msg_cache.open(IO_ReadOnly);
        message = new char[4096];
        memset(message,0,4096);
        while (msg_cache.readBlock(message,4095)>0) {
            msg+=message;
            memset(message,0,4096);
        }
        delete message;
        message = (char*)malloc(msg.length()+1*sizeof(char));
        memset(message,0,msg.length()+1);
        memcpy(message,msg.latin1(),msg.length());
    }
    
    /* transform to libetpan stuff */
    msg = mailmessage_new();
    mailmessage_init(msg, NULL, data_message_driver, 0, strlen(message));
    generic_message_t * msg_data;
    msg_data = (generic_message_t *)msg->msg_data;
    msg_data->msg_fetched = 1;
    msg_data->msg_message = message;
    msg_data->msg_length = strlen(message);

    /* parse the mail */
    body = parseMail(msg);

    /* clean up */
    mailmessage_free(msg);  
    free(message);

    /* finish */
    return body;
}

void POP3wrapper::listMessages(const QString &, QList<RecMail> &target )
{
    int err = MAILPOP3_NO_ERROR;
    char * header = 0;
    /* these vars are used recurcive! set it to 0!!!!!!!!!!!!!!!!! */
    size_t length = 0;
    carray * messages = 0;

    login();
    if (!m_pop3) return;

    mailpop3_list( m_pop3, &messages );

    for (unsigned int i = 0; i < carray_count(messages);++i) {
        mailpop3_msg_info *info;
        err = mailpop3_get_msg_info(m_pop3,i+1,&info);
        if (info->msg_deleted)
            continue;
        err = mailpop3_header( m_pop3, info->msg_index, &header, &length );
        if ( err != MAILPOP3_NO_ERROR ) {
            qDebug( "POP3: error retrieving header msgid: %i", info->msg_index );
            free(header);
            return;
        }
        RecMail *mail = parseHeader( header );
        mail->setNumber( info->msg_index );
        mail->setWrapper(this);
        mail->setMsgsize(info->msg_size);
        target.append( mail );
        free(header);
    }
    Global::statusMessage( tr("Mailbox contains %1 mail(s)").arg(carray_count(messages)-m_pop3->pop3_deleted_count));
}

void POP3wrapper::login()
{
    /* we'll hold the line */
    if ( m_pop3 != NULL ) return;

    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILPOP3_NO_ERROR;

    server = account->getServer().latin1();
    port = account->getPort().toUInt();

	if ( account->getUser().isEmpty() || account->getPassword().isEmpty() ) {
	  LoginDialog login( account->getUser(), account->getPassword(), NULL, 0, true );
	  login.show();
	  if ( QDialog::Accepted == login.exec() ) {
		// ok
		user = strdup( login.getUser().latin1() );
		pass = strdup( login.getPassword().latin1() );
	  } else {
		// cancel
		qDebug( "POP3: Login canceled" );
		return;
	  }
	} else {
	  user = account->getUser().latin1();
	  pass = account->getPassword().latin1();
	}

    m_pop3 = mailpop3_new( 200, &pop3_progress );

    // connect
    if (account->getSSL()) {
        err = mailpop3_ssl_connect( m_pop3, (char*)server, port );
    } else {
        err = mailpop3_socket_connect( m_pop3, (char*)server, port );
    }

    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error connecting to %s\n reason: %s", server,
                m_pop3->pop3_response );
        mailpop3_free( m_pop3 );
        m_pop3 = NULL;
        return;
    }
    qDebug( "POP3: connected!" );

    // login
    // TODO: decide if apop or plain login should be used
    err = mailpop3_login( m_pop3, (char *) user, (char *) pass );
    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error logging in: %s", m_pop3->pop3_response );
        logout();
        return;
    }

    qDebug( "POP3: logged in!" );
}

void POP3wrapper::logout()
{
    int err = MAILPOP3_NO_ERROR;
    if ( m_pop3 == NULL ) return;
    err = mailpop3_quit( m_pop3 );
    mailpop3_free( m_pop3 );
    m_pop3 = NULL;
}


QList<Folder>* POP3wrapper::listFolders()
{
    /* TODO: integrate MH directories 
       but not before version 0.1 ;)
    */
    QList<Folder> * folders = new QList<Folder>();
    folders->setAutoDelete( false );
    Folder*inb=new Folder("INBOX","/");
    folders->append(inb);
    return folders;
}

void POP3wrapper::deleteMail(const RecMail&mail)
{
    login();
    if (!m_pop3) return;
    int err = mailpop3_dele(m_pop3,mail.getNumber());
    if (err != MAILPOP3_NO_ERROR) {
        qDebug("error deleting mail");
    }
}

void POP3wrapper::answeredMail(const RecMail&)
{
}
