#include <stdlib.h>
#include "pop3wrapper.h"
#include "mailtypes.h"
#include "logindialog.h"
#include <libetpan/libetpan.h>

#include <opie2/odebug.h>
#include <qpe/global.h>
#include <qfile.h>

/* we don't fetch messages larger than 5 MB */
#define HARD_MSG_SIZE_LIMIT 5242880

using namespace Opie::Core;
POP3wrapper::POP3wrapper( POP3account *a )
: Genericwrapper() {
    account = a;
    m_pop3 = NULL;
    msgTempName = a->getFileName()+"_msg_cache";
    last_msg_id = 0;
}

POP3wrapper::~POP3wrapper() {
    logout();
    QFile msg_cache(msgTempName);
    if (msg_cache.exists()) {
        msg_cache.remove();
    }
}

void POP3wrapper::pop3_progress( size_t current, size_t maximum ) {
    odebug << "POP3: " << current << " of " << maximum << "" << oendl; 
}

RecBodyP POP3wrapper::fetchBody( const RecMailP &mail ) {
    int err = MAILPOP3_NO_ERROR;
    char *message = 0;
    size_t length = 0;

    RecBodyP body = new RecBody();

    login();
    if ( !m_pop3 ) {
        return body;
    }

    mailmessage * mailmsg;
    if (mail->Msgsize()>HARD_MSG_SIZE_LIMIT) {
        odebug << "Message to large: " << mail->Msgsize() << "" << oendl; 
        return body;
    }

    QFile msg_cache(msgTempName);

    cleanMimeCache();

    if (mail->getNumber()!=last_msg_id) {
        if (msg_cache.exists()) {
            msg_cache.remove();
        }
        msg_cache.open(IO_ReadWrite|IO_Truncate);
        last_msg_id = mail->getNumber();
        err = mailsession_get_message(m_pop3->sto_session, mail->getNumber(), &mailmsg);
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
    if (mailmsg)
        mailmessage_free(mailmsg);
    if (message)
        free(message);

    return body;
}

void POP3wrapper::listMessages(const QString &, QValueList<Opie::Core::OSmartPointer<RecMail> > &target )
{
    login();
    if (!m_pop3)
        return;
    uint32_t res_messages,res_recent,res_unseen;
    mailsession_status_folder(m_pop3->sto_session,"INBOX",&res_messages,&res_recent,&res_unseen);
    parseList(target,m_pop3->sto_session,"INBOX");
    Global::statusMessage( tr("Mailbox contains %1 mail(s)").arg(res_messages));
}

void POP3wrapper::login()
{
    if (account->getOffline())
        return;
    /* we'll hold the line */
    if ( m_pop3 != NULL )
        return;

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
            odebug << "POP3: Login canceled" << oendl; 
            return;
        }
    } else {
        user = account->getUser().latin1();
        pass = account->getPassword().latin1();
    }

    //  bool ssl = account->getSSL();

    m_pop3=mailstorage_new(NULL);

    int conntypeset = account->ConnectionType();
    int conntype = 0;
    if ( conntypeset == 3 ) {
        conntype = CONNECTION_TYPE_COMMAND;
    } else if ( conntypeset == 2 ) {
        conntype = CONNECTION_TYPE_TLS;
    } else if ( conntypeset == 1 ) {
        conntype = CONNECTION_TYPE_STARTTLS;
    } else if ( conntypeset == 0 ) {
        conntype = CONNECTION_TYPE_TRY_STARTTLS;
    }

    //(ssl?CONNECTION_TYPE_TLS:CONNECTION_TYPE_PLAIN);

    pop3_mailstorage_init(m_pop3,(char*)server, port, NULL, conntype, POP3_AUTH_TYPE_PLAIN,
                          (char*)user,(char*)pass,0,0,0);


    err = mailstorage_connect(m_pop3);
    if (err != MAIL_NO_ERROR) {
        odebug << QString( "FEHLERNUMMER %1" ).arg(  err ) << oendl; 
        Global::statusMessage(tr("Error initializing folder"));
        mailstorage_free(m_pop3);
        m_pop3 = 0;
    } else {
        mailsession * session = m_pop3->sto_session;
        mailpop3 * mail =  ( (  pop3_session_state_data * )session->sess_data )->pop3_session;
        if (mail) {
            mail->pop3_progr_fun = &pop3_progress;
        }
    }
}

void POP3wrapper::logout()
{
    if ( m_pop3 == NULL )
        return;
    mailstorage_free(m_pop3);
    m_pop3 = 0;
}


QValueList<Opie::Core::OSmartPointer<Folder> >* POP3wrapper::listFolders() {
    QValueList<Opie::Core::OSmartPointer<Folder> >* folders = new QValueList<FolderP>();
    FolderP inb=new Folder("INBOX","/");
    folders->append(inb);
    return folders;
}

void POP3wrapper::deleteMail(const RecMailP&mail) {
    login();
    if (!m_pop3)
        return;
    int err = mailsession_remove_message(m_pop3->sto_session,mail->getNumber());
    if (err != MAIL_NO_ERROR) {
        Global::statusMessage(tr("error deleting mail"));
    }
}

void POP3wrapper::answeredMail(const RecMailP&) {}

int POP3wrapper::deleteAllMail(const FolderP&) {
    login();
    if (!m_pop3)
        return 0;
    int res = 1;

    uint32_t result = 0;
    int err = mailsession_messages_number(m_pop3->sto_session,NULL,&result);
    if (err != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error getting folder info"));
        return 0;
    }
    for (unsigned int i = 0; i < result; ++i) {
        err = mailsession_remove_message(m_pop3->sto_session,i+1);
        if (err != MAIL_NO_ERROR) {
            Global::statusMessage(tr("Error deleting mail %1").arg(i+1));
            res=0;
        }
        break;
    }
    return res;
}

void POP3wrapper::statusFolder(folderStat&target_stat,const QString&) {
    login();
    target_stat.message_count = 0;
    target_stat.message_unseen = 0;
    target_stat.message_recent = 0;
    if (!m_pop3)
        return;
    int r = mailsession_status_folder(m_pop3->sto_session,0,&target_stat.message_count,
                                      &target_stat.message_recent,&target_stat.message_unseen);
    if (r != MAIL_NO_ERROR) {
        odebug << "error getting folter status." << oendl; 
    }
}

encodedString* POP3wrapper::fetchRawBody(const RecMailP&mail) {
    char*target=0;
    size_t length=0;
    encodedString*res = 0;
    mailmessage * mailmsg = 0;
    int err = mailsession_get_message(m_pop3->sto_session, mail->getNumber(), &mailmsg);
    err = mailmessage_fetch(mailmsg,&target,&length);
    if (mailmsg)
        mailmessage_free(mailmsg);
    if (target) {
        res = new encodedString(target,length);
    }
    return res;
}

MAILLIB::ATYPE POP3wrapper::getType()const {
    return account->getType();
}

const QString&POP3wrapper::getName()const{
    return account->getAccountName();
}
