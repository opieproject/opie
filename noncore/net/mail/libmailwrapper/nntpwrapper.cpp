#include "nntpwrapper.h"
#include "logindialog.h"
#include "mailtypes.h"

#include <qfile.h>

#include <stdlib.h>

#include <libetpan/libetpan.h>


#define HARD_MSG_SIZE_LIMIT 5242880

using namespace Opie::Core;
NNTPwrapper::NNTPwrapper( NNTPaccount *a )
: Genericwrapper() {
    account = a;
    m_nntp = NULL;
    msgTempName = a->getFileName()+"_msg_cache";
    last_msg_id = 0;
}

NNTPwrapper::~NNTPwrapper() {
    logout();
    QFile msg_cache(msgTempName);
    if (msg_cache.exists()) {
        msg_cache.remove();
    }
}

void NNTPwrapper::nntp_progress( size_t current, size_t maximum ) {
    qDebug( "NNTP: %i of %i", current, maximum );
}


RecBody NNTPwrapper::fetchBody( const RecMailP &mail ) {
    int err = NEWSNNTP_NO_ERROR;
    char *message = 0;
    size_t length = 0;

    login();
    if ( !m_nntp ) {
        return RecBody();
    }

    RecBody body;
    mailmessage * mailmsg;
    if (mail->Msgsize()>HARD_MSG_SIZE_LIMIT) {
        qDebug("Message to large: %i",mail->Msgsize());
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
        err = mailsession_get_message(m_nntp->sto_session, mail->getNumber(), &mailmsg);
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


void NNTPwrapper::listMessages(const QString & which, QValueList<Opie::Core::OSmartPointer<RecMail> > &target )
{
    login();
    if (!m_nntp)
        return;
    uint32_t res_messages,res_recent,res_unseen;
    mailsession_status_folder(m_nntp->sto_session,(char*)which.latin1(),&res_messages,&res_recent,&res_unseen);
    parseList(target,m_nntp->sto_session,which,true);
}

void NNTPwrapper::login()
{
    if (account->getOffline())
        return;
    /* we'll hold the line */
    if ( m_nntp != NULL )
        return;

    const char *server, *user, *pass;
    QString User,Pass;
    uint16_t port;
    int err = NEWSNNTP_NO_ERROR;

    server = account->getServer().latin1();
    port = account->getPort().toUInt();

    user = pass = 0;

    if ( ( account->getUser().isEmpty() || account->getPassword().isEmpty() ) && account->getLogin() ) {
        LoginDialog login( account->getUser(), account->getPassword(), NULL, 0, true );
        login.show();
        if ( QDialog::Accepted == login.exec() ) {
            // ok
            User = login.getUser().latin1();
            Pass = login.getPassword().latin1();
        } else {
            // cancel
            qDebug( "NNTP: Login canceled" );
            return;
        }
    } else {
        User = account->getUser().latin1();
        Pass = account->getPassword().latin1();
    }

    if (User.isEmpty()) {
        user=0;
        pass = 0;
    } else {
        user=User.latin1();
        pass=Pass.latin1();
    }
    //  bool ssl = account->getSSL();

    m_nntp=mailstorage_new(NULL);

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

   nntp_mailstorage_init(m_nntp,(char*)server, port, NULL, CONNECTION_TYPE_PLAIN, NNTP_AUTH_TYPE_PLAIN,
                          (char*)user,(char*)pass,0,0,0);

    err = mailstorage_connect( m_nntp );

   if (err != NEWSNNTP_NO_ERROR) {
        qDebug( QString( "FEHLERNUMMER %1" ).arg(  err ) );
     //   Global::statusMessage(tr("Error initializing folder"));
        mailstorage_free(m_nntp);
        m_nntp = 0;

    }

}

void NNTPwrapper::logout()
{
    int err = NEWSNNTP_NO_ERROR;
    if ( m_nntp == NULL )
        return;
    mailstorage_free(m_nntp);
   m_nntp = 0;
}

QValueList<Opie::Core::OSmartPointer<Folder> >* NNTPwrapper::listFolders() {

    QValueList<Opie::Core::OSmartPointer<Folder> >* folders = new QValueList<Opie::Core::OSmartPointer<Folder> >();
    QStringList groups;
    if (account) {
        groups = account->getGroups();
    }
    for ( QStringList::Iterator it = groups.begin(); it != groups.end(); ++it ) {
        folders->append(new Folder((*it),"."));
    }
    return folders;
}

/* we made this method in raw nntp access of etpan and not via generic interface
 * 'cause in that case there will be doubled copy operations. eg. the etpan would
 * copy that stuff into its own structures and we must copy it into useable c++
 * structures for our frontend. this would not make sense, so it is better to reimplement
 * the stuff from generic interface of etpan but copy it direct to qt classes.
 */
QStringList NNTPwrapper::listAllNewsgroups(const QString&mask) {
    login();
    QStringList res;
    clist *result = 0;
    clistcell *current = 0;
    newsnntp_group_description *group;

    if ( m_nntp )   {
        mailsession * session = m_nntp->sto_session;
        newsnntp * news =  ( (  nntp_session_state_data * )session->sess_data )->nntp_session;
        int err = NEWSNNTP_NO_ERROR;
        if (mask.isEmpty()) {
            err = newsnntp_list(news, &result);
        } else {
            /* taken from generic wrapper of etpan */
            QString nmask = mask+".*";
            err = newsnntp_list_active(news, nmask.latin1(), &result);
        }
        if ( err == NEWSNNTP_NO_ERROR && result) {
            for ( current=clist_begin(result);current!=NULL;current=clist_next(current) ) {
                group = (  newsnntp_group_description* ) current->data;
                if (!group||!group->grp_name||strlen(group->grp_name)==0) continue;
                res.append(group->grp_name);
            }
        }
    }
    if (result) {
        newsnntp_list_free(result);
    }
    return res;
}

void NNTPwrapper::answeredMail(const RecMailP&) {}

void NNTPwrapper::statusFolder(folderStat&target_stat,const QString&) {
    login();
    target_stat.message_count = 0;
    target_stat.message_unseen = 0;
    target_stat.message_recent = 0;
    if (!m_nntp)
        return;
    int r = mailsession_status_folder(m_nntp->sto_session,0,&target_stat.message_count,
                                      &target_stat.message_recent,&target_stat.message_unseen);
}


encodedString* NNTPwrapper::fetchRawBody(const RecMailP&mail) {
    char*target=0;
    size_t length=0;
    encodedString*res = 0;
    mailmessage * mailmsg = 0;
    int err = mailsession_get_message(m_nntp->sto_session, mail->getNumber(), &mailmsg);
    err = mailmessage_fetch(mailmsg,&target,&length);
    if (mailmsg)
        mailmessage_free(mailmsg);
    if (target) {
        res = new encodedString(target,length);
    }
    return res;
}

MAILLIB::ATYPE NNTPwrapper::getType()const {
    return account->getType();
}

const QString&NNTPwrapper::getName()const{
    return account->getAccountName();
}

void NNTPwrapper::deleteMail(const RecMailP&) {
}

int NNTPwrapper::deleteAllMail(const FolderP&) {
}
