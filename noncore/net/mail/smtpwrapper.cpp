#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <qdir.h>
#include <qt.h>

#include <libetpan/libetpan.h>

#include "smtpwrapper.h"
#include "mailwrapper.h"
#include "mboxwrapper.h"
#include "logindialog.h"
#include "mailtypes.h"
#include "defines.h"
#include "sendmailprogress.h"

progressMailSend*SMTPwrapper::sendProgress = 0;

SMTPwrapper::SMTPwrapper( Settings *s )
    : QObject()
{
    settings = s;
}

QString SMTPwrapper::mailsmtpError( int errnum )
{
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

mailimf_mailbox *SMTPwrapper::newMailbox(const QString&name, const QString&mail )
{
    return mailimf_mailbox_new( strdup( name.latin1() ),
                                strdup( mail.latin1() ) );
}

mailimf_address_list *SMTPwrapper::parseAddresses(const QString&addr )
{
    mailimf_address_list *addresses;

    if ( addr.isEmpty() ) return NULL;

    addresses = mailimf_address_list_new_empty();

    QStringList list = QStringList::split( ',', addr );
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); it++ ) {
        char *str = strdup( (*it).latin1() );
        int err = mailimf_address_list_add_parse( addresses, str );
        if ( err != MAILIMF_NO_ERROR ) {
            qDebug( "Error parsing" );
            qDebug( *it );
            free( str );
        } else {
            qDebug( "Parse success! :)" );
        }
    }

    return addresses;
}

mailimf_fields *SMTPwrapper::createImfFields(const Mail&mail )
{
    mailimf_fields *fields;
    mailimf_field *xmailer;
    mailimf_mailbox *sender, *fromBox;
    mailimf_mailbox_list *from;
    mailimf_address_list *to, *cc, *bcc, *reply;
    char *subject = strdup( mail.getSubject().latin1() );
    int err;

    sender = newMailbox( mail.getName(), mail.getMail() );
    if ( sender == NULL ) goto err_free;

    fromBox = newMailbox( mail.getName(), mail.getMail() );
    if ( fromBox == NULL ) goto err_free_sender;

    from = mailimf_mailbox_list_new_empty();
    if ( from == NULL ) goto err_free_fromBox;

    err = mailimf_mailbox_list_add( from, fromBox );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_from;

    to = parseAddresses( mail.getTo() );
    if ( to == NULL ) goto err_free_from;

    cc = parseAddresses( mail.getCC() );
    bcc = parseAddresses( mail.getBCC() );
    reply = parseAddresses( mail.getReply() );

    fields = mailimf_fields_new_with_data( from, sender, reply, to, cc, bcc,
                                           NULL, NULL, subject );
    if ( fields == NULL ) goto err_free_reply;

    xmailer = mailimf_field_new_custom( strdup( "User-Agent" ),
                                        strdup( USER_AGENT ) );
    if ( xmailer == NULL ) goto err_free_fields;

    err = mailimf_fields_add( fields, xmailer );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_xmailer;

    return fields;      // Success :)

err_free_xmailer:
    mailimf_field_free( xmailer );
err_free_fields:
    mailimf_fields_free( fields );
err_free_reply:
    mailimf_address_list_free( reply );
    mailimf_address_list_free( bcc );
    mailimf_address_list_free( cc );
    mailimf_address_list_free( to );
err_free_from:
    mailimf_mailbox_list_free( from );
err_free_fromBox:
    mailimf_mailbox_free( fromBox );
err_free_sender:
    mailimf_mailbox_free( sender );
err_free:
    free( subject );
    qDebug( "createImfFields - error" );

    return NULL;        // Error :(
}

mailmime *SMTPwrapper::buildTxtPart(const QString&str )
{
    mailmime *txtPart;
    mailmime_fields *fields;
    mailmime_content *content;
    mailmime_parameter *param;
    char *txt = strdup( str.latin1() );
    int err;

    param = mailmime_parameter_new( strdup( "charset" ),
                                    strdup( "iso-8859-1" ) );
    if ( param == NULL ) goto err_free;

    content = mailmime_content_new_with_str( "text/plain" );
    if ( content == NULL ) goto err_free_param;

    err = clist_append( content->ct_parameters, param );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_content;

    fields = mailmime_fields_new_encoding( MAILMIME_MECHANISM_8BIT );
    if ( fields == NULL ) goto err_free_content;

    txtPart = mailmime_new_empty( content, fields );
    if ( txtPart == NULL ) goto err_free_fields;

    err = mailmime_set_body_text( txtPart, txt, strlen( txt ) );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_txtPart;

    return txtPart;     // Success :)

err_free_txtPart:
    mailmime_free( txtPart );
err_free_fields:
    mailmime_fields_free( fields );
err_free_content:
    mailmime_content_free( content );
err_free_param:
    mailmime_parameter_free( param );
err_free:
    free( txt );
    qDebug( "buildTxtPart - error" );

    return NULL;        // Error :(
}

mailmime *SMTPwrapper::buildFilePart(const QString&filename,const QString&mimetype,const QString&TextContent )
{
    mailmime * filePart = 0;
    mailmime_fields * fields = 0;
    mailmime_content * content = 0;
    mailmime_parameter * param = 0;
    char*name = 0;
    char*file = 0;
    int err;

    int pos = filename.findRev( '/' );

    if (filename.length()>0) {
        QString tmp = filename.right( filename.length() - ( pos + 1 ) );
        name = strdup( tmp.latin1() );        // just filename
        file = strdup( filename.latin1() );   // full name with path
    }
    char *mime = strdup( mimetype.latin1() );   // mimetype -e.g. text/plain

    int disptype = MAILMIME_DISPOSITION_TYPE_ATTACHMENT;
    int mechanism = MAILMIME_MECHANISM_BASE64;

    if ( mimetype.startsWith( "text/" ) ) {
        param = mailmime_parameter_new( strdup( "charset" ),
                                        strdup( "iso-8859-1" ) );
        disptype = MAILMIME_DISPOSITION_TYPE_INLINE;
        mechanism = MAILMIME_MECHANISM_QUOTED_PRINTABLE;
    }

    fields = mailmime_fields_new_filename(
                disptype, name,
                mechanism );
    content = mailmime_content_new_with_str( mime );
    if (content!=0 && fields != 0) {
        if (param) {
            clist_append(content->ct_parameters,param);
            param = 0;
        }
        if (filename.length()>0) {
            QFileInfo f(filename);
            param = mailmime_parameter_new(strdup("name"),strdup(f.fileName().latin1()));
            clist_append(content->ct_parameters,param);
            param = 0;
        }
        filePart = mailmime_new_empty( content, fields );
    }
    if (filePart) {
        if (filename.length()>0) {
            err = mailmime_set_body_file( filePart, file );
        } else {
            err = mailmime_set_body_text(filePart,strdup(TextContent.ascii()),TextContent.length());
        }
        if (err != MAILIMF_NO_ERROR) {
            qDebug("Error setting body with file %s",file);
            mailmime_free( filePart );
            filePart = 0;
        }
    }

    if (!filePart) {
        if ( param != NULL ) {
            mailmime_parameter_free( param );
        }
        if (content) {
            mailmime_content_free( content );
        } else {
            if (mime) {
                free( mime );
            }
        }
        if (fields) {
            mailmime_fields_free( fields );
        } else {
            if (name) {
                free( name );
            }
            if (file) {
                free( file );
            }
        }
    }
    return filePart;        // Success :)

}

void SMTPwrapper::addFileParts( mailmime *message,const QList<Attachment>&files )
{
    const Attachment *it;
    unsigned int count = files.count();
    qDebug("List contains %i values",count);
    for ( unsigned int i = 0; i < count; ++i ) {
        qDebug( "Adding file" );
        mailmime *filePart;
        int err;
        it = ((QList<Attachment>)files).at(i);

        filePart = buildFilePart( it->getFileName(), it->getMimeType(),"" );
        if ( filePart == NULL ) {
            qDebug( "addFileParts: error adding file:" );
            qDebug( it->getFileName() );
            continue;
        }
        err = mailmime_smart_add_part( message, filePart );
        if ( err != MAILIMF_NO_ERROR ) {
            mailmime_free( filePart );
            qDebug("error smart add");
        }
    }
}

mailmime *SMTPwrapper::createMimeMail(const Mail &mail )
{
    mailmime *message, *txtPart;
    mailimf_fields *fields;
    int err;

    fields = createImfFields( mail );
    if ( fields == NULL ) goto err_free;

    message = mailmime_new_message_data( NULL );
    if ( message == NULL ) goto err_free_fields;

    mailmime_set_imf_fields( message, fields );

    if (mail.getAttachments().count()==0) {
        txtPart = buildTxtPart( mail.getMessage() );
    } else {
        txtPart = buildFilePart("","text/plain",mail.getMessage());
    }
    if ( txtPart == NULL ) goto err_free_message;

    err = mailmime_smart_add_part( message, txtPart );
    if ( err != MAILIMF_NO_ERROR ) goto err_free_txtPart;

    addFileParts( message, mail.getAttachments() );

    return message;         // Success :)

err_free_txtPart:
    mailmime_free( txtPart );
err_free_message:
    mailmime_free( message );
err_free_fields:
    mailimf_fields_free( fields );
err_free:
    qDebug( "createMimeMail: error" );

    return NULL;            // Error :(
}

mailimf_field *SMTPwrapper::getField( mailimf_fields *fields, int type )
{
    mailimf_field *field;
    clistiter *it;

    it = clist_begin( fields->fld_list );
    while ( it ) {
        field = (mailimf_field *) it->data;
        if ( field->fld_type == type ) {
            return field;
        }
        it = it->next;
    }

    return NULL;
}

void SMTPwrapper::addRcpts( clist *list, mailimf_address_list *addr_list )
{
    clistiter *it, *it2;

    for ( it = clist_begin( addr_list->ad_list ); it; it = it->next ) {
        mailimf_address *addr;
        addr = (mailimf_address *) it->data;

        if ( addr->ad_type == MAILIMF_ADDRESS_MAILBOX ) {
    	    esmtp_address_list_add( list, addr->ad_data.ad_mailbox->mb_addr_spec, 0, NULL );
        } else if ( addr->ad_type == MAILIMF_ADDRESS_GROUP ) {
            clist *l = addr->ad_data.ad_group->grp_mb_list->mb_list;
    	    for ( it2 = clist_begin( l ); it2; it2 = it2->next ) {
                mailimf_mailbox *mbox;
    	        mbox = (mailimf_mailbox *) it2->data;
	            esmtp_address_list_add( list, mbox->mb_addr_spec, 0, NULL );
	        }
	    }
    }
}

clist *SMTPwrapper::createRcptList( mailimf_fields *fields )
{
    clist *rcptList;
    mailimf_field *field;

    rcptList = esmtp_address_list_new();

    field = getField( fields, MAILIMF_FIELD_TO );
    if ( field && (field->fld_type == MAILIMF_FIELD_TO)
         && field->fld_data.fld_to->to_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_to->to_addr_list );
    }

    field = getField( fields, MAILIMF_FIELD_CC );
    if ( field && (field->fld_type == MAILIMF_FIELD_CC)
         && field->fld_data.fld_cc->cc_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_cc->cc_addr_list );
    }

    field = getField( fields, MAILIMF_FIELD_BCC );
    if ( field && (field->fld_type == MAILIMF_FIELD_BCC)
         && field->fld_data.fld_bcc->bcc_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_bcc->bcc_addr_list );
    }

    return rcptList;
}

char *SMTPwrapper::getFrom( mailimf_field *ffrom)
{
    char *from = NULL;
    if ( ffrom && (ffrom->fld_type == MAILIMF_FIELD_FROM)
         && ffrom->fld_data.fld_from->frm_mb_list && ffrom->fld_data.fld_from->frm_mb_list->mb_list ) {
        clist *cl = ffrom->fld_data.fld_from->frm_mb_list->mb_list;
        clistiter *it;
        for ( it = clist_begin( cl ); it; it = it->next ) {
            mailimf_mailbox *mb = (mailimf_mailbox *) it->data;
            from = strdup( mb->mb_addr_spec );
        }
    }

    return from;
}

char *SMTPwrapper::getFrom( mailmime *mail )
{
    /* no need to delete - its just a pointer to structure content */
    mailimf_field *ffrom = 0;
    char*f = 0;
    ffrom = getField( mail->mm_data.mm_message.mm_fields, MAILIMF_FIELD_FROM );
    return getFrom(ffrom);
}

SMTPaccount *SMTPwrapper::getAccount(const QString&name )
{
    SMTPaccount *smtp;

    QList<Account> list = settings->getAccounts();
    Account *it;
    for ( it = list.first(); it; it = list.next() ) {
        if ( it->getType().compare( "SMTP" ) == 0 ) {
            smtp = static_cast<SMTPaccount *>(it);
            if ( smtp->getName()== name ) {
                qDebug( "SMTPaccount found for" );
                qDebug( name );
                return smtp;
            }
        }
    }

    return NULL;
}

QString SMTPwrapper::getTmpFile() {
    int num = 0;
    QString unique;

    QDir dir( "/tmp" );

    QStringList list = dir.entryList( "opiemail-tmp-*" );

    do {
        unique.setNum( num++ );
    } while ( list.contains( "opiemail-tmp-" + unique ) > 0 );

    return "/tmp/opiemail-tmp-" + unique;
}

void SMTPwrapper::writeToFile(const QString&file, mailmime *mail )
{
    FILE *f;
    int err, col = 0;

    f = fopen( file.latin1(), "w" );
    if ( f == NULL ) {
        qDebug( "writeToFile: error opening file" );
        return;
    }

    err = mailmime_write( f, &col, mail );
    if ( err != MAILIMF_NO_ERROR ) {
        fclose( f );
        qDebug( "writeToFile: error writing mailmime" );
        return;
    }

    fclose( f );
}

void SMTPwrapper::readFromFile(const QString&file, char **data, size_t *size )
{

    QFile msg_cache(file);
    QString msg = "";
    msg_cache.open(IO_ReadOnly);
    char*message = new char[4096];
    memset(message,0,4096);
    while (msg_cache.readBlock(message,4095)>0) {
         msg+=message;
         memset(message,0,4096);
    }
    delete message;
    *data = (char*)malloc(msg.length()+1*sizeof(char));
    memset(*data,0,msg.length()+1);
    memcpy(*data,msg.ascii(),msg.length());
    *size=msg.length();
}

void SMTPwrapper::progress( size_t current, size_t maximum )
{
    if (SMTPwrapper::sendProgress) {
        SMTPwrapper::sendProgress->setSingleMail(current, maximum );
        qApp->processEvents();
    }
}

void SMTPwrapper::storeMail(char*mail, size_t length, const QString&box)
{
    if (!mail) return;
    QString localfolders = (QString) getenv( "HOME" ) + QString("/Applications/opiemail/localmail/");
    MBOXwrapper*wrap = new MBOXwrapper(localfolders);
    wrap->storeMessage(mail,length,box);
    delete wrap;
}

void SMTPwrapper::smtpSend( mailmime *mail,bool later, SMTPaccount *smtp )
{
    clist *rcpts = 0;
    char *from, *data;
    size_t size;

    if ( smtp == NULL ) {
        return;
    }
    from = data = 0;

    QString file = getTmpFile();
    writeToFile( file, mail );
    readFromFile( file, &data, &size );
    QFile f( file );
    f.remove();

    if (later) {
        storeMail(data,size,"Outgoing");
        if (data) free( data );
        return;
    }
    from = getFrom( mail );
    rcpts = createRcptList( mail->mm_data.mm_message.mm_fields );
    smtpSend(from,rcpts,data,size,smtp);
    if (data) {free(data);}
    if (from) {free(from);}
    if (rcpts) smtp_address_list_free( rcpts );
}

int SMTPwrapper::smtpSend(char*from,clist*rcpts,char*data,size_t size, SMTPaccount *smtp )
{
    char *server, *user, *pass;
    bool ssl;
    uint16_t port;
    mailsmtp *session;
    int err,result;

    result = 1;
    server = user = pass = 0;
    server = strdup( smtp->getServer().latin1() );
    ssl = smtp->getSSL();
    port = smtp->getPort().toUInt();

    session = mailsmtp_new( 20, &progress );
    if ( session == NULL ) goto free_mem;

    qDebug( "Servername %s at port %i", server, port );
    if ( ssl ) {
        qDebug( "SSL session" );
        err = mailsmtp_ssl_connect( session, server, port );
    } else {
        qDebug( "No SSL session" );
        err = mailsmtp_socket_connect( session, server, port );
    }
    if ( err != MAILSMTP_NO_ERROR ) {result = 0;goto free_mem_session;}

    err = mailsmtp_init( session );
    if ( err != MAILSMTP_NO_ERROR ) {result = 0; goto free_con_session;}

    qDebug( "INIT OK" );

    if ( smtp->getLogin() ) {
        if ( smtp->getUser().isEmpty() || smtp->getPassword().isEmpty() ) {
            // get'em
            LoginDialog login( smtp->getUser(), smtp->getPassword(), NULL, 0, true );
            login.show();
            if ( QDialog::Accepted == login.exec() ) {
                // ok
                user = strdup( login.getUser().latin1() );
                pass = strdup( login.getPassword().latin1() );
            } else {
                result = 0; goto free_con_session;
            }
        } else {
            user = strdup( smtp->getUser().latin1() );
            pass = strdup( smtp->getPassword().latin1() );
        }
        qDebug( "session->auth: %i", session->auth);
        err = mailsmtp_auth( session, user, pass );
        if ( err == MAILSMTP_NO_ERROR ) qDebug("auth ok");
        qDebug( "Done auth!" );
    }

    err = mailsmtp_send( session, from, rcpts, data, size );
    if ( err != MAILSMTP_NO_ERROR ) {result = 0; goto free_con_session;}

    qDebug( "Mail sent." );
    storeMail(data,size,"Sent");

free_con_session:
    mailsmtp_quit( session );
free_mem_session:
    mailsmtp_free( session );
free_mem:
    if (server) free( server );
    if ( smtp->getLogin() ) {
        free( user );
        free( pass );
    }
    return result;
}

void SMTPwrapper::sendMail(const Mail&mail,bool later )
{
    mailmime * mimeMail;

    SMTPaccount *smtp = getAccount(mail.getName());

    mimeMail = createMimeMail(mail );
    if ( mimeMail == NULL ) {
        qDebug( "sendMail: error creating mime mail" );
    } else {
        sendProgress = new progressMailSend();
        sendProgress->show();
        sendProgress->setMaxMails(1);
        smtpSend( mimeMail,later,smtp);
        mailmime_free( mimeMail );
        qDebug("Clean up done");
        sendProgress->hide();
        delete sendProgress;
        sendProgress = 0;
    }
}

int SMTPwrapper::sendQueuedMail(MBOXwrapper*wrap,SMTPaccount*smtp,RecMail*which)
{
    char*data = 0;
    size_t length = 0;
    size_t curTok = 0;
    mailimf_fields *fields = 0;
    mailimf_field*ffrom = 0;
    clist *rcpts = 0;
    char*from = 0;
    int res = 0;

    wrap->fetchRawBody(*which,&data,&length);
    if (!data) return 0;
    int err = mailimf_fields_parse( data, length, &curTok, &fields );
    if (err != MAILIMF_NO_ERROR) {
        free(data);
        delete wrap;
        return 0;
    }

    rcpts = createRcptList( fields );
    ffrom = getField(fields, MAILIMF_FIELD_FROM );
    from = getFrom(ffrom);

    qDebug("Size: %i vs. %i",length,strlen(data));
    if (rcpts && from) {
        res = smtpSend(from,rcpts,data,length,smtp );
    }
    if (fields) {
        mailimf_fields_free(fields);
        fields = 0;
    }
    if (data) {
        free(data);
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
bool SMTPwrapper::flushOutbox(SMTPaccount*smtp)
{
    bool returnValue = true;

    if (!smtp) return false;

   QString localfolders = (QString) getenv( "HOME" ) + QString("/Applications/opiemail/localmail/");
    MBOXwrapper*wrap = new MBOXwrapper(localfolders);
    if (!wrap) {
        qDebug("memory error");
        return false;
    }
    QList<RecMail> mailsToSend;
    QList<RecMail> mailsToRemove;
    QString mbox("Outgoing");
    wrap->listMessages(mbox,mailsToSend);
    if (mailsToSend.count()==0) {
        delete wrap;
        return false;
    }
    mailsToSend.setAutoDelete(false);
    sendProgress = new progressMailSend();
    sendProgress->show();
    sendProgress->setMaxMails(mailsToSend.count());

    while (mailsToSend.count()>0) {
        if (sendQueuedMail(wrap,smtp,mailsToSend.at(0))==0) {
            QMessageBox::critical(0,tr("Error sending mail"),
                tr("Error sending queued mail - breaking"));

            returnValue = false;
            break;
        }
        mailsToRemove.append(mailsToSend.at(0));
        mailsToSend.removeFirst();
        sendProgress->setCurrentMails(mailsToRemove.count());
    }
    sendProgress->hide();
    delete sendProgress;
    sendProgress = 0;
    wrap->deleteMails(mbox,mailsToRemove);
    mailsToSend.setAutoDelete(true);
    delete wrap;
    return returnValue;
}
