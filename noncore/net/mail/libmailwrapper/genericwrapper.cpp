#include "genericwrapper.h"
#include <libetpan/libetpan.h>
#include "mailtypes.h"

#include <opie2/odebug.h>

using namespace Opie::Core;
Genericwrapper::Genericwrapper()
    : AbstractMail()
{
    bodyCache.clear();
    m_storage = 0;
    m_folder = 0;
}

Genericwrapper::~Genericwrapper()
{
    if (m_folder) {
        mailfolder_free(m_folder);
    }
    if (m_storage) {
        mailstorage_free(m_storage);
    }
    cleanMimeCache();
}

void Genericwrapper::fillSingleBody(RecPartP&target,mailmessage*,mailmime*mime)
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
        target->setType("text");
        target->setSubtype("plain");
    } else {
        target->setSubtype(type->ct_subtype);
        switch(type->ct_type->tp_data.tp_discrete_type->dt_type) {
        case MAILMIME_DISCRETE_TYPE_TEXT:
            target->setType("text");
            break;
        case MAILMIME_DISCRETE_TYPE_IMAGE:
            target->setType("image");
            break;
        case MAILMIME_DISCRETE_TYPE_AUDIO:
            target->setType("audio");
            break;
        case MAILMIME_DISCRETE_TYPE_VIDEO:
            target->setType("video");
            break;
        case MAILMIME_DISCRETE_TYPE_APPLICATION:
            target->setType("application");
            break;
        case MAILMIME_DISCRETE_TYPE_EXTENSION:
        default:
            if (type->ct_type->tp_data.tp_discrete_type->dt_extension) {
                target->setType(type->ct_type->tp_data.tp_discrete_type->dt_extension);
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
                target->setEncoding(getencoding(field->fld_data.fld_encoding));
                break;
            case MAILMIME_FIELD_ID:
                target->setIdentifier(field->fld_data.fld_id);
                break;
            case MAILMIME_FIELD_DESCRIPTION:
                target->setDescription(field->fld_data.fld_description);
                break;
            default:
                break;
            }
        }
    }
}

void Genericwrapper::fillParameters(RecPartP&target,clist*parameters)
{
    if (!parameters) {return;}
    clistcell*current=0;
    mailmime_parameter*param;
    for (current=clist_begin(parameters);current!=0;current=clist_next(current)) {
        param = (mailmime_parameter*)current->data;
        if (param) {
            target->addParameter(QString(param->pa_name).lower(),QString(param->pa_value));
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

void Genericwrapper::traverseBody(RecBodyP&target,mailmessage*message,mailmime*mime,QValueList<int>recList,unsigned int current_rec,int current_count)
{
    if (current_rec >= 10) {
        odebug << "too deep recursion!" << oendl; 
    }
    if (!message || !mime) {
        return;
    }
    int r;
    char*data = 0;
    size_t len;
    clistiter * cur = 0;
    QString b;
    RecPartP part = new RecPart();

    switch (mime->mm_type) {
    case MAILMIME_SINGLE:
    {
        QValueList<int>countlist = recList;
        countlist.append(current_count);
        r = mailmessage_fetch_section(message,mime,&data,&len);
        part->setSize(len);
        part->setPositionlist(countlist);
        b = gen_attachment_id();
        part->setIdentifier(b);
        fillSingleBody(part,message,mime);
        if (part->Type()=="text" && target->Bodytext().isNull()) {
            encodedString*rs = new encodedString();
            rs->setContent(data,len);
            encodedString*res = decode_String(rs,part->Encoding());
            if (countlist.count()>2) {
                bodyCache[b]=rs;
                target->addPart(part);
            } else {
                delete rs;
            }
            b = QString(res->Content());
            delete res;
            target->setBodytext(b);
            target->setDescription(part);
        } else {
            bodyCache[b]=new encodedString(data,len);
            target->addPart(part);
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
                RecPartP targetPart = new RecPart();
                targetPart->setType("multipart");
                countlist.append(current_count);
                targetPart->setPositionlist(countlist);
                target->addPart(targetPart);
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
            part->setPositionlist(countlist);
            r = mailmessage_fetch_section(message,mime,&data,&len);
            part->setSize(len);
            part->setPositionlist(countlist);
            b = gen_attachment_id();
            part->setIdentifier(b);
            part->setType("message");
            part->setSubtype("rfc822");
            bodyCache[b]=new encodedString(data,len);
            target->addPart(part);
        }
        if (mime->mm_data.mm_message.mm_msg_mime != NULL) {
            traverseBody(target,message,mime->mm_data.mm_message.mm_msg_mime,countlist,current_rec+1);
        }
    }
    break;
    }
}

RecBodyP Genericwrapper::parseMail( mailmessage * msg )
{
    int err = MAILIMF_NO_ERROR;
    mailmime_single_fields fields;
    /* is bound to msg and will be freed there */
    mailmime * mime=0;
    RecBodyP body = new RecBody();
    memset(&fields, 0, sizeof(struct mailmime_single_fields));
    err = mailmessage_get_bodystructure(msg,&mime);
    QValueList<int>recList;
    traverseBody(body,msg,mime,recList);
    return body;
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
                odebug << "Generic: unkown mailimf address type" << oendl; 
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

encodedString* Genericwrapper::fetchDecodedPart(const RecMailP&,const RecPartP&part)
{
    QMap<QString,encodedString*>::ConstIterator it = bodyCache.find(part->Identifier());
    if (it==bodyCache.end()) return new encodedString();
    encodedString*t = decode_String(it.data(),part->Encoding());
    return t;
}

encodedString* Genericwrapper::fetchRawPart(const RecMailP&,const RecPartP&part)
{
    QMap<QString,encodedString*>::ConstIterator it = bodyCache.find(part->Identifier());
    if (it==bodyCache.end()) return new encodedString();
    encodedString*t = it.data();
    return t;
}

QString Genericwrapper::fetchTextPart(const RecMailP&mail,const RecPartP&part)
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
    odebug << "Genericwrapper: cache cleaned" << oendl; 
}

QStringList Genericwrapper::parseInreplies(mailimf_in_reply_to * in_replies)
{
    QStringList res;
    if (!in_replies || !in_replies->mid_list) return res;
    clistiter * current = 0;
    for ( current = clist_begin( in_replies->mid_list ); current != NULL; current = current->next ) {
        QString h((char*)current->data);
        while (h.length()>0 && h[0]=='<') {
            h.remove(0,1);
        }
        while (h.length()>0 && h[h.length()-1]=='>') {
            h.remove(h.length()-1,1);
        }
        if (h.length()>0) {
            res.append(h);
        }
    }
    return res;
}

void Genericwrapper::parseList(QValueList<Opie::Core::OSmartPointer<RecMail> > &target,mailsession*session,const QString&mailbox,bool mbox_as_to)
{
    int r;
    mailmessage_list * env_list = 0;
    r = mailsession_get_messages_list(session,&env_list);
    if (r != MAIL_NO_ERROR) {
        odebug << "Error message list" << oendl; 
        return;
    }
    r = mailsession_get_envelopes_list(session, env_list);
    if (r != MAIL_NO_ERROR) {
        odebug << "Error filling message list" << oendl; 
        if (env_list) {
            mailmessage_list_free(env_list);
        }
        return;
    }
    mailimf_references * refs = 0;
    mailimf_in_reply_to * in_replies = 0;
    uint32_t i = 0;
    for(; i < carray_count(env_list->msg_tab) ; ++i) {
        mailmessage * msg;
        QBitArray mFlags(7);
        msg = (mailmessage*)carray_get(env_list->msg_tab, i);
        if (msg->msg_fields == NULL) {
            //odebug << "could not fetch envelope of message " << i << "" << oendl; 
            continue;
        }
        RecMailP mail = new RecMail();
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
        if (!mbox_as_to) {
            if (single_fields.fld_to)
                mail->setTo( parseAddressList( single_fields.fld_to->to_addr_list ) );
        } else {
            mail->setTo(mailbox);
        }
        if (single_fields.fld_cc)
            mail->setCC( parseAddressList( single_fields.fld_cc->cc_addr_list ) );
        if (single_fields.fld_bcc)
            mail->setBcc( parseAddressList( single_fields.fld_bcc->bcc_addr_list ) );
        if (single_fields.fld_orig_date)
            mail->setDate( parseDateTime( single_fields.fld_orig_date->dt_date_time ) );
        // crashes when accessing pop3 account?
        if (single_fields.fld_message_id->mid_value) {
            mail->setMsgid(QString(single_fields.fld_message_id->mid_value));
            odebug << "Msgid == " << mail->Msgid().latin1() << "" << oendl; 
        }

        if (single_fields.fld_reply_to) {
            QStringList t = parseAddressList(single_fields.fld_reply_to->rt_addr_list);
            if (t.count()>0) {
                mail->setReplyto(t[0]);
            }
        }
#if 0
        refs = single_fields.fld_references;
        if (refs && refs->mid_list && clist_count(refs->mid_list)) {
            char * text = (char*)refs->mid_list->first->data;
            mail->setReplyto(QString(text));
        }
#endif
        if (single_fields.fld_in_reply_to && single_fields.fld_in_reply_to->mid_list &&
                clist_count(single_fields.fld_in_reply_to->mid_list)) {
            mail->setInreply(parseInreplies(single_fields.fld_in_reply_to));
        }
        target.append(mail);
    }
    if (env_list) {
        mailmessage_list_free(env_list);
    }
}
