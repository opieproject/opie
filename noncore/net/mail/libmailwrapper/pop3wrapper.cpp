#include <stdlib.h>
#include "pop3wrapper.h"
#include "mailtypes.h"
#include "logindialog.h"
#include <libetpan/libetpan.h>
#include <qpe/global.h>
#include <qfile.h>
#include <qstring.h>

/* we don't fetch messages larger than 5 MB */
#define HARD_MSG_SIZE_LIMIT 5242880

POP3wrapper::POP3wrapper( POP3account *a )
    : Genericwrapper()
{
    account = a;
    m_pop3 = NULL;
    m_folder = NULL;
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
    qDebug( "POP3: %i of %i", current, maximum );
}

RecBody POP3wrapper::fetchBody( const RecMail &mail )
{
    int err = MAILPOP3_NO_ERROR;
    char *message = 0;
    size_t length = 0;
    
    login();
    if ( !m_pop3 ) {
        return RecBody();
    }

    RecBody body;
    mailmessage * mailmsg;
    if (mail.Msgsize()>HARD_MSG_SIZE_LIMIT) {
        qDebug("Message to large: %i",mail.Msgsize());
        return body;
    }
    
    QFile msg_cache(msgTempName);

    cleanMimeCache();
    
    if (mail.getNumber()!=last_msg_id) {
        if (msg_cache.exists()) {
            msg_cache.remove();
        }
        msg_cache.open(IO_ReadWrite|IO_Truncate);
        last_msg_id = mail.getNumber();
        err = mailsession_get_message(m_folder->fld_session, mail.getNumber(), &mailmsg);
        err = mailmessage_fetch(mailmsg,&message,&length);
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
        /* transform to libetpan stuff */
        mailmsg = mailmessage_new();
        mailmessage_init(mailmsg, NULL, data_message_driver, 0, strlen(message));
        generic_message_t * msg_data;
        msg_data = (generic_message_t *)mailmsg->msg_data;
        msg_data->msg_fetched = 1;
        msg_data->msg_message = message;
        msg_data->msg_length = strlen(message);
    }
    body = parseMail(mailmsg);
    
    /* clean up */
    if (mailmsg) mailmessage_free(mailmsg);  
    if (message) free(message);
    
    return body;
}

void POP3wrapper::listMessages(const QString &, QList<RecMail> &target )
{
    login();
    if (!m_pop3) return;
    uint32_t res_messages,res_recent,res_unseen;
    mailsession_status_folder(m_folder->fld_session,"INBOX",&res_messages,&res_recent,&res_unseen);
    parseList(target,m_folder->fld_session,"INBOX");
    Global::statusMessage( tr("Mailbox contains %1 mail(s)").arg(res_messages));
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
		user = login.getUser().latin1();
		pass = login.getPassword().latin1();
	  } else {
		// cancel
		qDebug( "POP3: Login canceled" );
		return;
	  }
	} else {
	  user = account->getUser().latin1();
	  pass = account->getPassword().latin1();
	}

    bool ssl = account->getSSL();

    m_pop3=mailstorage_new(NULL);
    pop3_mailstorage_init(m_pop3,(char*)server,port,NULL,CONNECTION_TYPE_TRY_STARTTLS,POP3_AUTH_TYPE_TRY_APOP,
        (char*)user,(char*)pass,0,0,0);

    m_folder = mailfolder_new(m_pop3, NULL, NULL);

    if (m_folder==0) {
        Global::statusMessage(tr("Error initializing folder"));
        mailstorage_free(m_pop3);
        m_pop3 = NULL;
        return;
    }
    err = mailfolder_connect(m_folder);
    if (err != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error initializing folder"));
        mailfolder_free(m_folder);
        m_folder = 0;
        mailstorage_free(m_pop3);
        m_pop3 = 0;
    }
    qDebug( "POP3: logged in!" );
}

void POP3wrapper::logout()
{
    int err = MAILPOP3_NO_ERROR;
    if ( m_pop3 == NULL ) return;
    mailfolder_free(m_folder);
    m_folder = 0;
    mailstorage_free(m_pop3);
    m_pop3 = 0;
}


QList<Folder>* POP3wrapper::listFolders()
{
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
    int err = mailsession_remove_message(m_folder->fld_session,mail.getNumber());
    if (err != MAIL_NO_ERROR) {
        Global::statusMessage(tr("error deleting mail"));
    }
}

void POP3wrapper::answeredMail(const RecMail&)
{
}

int POP3wrapper::deleteAllMail(const Folder*)
{
    login();
    if (!m_pop3) return 0;
    int res = 1;

    uint32_t result = 0;
    int err = mailsession_messages_number(m_folder->fld_session,NULL,&result);
    if (err != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error getting folder info"));
        return 0;
    }
    for (unsigned int i = 0; i < result; ++i) {
        err = mailsession_remove_message(m_folder->fld_session,i+1);
        if (err != MAIL_NO_ERROR) {
            Global::statusMessage(tr("Error deleting mail %1").arg(i+1));
            res=0;
        }
        break;
    }           
    return res;
}

void POP3wrapper::statusFolder(folderStat&target_stat,const QString&)
{
    login();
    target_stat.message_count = 0;
    target_stat.message_unseen = 0;
    target_stat.message_recent = 0;
    if (!m_pop3) return;
    int r = mailsession_status_folder(m_folder->fld_session,0,&target_stat.message_count,
        &target_stat.message_recent,&target_stat.message_unseen);
}

void POP3wrapper::fetchRawBody(const RecMail&mail,char**target,size_t*length)
{
    mailmessage * mailmsg = 0;
    int err = mailsession_get_message(m_folder->fld_session, mail.getNumber(), &mailmsg);
    err = mailmessage_fetch(mailmsg,target,length);
    if (mailmsg) mailmessage_free(mailmsg);  
}

const QString&POP3wrapper::getType()const
{
    return account->getType();
}
