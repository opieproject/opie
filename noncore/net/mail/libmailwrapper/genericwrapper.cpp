#include "genericwrapper.h"
#include <libetpan/libetpan.h>
#include "mailtypes.h"

Genericwrapper::Genericwrapper()
    : AbstractMail()
{
    bodyCache.clear();
}

Genericwrapper::~Genericwrapper()
{
    cleanMimeCache();
}

void Genericwrapper::fillSingleBody(RecPart&target,mailmessage*,mailmime*mime)
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

void Genericwrapper::fillParameters(RecPart&target,clist*parameters)
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

QString Genericwrapper::getencoding(mailmime_mechanism*aEnc)
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

void Genericwrapper::traverseBody(RecBody&target,mailmessage*message,mailmime*mime,QValueList<int>recList,unsigned int current_rec,int current_count)
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
    QString b;
    RecPart part;
    
    switch (mime->mm_type) {
    case MAILMIME_SINGLE:
    {
        QValueList<int>countlist = recList;
        countlist.append(current_count);
        r = mailmessage_fetch_section(message,mime,&data,&len);
        part.setSize(len);
        part.setPositionlist(countlist);
        b = gen_attachment_id();
        part.setIdentifier(b);
        fillSingleBody(part,message,mime);
        if (part.Type()=="text" && target.Bodytext().isNull()) {
            encodedString*r = new encodedString();
            r->setContent(data,len);
            encodedString*res = decode_String(r,part.Encoding());
            if (countlist.count()>2) {
                bodyCache[b]=r;
                target.addPart(part);
            } else {
                delete r;
            }
            b = QString(res->Content());
            delete res;
            target.setBodytext(b);
            target.setDescription(part);
        } else {
            bodyCache[b]=new encodedString(data,len);
            target.addPart(part);
        }
    }
    break;
    case MAILMIME_MULTIPLE:
    {
        unsigned int ccount = 1;
        mailmime*cbody=0;
        QValueList<int>countlist = recList;
        for (cur = clist_begin(mime->mm_data.mm_multipart.mm_mp_list) ; cur != NULL ; cur = clist_next(cur)) {
            cbody = (mailmime*)clist_content(cur);
            if (cbody->mm_type==MAILMIME_MULTIPLE) {
                RecPart targetPart;
                targetPart.setType("multipart");
                countlist.append(current_count);
                targetPart.setPositionlist(countlist);
                target.addPart(targetPart);
            }
            traverseBody(target,message, cbody,countlist,current_rec+1,ccount);
            if (cbody->mm_type==MAILMIME_MULTIPLE) {
                countlist = recList;
            }
            ++ccount;
        }
    }
    break;
    case MAILMIME_MESSAGE:
    {
        QValueList<int>countlist = recList;
        countlist.append(current_count);
        /* the own header is always at recursion 0 - we don't need that */
        if (current_rec > 0) {
            part.setPositionlist(countlist);
            r = mailmessage_fetch_section(message,mime,&data,&len);
            part.setSize(len);
            part.setPositionlist(countlist);
            b = gen_attachment_id();
            part.setIdentifier(b);
            part.setType("message");
            part.setSubtype("rfc822");
            bodyCache[b]=new encodedString(data,len);
            target.addPart(part);
        }
        if (mime->mm_data.mm_message.mm_msg_mime != NULL) {
            traverseBody(target,message,mime->mm_data.mm_message.mm_msg_mime,countlist,current_rec+1);
        }
    }
    break;
    }
}

RecBody Genericwrapper::parseMail( mailmessage * msg )
{
    int err = MAILIMF_NO_ERROR;
    mailmime_single_fields fields;
    /* is bound to msg and will be freed there */
    mailmime * mime=0;
    RecBody body;
    memset(&fields, 0, sizeof(struct mailmime_single_fields));
    err = mailmessage_get_bodystructure(msg,&mime);
    QValueList<int>recList;
    traverseBody(body,msg,mime,recList);
    return body;
}

RecMail *Genericwrapper::parseHeader( const char *header )
{
    int err = MAILIMF_NO_ERROR;
    size_t curTok = 0;
    RecMail *mail = new RecMail();
    mailimf_fields *fields = 0;
    mailimf_references * refs = 0;
    mailimf_keywords*keys = 0;
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

QString Genericwrapper::parseDateTime( mailimf_date_time *date )
{
    char tmp[23];
    
    snprintf( tmp, 23,  "%02i.%02i.%04i %02i:%02i:%02i %+05i", 
        date->dt_day, date->dt_month, date->dt_year, date->dt_hour, date->dt_min, date->dt_sec, date->dt_zone );
    
    return QString( tmp );
}

QString Genericwrapper::parseAddressList( mailimf_address_list *list )
{
    QString result( "" );

    bool first = true;
    if (list == 0) return result;
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
                qDebug( "Generic: unkown mailimf address type" );
                break;
        }
    }
    
    return result;
}

QString Genericwrapper::parseGroup( mailimf_group *group )
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

QString Genericwrapper::parseMailbox( mailimf_mailbox *box )
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

QString Genericwrapper::parseMailboxList( mailimf_mailbox_list *list )
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

encodedString* Genericwrapper::fetchDecodedPart(const RecMail&,const RecPart&part)
{
    QMap<QString,encodedString*>::ConstIterator it = bodyCache.find(part.Identifier());
    if (it==bodyCache.end()) return new encodedString();
    encodedString*t = decode_String(it.data(),part.Encoding());
    return t;
}

encodedString* Genericwrapper::fetchRawPart(const RecMail&mail,const RecPart&part)
{
    QMap<QString,encodedString*>::ConstIterator it = bodyCache.find(part.Identifier());
    if (it==bodyCache.end()) return new encodedString();
    encodedString*t = it.data();
    return t;
}

QString Genericwrapper::fetchTextPart(const RecMail&mail,const RecPart&part)
{
    encodedString*t = fetchDecodedPart(mail,part);
    QString text=t->Content();
    delete t;
    return text;
}

void Genericwrapper::cleanMimeCache()
{
    QMap<QString,encodedString*>::Iterator it = bodyCache.begin();
    for (;it!=bodyCache.end();++it) {
        encodedString*t = it.data();
        //it.setValue(0);
        if (t) delete t;
    }
    bodyCache.clear();
    qDebug("Genericwrapper: cache cleaned");
}

void Genericwrapper::parseList(QList<RecMail> &target,mailsession*session,const QString&mailbox)
{
    int r;
    mailmessage_list * env_list = 0;
    r = mailsession_get_messages_list(session,&env_list);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error message list");
        return;
    }
    r = mailsession_get_envelopes_list(session, env_list);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error filling message list");
        if (env_list) {
            mailmessage_list_free(env_list);
        }
        return;
    }
    mailimf_references * refs;
    uint32_t i = 0;
    for(; i < carray_count(env_list->msg_tab) ; ++i) {
        mailmessage * msg;
        QBitArray mFlags(7);
        msg = (mailmessage*)carray_get(env_list->msg_tab, i);
        if (msg->msg_fields == NULL) {
            qDebug("could not fetch envelope of message %i", i);
            continue;
        }
        RecMail * mail = new RecMail();
        mail->setWrapper(this);
        mail_flags * flag_result = 0;
        r = mailmessage_get_flags(msg,&flag_result);
        if (r == MAIL_ERROR_NOT_IMPLEMENTED) {
            mFlags.setBit(FLAG_SEEN);
        }
        mailimf_single_fields single_fields;
        mailimf_single_fields_init(&single_fields, msg->msg_fields);
        mail->setMsgsize(msg->msg_size);
        mail->setFlags(mFlags);
        mail->setMbox(mailbox);
        mail->setNumber(msg->msg_index);
        if (single_fields.fld_subject)
            mail->setSubject( convert_String(single_fields.fld_subject->sbj_value));
        if (single_fields.fld_from)
            mail->setFrom(parseMailboxList(single_fields.fld_from->frm_mb_list));
        if (single_fields.fld_to)
            mail->setTo( parseAddressList( single_fields.fld_to->to_addr_list ) );
        if (single_fields.fld_cc)
            mail->setCC( parseAddressList( single_fields.fld_cc->cc_addr_list ) );
        if (single_fields.fld_bcc)
            mail->setBcc( parseAddressList( single_fields.fld_bcc->bcc_addr_list ) );
        if (single_fields.fld_orig_date)
            mail->setDate( parseDateTime( single_fields.fld_orig_date->dt_date_time ) );
		// crashes when accessing pop3 account
		//        if (single_fields.fld_message_id->mid_value)
        //    mail->setMsgid(QString(single_fields.fld_message_id->mid_value));
        refs = single_fields.fld_references;
        if (refs && refs->mid_list && clist_count(refs->mid_list)) {
            char * text = (char*)refs->mid_list->first->data;
            mail->setReplyto(QString(text));
        }
        target.append(mail);
    }
    if (env_list) {
        mailmessage_list_free(env_list);
    }
}
