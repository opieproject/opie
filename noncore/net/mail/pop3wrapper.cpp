#include <stdlib.h>
#include "pop3wrapper.h"
#include "mailtypes.h"
#include "logindialog.h"
#include <libetpan/mailpop3.h>
#include <libetpan/mailmime.h>
#include <libetpan/data_message_driver.h>
#include <qfile.h>

/* we don't fetch messages larger than 5 MB */
#define HARD_MSG_SIZE_LIMIT 5242880

POP3wrapper::POP3wrapper( POP3account *a )
{
    account = a;
    m_pop3 = NULL;
    msgTempName = a->getFileName()+"_msg_cache";
    last_msg_id = 0;
    bodyCache.clear();
}

POP3wrapper::~POP3wrapper()
{
    logout();
    QFile msg_cache(msgTempName);
    if (msg_cache.exists()) {
        msg_cache.remove();
    }
    cleanUpCache();
}

void POP3wrapper::cleanUpCache()
{
    QMap<QString,encodedString*>::Iterator it = bodyCache.begin();
    for (;it!=bodyCache.end();++it) {
        encodedString*t = it.data();
        //it.setValue(0);
        if (t) delete t;
    }
    bodyCache.clear();
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
    
    QFile msg_cache(msgTempName);

    if (mail.Msgsize()>HARD_MSG_SIZE_LIMIT) {
        qDebug("Message to large: %i",mail.Msgsize());
        return body;
    }
    cleanUpCache();
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
    body = parseMail(message);
    free(message);
    return body;
}

RecBody POP3wrapper::parseMail( char *message )
{
    int err = MAILIMF_NO_ERROR;
    /* these vars are used recurcive! set it to 0!!!!!!!!!!!!!!!!! */
    size_t curTok = 0;
    mailimf_message *result = 0;
    mailmessage * msg=0;
    mailmime_single_fields fields;
    
    /* is bound to msg and will be freed there */
    struct mailmime * mime=0;
    
    RecBody body;


    err = mailimf_message_parse( (char *) message, strlen( message ), &curTok, &result );
    if ( err != MAILIMF_NO_ERROR ) {
        if (result) mailimf_message_free(result);
        return body;
    }

    char*body_msg = message;
    msg = mailmessage_new();
    mailmessage_init(msg, NULL, data_message_driver, 0, strlen(body_msg));
    generic_message_t * msg_data;
    msg_data = (generic_message_t *)msg->msg_data;
    msg_data->msg_fetched = 1;
    msg_data->msg_message = body_msg;
    msg_data->msg_length = strlen(body_msg);


    memset(&fields, 0, sizeof(struct mailmime_single_fields));
    err = mailmessage_get_bodystructure(msg,&mime);
    traverseBody(body,msg,mime);

    mailmessage_free(msg);

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
}

RecMail *POP3wrapper::parseHeader( const char *header )
{
    int err = MAILIMF_NO_ERROR;
    size_t curTok = 0;
    RecMail *mail = new RecMail();
    mailimf_fields *fields;
    mailimf_references * refs;
    mailimf_keywords*keys;
    QString status;
    QString value;
    QBitArray mFlags(7);

    err = mailimf_fields_parse( (char *) header, strlen( header ), &curTok, &fields );
    for ( clistiter *current = clist_begin( fields->fld_list ); current != NULL; current = current->next ) {
        mailimf_field *field = (mailimf_field *) current->data;
        switch ( field->fld_type ) {
            case MAILIMF_FIELD_FROM:
                mail->setFrom( parseMailboxList( field->fld_data.fld_from->frm_mb_list ) );
                break;
            case MAILIMF_FIELD_TO:
                mail->setTo( parseAddressList( field->fld_data.fld_to->to_addr_list ) );
                break;
            case MAILIMF_FIELD_CC:
                mail->setCC( parseAddressList( field->fld_data.fld_cc->cc_addr_list ) );
                break;
            case MAILIMF_FIELD_BCC:
                mail->setBcc( parseAddressList( field->fld_data.fld_bcc->bcc_addr_list ) );
                break;
            case MAILIMF_FIELD_SUBJECT:
                mail->setSubject(convert_String( field->fld_data.fld_subject->sbj_value ) );
                break;
            case MAILIMF_FIELD_ORIG_DATE:
                mail->setDate( parseDateTime( field->fld_data.fld_orig_date->dt_date_time ) );
                break;
            case MAILIMF_FIELD_MESSAGE_ID:
                mail->setMsgid(QString(field->fld_data.fld_message_id->mid_value));
                break;
            case MAILIMF_FIELD_REFERENCES:
                refs = field->fld_data.fld_references;
                if (refs && refs->mid_list && clist_count(refs->mid_list)) {
                    char * text = (char*)refs->mid_list->first->data;
                    mail->setReplyto(QString(text));
                }
                break;
            case MAILIMF_FIELD_KEYWORDS:
                keys = field->fld_data.fld_keywords;
                for (clistcell*cur = clist_begin(keys->kw_list);cur!=0;cur=clist_next(cur)) {
                    qDebug("Keyword: %s",(char*)cur->data);
                }
                break;
            case MAILIMF_FIELD_OPTIONAL_FIELD:
                status = field->fld_data.fld_optional_field->fld_name;
                value = field->fld_data.fld_optional_field->fld_value;
                if (status.lower()=="status") {
                    if (value.lower()=="ro") {
                        mFlags.setBit(FLAG_SEEN);
                    }
                } else if (status.lower()=="x-status") {
                    qDebug("X-Status: %s",value.latin1());
                    if (value.lower()=="a") {
                        mFlags.setBit(FLAG_ANSWERED);
                    }
                } else {
//                    qDebug("Optionales feld: %s -> %s)",field->fld_data.fld_optional_field->fld_name,
//                        field->fld_data.fld_optional_field->fld_value);
                }
                break;
            default:
                qDebug("Non parsed field");
                break;
        }
    }
    if (fields) mailimf_fields_free(fields);
    mail->setFlags(mFlags);
    return mail;
}

QString POP3wrapper::parseDateTime( mailimf_date_time *date )
{
    char tmp[23];
    
    snprintf( tmp, 23,  "%02i.%02i.%04i %02i:%02i:%02i %+05i", 
        date->dt_day, date->dt_month, date->dt_year, date->dt_hour, date->dt_min, date->dt_sec, date->dt_zone );
    
    return QString( tmp );
}

QString POP3wrapper::parseAddressList( mailimf_address_list *list )
{
    QString result( "" );

    bool first = true;
    for ( clistiter *current = clist_begin( list->ad_list ); current != NULL; current = current->next ) {
        mailimf_address *addr = (mailimf_address *) current->data;
        
        if ( !first ) {
            result.append( "," );
        } else {
            first = false;
        }

        switch ( addr->ad_type ) {
            case MAILIMF_ADDRESS_MAILBOX:
                result.append( parseMailbox( addr->ad_data.ad_mailbox ) );
                break;
            case MAILIMF_ADDRESS_GROUP:
                result.append( parseGroup( addr->ad_data.ad_group ) );
                break;
            default:
                qDebug( "POP3: unkown mailimf address type" );
                break;
        }
    }
    
    return result;
}

QString POP3wrapper::parseGroup( mailimf_group *group )
{
    QString result( "" );

    result.append( group->grp_display_name );
    result.append( ": " );

    if ( group->grp_mb_list != NULL ) {
        result.append( parseMailboxList( group->grp_mb_list ) );
    }

    result.append( ";" );
    
    return result;
}

QString POP3wrapper::parseMailbox( mailimf_mailbox *box )
{
    QString result( "" );

    if ( box->mb_display_name == NULL ) {
        result.append( box->mb_addr_spec );
    } else {
        result.append( convert_String(box->mb_display_name).latin1() );
        result.append( " <" );
        result.append( box->mb_addr_spec );
        result.append( ">" );
    }
        
    return result;
}

QString POP3wrapper::parseMailboxList( mailimf_mailbox_list *list )
{
    QString result( "" );

    bool first = true;
    for ( clistiter *current = clist_begin( list->mb_list ); current != NULL; current = current->next ) {
        mailimf_mailbox *box = (mailimf_mailbox *) current->data;

        if ( !first ) {
            result.append( "," );
        } else {
            first = false;
        }
       
        result.append( parseMailbox( box ) );
    }
    
    return result;
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

QString POP3wrapper::fetchTextPart(const RecMail&mail,const RecPart&part)
{
    encodedString*t = fetchDecodedPart(mail,part);
    QString text=t->Content();
    delete t;
    return text;
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

encodedString* POP3wrapper::fetchDecodedPart(const RecMail&,const RecPart&part)
{
    QMap<QString,encodedString*>::ConstIterator it = bodyCache.find(part.Identifier());
    if (it==bodyCache.end()) return new encodedString();
    encodedString*t = decode_String(it.data(),part.Encoding());
    return t;
}

encodedString* POP3wrapper::fetchRawPart(const RecMail&mail,const RecPart&part)
{
    QMap<QString,encodedString*>::ConstIterator it = bodyCache.find(part.Identifier());
    if (it==bodyCache.end()) return new encodedString();
    encodedString*t = it.data();
    return t;
}

void POP3wrapper::traverseBody(RecBody&target,mailmessage*message,mailmime*mime,unsigned int current_rec)
{
    if (current_rec >= 10) {
        qDebug("too deep recursion!");
    }
    if (!message || !mime) {
        return;
    }
    int r;
    char*data = 0;
    size_t len;
    clistiter * cur = 0;
    int res;
    QString b;
    RecPart part;
    
    switch (mime->mm_type) {
    case MAILMIME_SINGLE:
        r = mailmessage_fetch_section(message,mime,&data,&len);
        part.setSize(len);
        fillSingleBody(part,message,mime);
        if (part.Type()=="text" && target.Bodytext().isNull()) {
            encodedString*r = new encodedString();
            r->setContent(data,len);
            encodedString*res = decode_String(r,part.Encoding());
            b = QString(res->Content());
            delete r;
            delete res;
            target.setBodytext(b);
            target.setDescription(part);
        } else {
            b = gen_attachment_id();
            part.setIdentifier(b);
            bodyCache[b]=new encodedString(data,len);
            target.addPart(part);
        }
        break;
    case MAILMIME_MULTIPLE:
        for (cur = clist_begin(mime->mm_data.mm_multipart.mm_mp_list) ; cur != NULL ; cur = clist_next(cur)) {
            traverseBody(target,message, (mailmime*)clist_content(cur),current_rec+1);
        }
        break;
    case MAILMIME_MESSAGE:
        if (mime->mm_data.mm_message.mm_msg_mime != NULL) {
            traverseBody(target,message,mime->mm_data.mm_message.mm_msg_mime,current_rec+1);
        }
        break;
    }
}

QString POP3wrapper::getencoding(mailmime_mechanism*aEnc)
{
    QString enc="7bit";
    if (!aEnc) return enc;
    switch(aEnc->enc_type) {
    case MAILMIME_MECHANISM_7BIT:
        enc = "7bit";
        break;
    case MAILMIME_MECHANISM_8BIT:
        enc = "8bit";
        break;
    case MAILMIME_MECHANISM_BINARY:
        enc = "binary";
        break;
    case MAILMIME_MECHANISM_QUOTED_PRINTABLE:
        enc = "quoted-printable";
        break;
    case MAILMIME_MECHANISM_BASE64:
        enc = "base64";
        break;
    case MAILMIME_MECHANISM_TOKEN:
    default:
        if (aEnc->enc_token) {
            enc = QString(aEnc->enc_token);
        }
        break;
    }
    return enc;
}

void POP3wrapper::fillSingleBody(RecPart&target,mailmessage*,mailmime*mime)
{
    if (!mime) {
        return;
    }
    mailmime_field*field = 0;
    mailmime_single_fields fields;
    memset(&fields, 0, sizeof(struct mailmime_single_fields));
    if (mime->mm_mime_fields != NULL) {
        mailmime_single_fields_init(&fields, mime->mm_mime_fields,
            mime->mm_content_type);
    }
    
    mailmime_content*type = fields.fld_content;
    clistcell*current;
    if (!type) {
        target.setType("text");
        target.setSubtype("plain");
    } else {
        target.setSubtype(type->ct_subtype);
        switch(type->ct_type->tp_data.tp_discrete_type->dt_type) {
        case MAILMIME_DISCRETE_TYPE_TEXT:
            target.setType("text");
            break;
        case MAILMIME_DISCRETE_TYPE_IMAGE:
            target.setType("image");
            break;
        case MAILMIME_DISCRETE_TYPE_AUDIO:
            target.setType("audio");
            break;
        case MAILMIME_DISCRETE_TYPE_VIDEO:
            target.setType("video");
            break;
        case MAILMIME_DISCRETE_TYPE_APPLICATION:
            target.setType("application");
            break;
        case MAILMIME_DISCRETE_TYPE_EXTENSION:
        default:
            if (type->ct_type->tp_data.tp_discrete_type->dt_extension) {
                target.setType(type->ct_type->tp_data.tp_discrete_type->dt_extension);
            }
            break;
        }
        if (type->ct_parameters) {
            fillParameters(target,type->ct_parameters);
        }
    }
    if (mime->mm_mime_fields && mime->mm_mime_fields->fld_list) {
        for (current=clist_begin(mime->mm_mime_fields->fld_list);current!=0;current=clist_next(current)) {
            field = (mailmime_field*)current->data;
            switch(field->fld_type) {
            case MAILMIME_FIELD_TRANSFER_ENCODING:
                target.setEncoding(getencoding(field->fld_data.fld_encoding));
                break;
            case MAILMIME_FIELD_ID:
                target.setIdentifier(field->fld_data.fld_id);
                break;
            case MAILMIME_FIELD_DESCRIPTION:
                target.setDescription(field->fld_data.fld_description);
                break;
            default:
                break;
            }
        }
    }
}

void POP3wrapper::fillParameters(RecPart&target,clist*parameters)
{
    if (!parameters) {return;}
    clistcell*current=0;
    mailmime_parameter*param;
    for (current=clist_begin(parameters);current!=0;current=clist_next(current)) {
        param = (mailmime_parameter*)current->data;
        if (param) {
            target.addParameter(QString(param->pa_name).lower(),QString(param->pa_value));
        }
    }
}
