#include "smtpwrapper.h"
#include "mailwrapper.h"
#include "abstractmail.h"
#include "logindialog.h"
#include "mailtypes.h"
#include "sendmailprogress.h"

#include <qt.h>

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

#include <libetpan/libetpan.h>


using namespace Opie::Core;
progressMailSend*SMTPwrapper::sendProgress = 0;

SMTPwrapper::SMTPwrapper(SMTPaccount * aSmtp )
    : Generatemail()
{
    m_SmtpAccount = aSmtp;
    Config cfg( "mail" );
    cfg.setGroup( "Status" );
    m_queuedMail =  cfg.readNumEntry( "outgoing", 0 );
    emit queuedMails( m_queuedMail );
    connect( this, SIGNAL( queuedMails(int) ), this, SLOT( emitQCop(int) )  );
    m_smtp = 0;
}

SMTPwrapper::~SMTPwrapper()
{
    disc_server();
}

void SMTPwrapper::emitQCop( int queued ) {
    QCopEnvelope env( "QPE/Pim", "outgoingMails(int)" );
    env << queued;
}

QString SMTPwrapper::mailsmtpError( int errnum ) {
    switch ( errnum ) {
    case MAILSMTP_NO_ERROR:
        return tr( "No error" );
    case MAILSMTP_ERROR_UNEXPECTED_CODE:
        return tr( "Unexpected error code" );
    case MAILSMTP_ERROR_SERVICE_NOT_AVAILABLE:
        return tr( "Service not available" );
    case MAILSMTP_ERROR_STREAM:
        return tr( "Stream error" );
    case MAILSMTP_ERROR_HOSTNAME:
        return tr( "gethostname() failed" );
    case MAILSMTP_ERROR_NOT_IMPLEMENTED:
        return tr( "Not implemented" );
    case MAILSMTP_ERROR_ACTION_NOT_TAKEN:
        return tr( "Error, action not taken" );
    case MAILSMTP_ERROR_EXCEED_STORAGE_ALLOCATION:
        return tr( "Data exceeds storage allocation" );
    case MAILSMTP_ERROR_IN_PROCESSING:
        return tr( "Error in processing" );
    case MAILSMTP_ERROR_STARTTLS_NOT_SUPPORTED:
        return tr( "Starttls not supported" );
        //      case MAILSMTP_ERROR_INSUFFISANT_SYSTEM_STORAGE:
        //        return tr( "Insufficient system storage" );
    case MAILSMTP_ERROR_MAILBOX_UNAVAILABLE:
        return tr( "Mailbox unavailable" );
    case MAILSMTP_ERROR_MAILBOX_NAME_NOT_ALLOWED:
        return tr( "Mailbox name not allowed" );
    case MAILSMTP_ERROR_BAD_SEQUENCE_OF_COMMAND:
        return tr( "Bad command sequence" );
    case MAILSMTP_ERROR_USER_NOT_LOCAL:
        return tr( "User not local" );
    case MAILSMTP_ERROR_TRANSACTION_FAILED:
        return tr( "Transaction failed" );
    case MAILSMTP_ERROR_MEMORY:
        return tr( "Memory error" );
    case MAILSMTP_ERROR_CONNECTION_REFUSED:
        return tr( "Connection refused" );
    default:
        return tr( "Unknown error code" );
    }
}


void SMTPwrapper::progress( size_t current, size_t maximum ) {
    if (SMTPwrapper::sendProgress) {
        SMTPwrapper::sendProgress->setSingleMail(current, maximum );
        qApp->processEvents();
    }
}

void SMTPwrapper::storeMail(const char*mail, size_t length, const QString&box) {
    if (!mail)
        return;
    QString localfolders = AbstractMail::defaultLocalfolder();
    AbstractMail*wrap = AbstractMail::getWrapper(localfolders);
    wrap->createMbox(box);
    wrap->storeMessage(mail,length,box);
    delete wrap;
}

void SMTPwrapper::smtpSend( mailmime *mail,bool later) {
    clist *rcpts = 0;
    char *from, *data;
    size_t size;

    from = data = 0;

    mailmessage * msg = 0;
    msg = mime_message_init(mail);
    mime_message_set_tmpdir(msg,getenv( "HOME" ));
    int r = mailmessage_fetch(msg,&data,&size);
    mime_message_detach_mime(msg);
    mailmessage_free(msg);
    if (r != MAIL_NO_ERROR || !data) {
        if (data)
            free(data);
        qDebug("Error fetching mime...");
        return;
    }
    msg = 0;
    if (later) {
        storeMail(data,size,"Outgoing");
        if (data)
            free( data );
        Config cfg( "mail" );
        cfg.setGroup( "Status" );
        cfg.writeEntry( "outgoing", ++m_queuedMail );
        emit queuedMails( m_queuedMail );
        return;
    }
    from = getFrom( mail );
    rcpts = createRcptList( mail->mm_data.mm_message.mm_fields );
    smtpSend(from,rcpts,data,size);
    if (data) {
        free(data);
    }
    if (from) {
        free(from);
    }
    if (rcpts)
        smtp_address_list_free( rcpts );
}

void SMTPwrapper::storeFailedMail(const char*data,unsigned int size, const char*failuremessage)
{
    if (data) {
        storeMail(data,size,"Sendfailed");
    }
    if (failuremessage) {
        QMessageBox::critical(0,tr("Error sending mail"),
            tr("<center>%1</center>").arg(failuremessage));
    }
}

int SMTPwrapper::start_smtp_tls()
{
    if (!m_smtp) {
        return MAILSMTP_ERROR_IN_PROCESSING;
    }
    int err = mailesmtp_starttls(m_smtp);
    if (err != MAILSMTP_NO_ERROR) return err;
    mailstream_low * low;
    mailstream_low * new_low;
    low = mailstream_get_low(m_smtp->stream);
    if (!low) {
        return MAILSMTP_ERROR_IN_PROCESSING;
    }
    int fd = mailstream_low_get_fd(low);
    if (fd > -1 && (new_low = mailstream_low_ssl_open(fd))!=0) {
        mailstream_low_free(low);
        mailstream_set_low(m_smtp->stream, new_low);
    } else {
        return MAILSMTP_ERROR_IN_PROCESSING;
    }
    return err;
}

void SMTPwrapper::connect_server()
{
    const char *server, *user, *pass;
    bool ssl;
    uint16_t port;
    ssl = false;
    bool try_tls = true;
    bool force_tls=false;
    server = user = pass = 0;
    QString failuretext = "";

    if (m_smtp || !m_SmtpAccount) {
        return;
    }
    server = m_SmtpAccount->getServer().latin1();
    if  ( m_SmtpAccount->ConnectionType() == 2 ) {
        ssl = true;
        try_tls = false;
    } else if (m_SmtpAccount->ConnectionType() == 1) {
        force_tls = true;
    }
    int result = 1;
    port = m_SmtpAccount->getPort().toUInt();

    m_smtp = mailsmtp_new( 20, &progress );
    if ( m_smtp == NULL ) {
        /* no failure message cause this happens when problems with memory - than we
           we can not display any messagebox */
        return;
    }

    int err = MAILSMTP_NO_ERROR;
    qDebug( "Servername %s at port %i", server, port );
    if ( ssl ) {
        qDebug( "SSL session" );
        err = mailsmtp_ssl_connect( m_smtp, server, port );
    } else {
        qDebug( "No SSL session" );
        err = mailsmtp_socket_connect( m_smtp, server, port );
    }
    if ( err != MAILSMTP_NO_ERROR ) {
        qDebug("Error init connection");
        failuretext = tr("Error init SMTP connection: %1").arg(mailsmtpError(err));
        result = 0;
    }

    /* switch to tls after init 'cause there it will send the ehlo */
    if (result) {
        err = mailsmtp_init( m_smtp );
        if (err != MAILSMTP_NO_ERROR) {
            result = 0;
            failuretext = tr("Error init SMTP connection: %1").arg(mailsmtpError(err));
        }
    }

    if (try_tls) {
        err = start_smtp_tls();
        if (err != MAILSMTP_NO_ERROR) {
            try_tls = false;
        } else {
            err = mailesmtp_ehlo(m_smtp);
        }
    }

    if (!try_tls && force_tls) {
        result = 0;
        failuretext = tr("Error init SMTP tls: %1").arg(mailsmtpError(err));
    }

    if (result==1 && m_SmtpAccount->getLogin() ) {
        qDebug("smtp with auth");
        if ( m_SmtpAccount->getUser().isEmpty() || m_SmtpAccount->getPassword().isEmpty() ) {
            // get'em
            LoginDialog login( m_SmtpAccount->getUser(),
                m_SmtpAccount->getPassword(), NULL, 0, true );
            login.show();
            if ( QDialog::Accepted == login.exec() ) {
                // ok
                user = login.getUser().latin1();
                pass = login.getPassword().latin1();
            } else {
                result = 0;
                failuretext=tr("Login aborted - storing mail to localfolder");
            }
        } else {
            user = m_SmtpAccount->getUser().latin1();
            pass = m_SmtpAccount->getPassword().latin1();
        }
        qDebug( "session->auth: %i", m_smtp->auth);
        if (result) {
            err = mailsmtp_auth( m_smtp, (char*)user, (char*)pass );
            if ( err == MAILSMTP_NO_ERROR ) {
                qDebug("auth ok");
            } else {
                failuretext = tr("Authentification failed");
                result = 0;
            }
        }
    }
}

void SMTPwrapper::disc_server()
{
    if (m_smtp) {
        mailsmtp_quit( m_smtp );
        mailsmtp_free( m_smtp );
        m_smtp = 0;
    }
}

int SMTPwrapper::smtpSend(char*from,clist*rcpts,const char*data,size_t size )
{
    int err,result;
    QString failuretext = "";

    connect_server();

    result = 1;
    if (m_smtp) {
        err = mailsmtp_send( m_smtp, from, rcpts, data, size );
        if ( err != MAILSMTP_NO_ERROR ) {
            failuretext=tr("Error sending mail: %1").arg(mailsmtpError(err));
            result = 0;
        }
    } else {
        result = 0;
    }

    if (!result) {
        storeFailedMail(data,size,failuretext);
    } else {
        qDebug( "Mail sent." );
        storeMail(data,size,"Sent");
    }
    return result;
}

void SMTPwrapper::sendMail(const Opie::Core::OSmartPointer<Mail>&mail,bool later )
{
    mailmime * mimeMail;

    mimeMail = createMimeMail(mail );
    if ( mimeMail == NULL ) {
        qDebug( "sendMail: error creating mime mail" );
    } else {
        sendProgress = new progressMailSend();
        sendProgress->show();
        sendProgress->setMaxMails(1);
        smtpSend( mimeMail,later);
        qDebug("Clean up done");
        sendProgress->hide();
        delete sendProgress;
        sendProgress = 0;
        mailmime_free( mimeMail );
    }
}

int SMTPwrapper::sendQueuedMail(AbstractMail*wrap,const RecMailP&which) {
    size_t curTok = 0;
    mailimf_fields *fields = 0;
    mailimf_field*ffrom = 0;
    clist *rcpts = 0;
    char*from = 0;
    int res = 0;

    encodedString * data = wrap->fetchRawBody(which);
    if (!data)
        return 0;
    int err = mailimf_fields_parse( data->Content(), data->Length(), &curTok, &fields );
    if (err != MAILIMF_NO_ERROR) {
        delete data;
        delete wrap;
        return 0;
    }

    rcpts = createRcptList( fields );
    ffrom = getField(fields, MAILIMF_FIELD_FROM );
    from = getFrom(ffrom);

    if (rcpts && from) {
        res = smtpSend(from,rcpts,data->Content(),data->Length());
    }
    if (fields) {
        mailimf_fields_free(fields);
        fields = 0;
    }
    if (data) {
        delete data;
    }
    if (from) {
        free(from);
    }
    if (rcpts) {
        smtp_address_list_free( rcpts );
    }
    return res;
}

/* this is a special fun */
bool SMTPwrapper::flushOutbox() {
    bool returnValue = true;

    qDebug("Sending the queue");
    if (!m_SmtpAccount) {
        qDebug("No smtp account given");
        return false;
    }

    bool reset_user_value = false;
    QString localfolders = AbstractMail::defaultLocalfolder();
    AbstractMail*wrap = AbstractMail::getWrapper(localfolders);
    if (!wrap) {
        qDebug("memory error");
        return false;
    }
    QString oldPw, oldUser;
    QValueList<RecMailP> mailsToSend;
    QValueList<RecMailP> mailsToRemove;
    QString mbox("Outgoing");
    wrap->listMessages(mbox,mailsToSend);
    if (mailsToSend.count()==0) {
        delete wrap;
        qDebug("No mails to send");
        return false;
    }

    oldPw = m_SmtpAccount->getPassword();
    oldUser = m_SmtpAccount->getUser();
    if (m_SmtpAccount->getLogin() && (m_SmtpAccount->getUser().isEmpty() || m_SmtpAccount->getPassword().isEmpty()) ) {
        // get'em
        QString user,pass;
        LoginDialog login( m_SmtpAccount->getUser(), m_SmtpAccount->getPassword(), NULL, 0, true );
        login.show();
        if ( QDialog::Accepted == login.exec() ) {
            // ok
            user = login.getUser().latin1();
            pass = login.getPassword().latin1();
            reset_user_value = true;
            m_SmtpAccount->setUser(user);
            m_SmtpAccount->setPassword(pass);
        } else {
            return true;
        }
    }


    sendProgress = new progressMailSend();
    sendProgress->show();
    sendProgress->setMaxMails(mailsToSend.count());

    while (mailsToSend.count()>0) {
        if (sendQueuedMail(wrap, (*mailsToSend.begin()))==0) {
            QMessageBox::critical(0,tr("Error sending mail"),
                                  tr("Error sending queued mail - breaking"));
            returnValue = false;
            break;
        }
        mailsToRemove.append((*mailsToSend.begin()));
        mailsToSend.remove(mailsToSend.begin());
        sendProgress->setCurrentMails(mailsToRemove.count());
    }
    if (reset_user_value) {
        m_SmtpAccount->setUser(oldUser);
        m_SmtpAccount->setPassword(oldPw);
    }
    Config cfg( "mail" );
    cfg.setGroup( "Status" );
    m_queuedMail = 0;
    cfg.writeEntry( "outgoing", m_queuedMail );
    emit queuedMails( m_queuedMail );
    sendProgress->hide();
    delete sendProgress;
    sendProgress = 0;
    wrap->deleteMails(mbox,mailsToRemove);
    delete wrap;
    return returnValue;
}
