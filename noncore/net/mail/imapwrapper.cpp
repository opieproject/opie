
#include <stdlib.h>

#include "imapwrapper.h"
#include "mailtypes.h"
#include <libetpan/mailimap.h>

IMAPwrapper::IMAPwrapper( IMAPaccount *a )
    : AbstractMail()
{
    account = a;
    m_imap = 0;
}

IMAPwrapper::~IMAPwrapper()
{
    logout();
}

void IMAPwrapper::imap_progress( size_t current, size_t maximum )
{
    qDebug( "IMAP: %i of %i", current, maximum );
}

void IMAPwrapper::login()
{
    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILIMAP_NO_ERROR;

    /* we are connected this moment */
    /* TODO: setup a timer holding the line or if connection closed - delete the value */
    if (m_imap) {
        mailstream_flush(m_imap->imap_stream);
        return;
    }
    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();

    m_imap = mailimap_new( 20, &imap_progress );
    /* connect */
    if (account->getSSL()) {
        err = mailimap_ssl_connect( m_imap, (char*)server, port );
    } else {
        err = mailimap_socket_connect( m_imap, (char*)server, port );
    }

    if ( err != MAILIMAP_NO_ERROR &&
         err != MAILIMAP_NO_ERROR_AUTHENTICATED &&
         err != MAILIMAP_NO_ERROR_NON_AUTHENTICATED ) {
         qDebug("error connecting server: %s",m_imap->imap_response);
         mailimap_free( m_imap );
         m_imap = 0;
         return;
    }

    /* login */
    err = mailimap_login_simple( m_imap, (char*)user, (char*)pass );
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error logging in imap: %s",m_imap->imap_response);
        err = mailimap_close( m_imap );
        mailimap_free( m_imap );
        m_imap = 0;
    }
}

void IMAPwrapper::logout()
{
    int err = MAILIMAP_NO_ERROR;
    if (!m_imap) return;
    err = mailimap_logout( m_imap );
    err = mailimap_close( m_imap );
    mailimap_free( m_imap );
    m_imap = 0;
}

void IMAPwrapper::listMessages(const QString&mailbox,QList<RecMail> &target )
{
    const char *mb;
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current;
//    mailimap_fetch_att *fetchAtt,*fetchAttFlags,*fetchAttDate,*fetchAttSize;
    mailimap_fetch_type *fetchType;
    mailimap_set *set;

    mb = mailbox.latin1();
    login();
    if (!m_imap) {
        return;
    }
    /* select mailbox READONLY for operations */
    err = mailimap_examine( m_imap, (char*)mb);
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox: %s",m_imap->imap_response);
        return;
    }

    int last = m_imap->imap_selection_info->sel_exists;

    if (last == 0) {
        qDebug("mailbox has no mails");
        return;
    }

    result = clist_new();
    /* the range has to start at 1!!! not with 0!!!! */
    set = mailimap_set_new_interval( 1, last );
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType,mailimap_fetch_att_new_envelope());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType,mailimap_fetch_att_new_flags());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType,mailimap_fetch_att_new_internaldate());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType,mailimap_fetch_att_new_rfc822_size());

    err = mailimap_fetch( m_imap, set, fetchType, &result );
    mailimap_set_free( set );
    mailimap_fetch_type_free( fetchType );

    QString date,subject,from;

    if ( err == MAILIMAP_NO_ERROR ) {
        
        mailimap_msg_att * msg_att;
        int i = 0;
        for (current = clist_begin(result); current != 0; current=clist_next(current)) {
            ++i;
            msg_att = (mailimap_msg_att*)current->data;
            RecMail*m = parse_list_result(msg_att);
            if (m) {
                m->setNumber(i);
                m->setMbox(mailbox);
                m->setWrapper(this);
                target.append(m);
            }
        }
    } else {
        qDebug("Error fetching headers: %s",m_imap->imap_response);
    }
    mailimap_fetch_list_free(result);
}

QList<Folder>* IMAPwrapper::listFolders()
{
    const char *path, *mask;
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current;

    QList<Folder> * folders = new QList<Folder>();
    folders->setAutoDelete( false );
    login();
    if (!m_imap) {
        return folders;
    }

/*
 * First we have to check for INBOX 'cause it sometimes it's not inside the path.
 * We must not forget to filter them out in next loop!
 * it seems like ugly code. and yes - it is ugly code. but the best way.
 */
    QString temp;
    mask = "INBOX" ;
    result = clist_new();
    mailimap_mailbox_list *list;
    err = mailimap_list( m_imap, (char*)"", (char*)mask, &result );
    if ( err == MAILIMAP_NO_ERROR ) {
        current = result->first;
        for ( int i = result->count; i > 0; i-- ) {
            list = (mailimap_mailbox_list *) current->data;
            // it is better use the deep copy mechanism of qt itself
            // instead of using strdup!
            temp = list->mb_name;
            folders->append( new IMAPFolder(temp));
            current = current->next;
        }
    } else {
        qDebug("error fetching folders: %s",m_imap->imap_response);
    }
    mailimap_list_result_free( result );

/*
 * second stage - get the other then inbox folders
 */
    mask = "*" ;
    path = account->getPrefix().latin1();
    if (!path) path = "";
    result = clist_new();
    qDebug(path);
    bool selectable = true;
    mailimap_mbx_list_flags*bflags;
    err = mailimap_list( m_imap, (char*)path, (char*)mask, &result );
    if ( err == MAILIMAP_NO_ERROR ) {
        current = result->first;
        for ( current=clist_begin(result);current!=NULL;current=clist_next(current)) {
            list = (mailimap_mailbox_list *) current->data;
            // it is better use the deep copy mechanism of qt itself
            // instead of using strdup!
            temp = list->mb_name;
            if (temp.lower()=="inbox")
                continue;
            if (temp.lower()==account->getPrefix().lower())
                continue;
            if ( (bflags = list->mb_flag) ) {
                selectable = !(bflags->mbf_type==MAILIMAP_MBX_LIST_FLAGS_SFLAG&&
                            bflags->mbf_sflag==MAILIMAP_MBX_LIST_SFLAG_NOSELECT);
            }
            folders->append(new IMAPFolder(temp,selectable,account->getPrefix()));
        }
    } else {
        qDebug("error fetching folders %s",m_imap->imap_response);
    }
    mailimap_list_result_free( result );
    return folders;
}

RecMail*IMAPwrapper::parse_list_result(mailimap_msg_att* m_att)
{
    RecMail * m = 0;
    mailimap_msg_att_item *item=0;
    clistcell *current,*c,*cf;
    mailimap_msg_att_dynamic*flist;
    mailimap_flag_fetch*cflag;
    int size;
    QBitArray mFlags(7);
    QStringList addresslist;

    if (!m_att) {
        return m;
    }
    m = new RecMail();
    for (c = clist_begin(m_att->att_list); c!=NULL;c=clist_next(c) ) {
        current = c;
        size = 0;
        item = (mailimap_msg_att_item*)current->data;
        if (item->att_type!=MAILIMAP_MSG_ATT_ITEM_STATIC) {
            flist = (mailimap_msg_att_dynamic*)item->att_data.att_dyn;
            if (!flist->att_list) {
                continue;
            }
            cf = flist->att_list->first;
            for (cf = clist_begin(flist->att_list); cf!=NULL; cf = clist_next(cf)) {
                cflag = (mailimap_flag_fetch*)cf->data;
                if (cflag->fl_type==MAILIMAP_FLAG_FETCH_OTHER && cflag->fl_flag!=0) {
                    switch (cflag->fl_flag->fl_type) {
                    case MAILIMAP_FLAG_ANSWERED:  /* \Answered flag */
                        mFlags.setBit(FLAG_ANSWERED);
                        break;
                    case MAILIMAP_FLAG_FLAGGED:   /* \Flagged flag */
                        mFlags.setBit(FLAG_FLAGGED);
                        break;
                    case MAILIMAP_FLAG_DELETED:   /* \Deleted flag */
                        mFlags.setBit(FLAG_DELETED);
                        break;
                    case MAILIMAP_FLAG_SEEN:      /* \Seen flag */
                        mFlags.setBit(FLAG_SEEN);
                        break;
                    case MAILIMAP_FLAG_DRAFT:     /* \Draft flag */
                        mFlags.setBit(FLAG_DRAFT);
                        break;
                    case MAILIMAP_FLAG_KEYWORD:   /* keyword flag */
                        break;
                    case MAILIMAP_FLAG_EXTENSION: /* \extension flag */
                        break;
                    default:
                        break;
                    }
                } else if (cflag->fl_type==MAILIMAP_FLAG_FETCH_RECENT) {
                    mFlags.setBit(FLAG_RECENT);
                }
            }
            continue;
        }
        if (item->att_data.att_static->att_type==MAILIMAP_MSG_ATT_ENVELOPE) {
            mailimap_envelope * head = item->att_data.att_static->att_data.att_env;
            m->setDate(head->env_date);
            m->setSubject(head->env_subject);
            if (head->env_from!=NULL) {
                addresslist = address_list_to_stringlist(head->env_from->frm_list);
                if (addresslist.count()) {
                    m->setFrom(addresslist.first());
                }
            }
            if (head->env_to!=NULL) {
                addresslist = address_list_to_stringlist(head->env_to->to_list);
                m->setTo(addresslist);
            }
            if (head->env_cc!=NULL) {
                addresslist = address_list_to_stringlist(head->env_cc->cc_list);
                m->setCC(addresslist);
            }
            if (head->env_bcc!=NULL) {
                addresslist = address_list_to_stringlist(head->env_bcc->bcc_list);
                m->setBcc(addresslist);
            }
            if (head->env_reply_to!=NULL) {
                addresslist = address_list_to_stringlist(head->env_reply_to->rt_list);
                if (addresslist.count()) {
                    m->setReplyto(addresslist.first());
                }
            }
            m->setMsgid(QString(head->env_message_id));
        } else if (item->att_data.att_static->att_type==MAILIMAP_MSG_ATT_INTERNALDATE) {
            mailimap_date_time*d = item->att_data.att_static->att_data.att_internal_date;
#if 0            
            QDateTime da(QDate(d->dt_year,d->dt_month,d->dt_day),QTime(d->dt_hour,d->dt_min,d->dt_sec));
            qDebug("%i %i %i - %i %i %i",d->dt_year,d->dt_month,d->dt_day,d->dt_hour,d->dt_min,d->dt_sec);
            qDebug(da.toString());
#endif            
        } else if (item->att_data.att_static->att_type==MAILIMAP_MSG_ATT_RFC822_SIZE) {
            size = item->att_data.att_static->att_data.att_rfc822_size;
        }
    }
    /* msg is already deleted */
    if (mFlags.testBit(FLAG_DELETED) && m) {
        delete m;
        m = 0;
    }
    if (m) {
        m->setFlags(mFlags);
        m->setMsgsize(size);
    }
    return m;
}

RecBody IMAPwrapper::fetchBody(const RecMail&mail)
{
    RecBody body;
    const char *mb;
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current;
    mailimap_fetch_att *fetchAtt;
    mailimap_fetch_type *fetchType;
    mailimap_set *set;
    mailimap_body*body_desc;

    mb = mail.getMbox().latin1();

    login();
    if (!m_imap) {
        return body;
    }
    
    err = mailimap_select( m_imap, (char*)mb);
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox: %s",m_imap->imap_response);
        return body;
    }
 
    result = clist_new();
    /* the range has to start at 1!!! not with 0!!!! */
    set = mailimap_set_new_interval( mail.getNumber(),mail.getNumber() );
    fetchAtt = mailimap_fetch_att_new_bodystructure();
    fetchType = mailimap_fetch_type_new_fetch_att(fetchAtt);
    err = mailimap_fetch( m_imap, set, fetchType, &result );
    mailimap_set_free( set );
    mailimap_fetch_type_free( fetchType );

    if (err == MAILIMAP_NO_ERROR && (current=clist_begin(result)) ) {
        mailimap_msg_att * msg_att;
        msg_att = (mailimap_msg_att*)current->data;
        mailimap_msg_att_item*item = (mailimap_msg_att_item*)msg_att->att_list->first->data;
        body_desc = item->att_data.att_static->att_data.att_body;
        if (body_desc->bd_type==MAILIMAP_BODY_1PART) {
            searchBodyText(mail,body_desc->bd_data.bd_body_1part,body);
        } else if (body_desc->bd_type==MAILIMAP_BODY_MPART) {
            qDebug("Mulitpart mail");
            searchBodyText(mail,body_desc->bd_data.bd_body_mpart,body);
        }
    } else {
        qDebug("error fetching body: %s",m_imap->imap_response);
    }
    mailimap_fetch_list_free(result);
    return body;
}

/* this routine is just called when the mail has only ONE part.
   for filling the parts of a multi-part-message there are other
   routines 'cause we can not simply fetch the whole body. */
void IMAPwrapper::searchBodyText(const RecMail&mail,mailimap_body_type_1part*mailDescription,RecBody&target_body)
{
    if (!mailDescription) {
        return;
    }
    QString sub,body_text;
    RecPart singlePart;
    QValueList<int> path;
    fillSinglePart(singlePart,mailDescription);
    switch (mailDescription->bd_type) {
    case MAILIMAP_BODY_TYPE_1PART_MSG:
        path.append(1);
        body_text = fetchPart(mail,path,true);
        target_body.setBodytext(body_text);
        target_body.setDescription(singlePart);
        break;
    case MAILIMAP_BODY_TYPE_1PART_TEXT:
        qDebug("Mediatype single: %s",mailDescription->bd_data.bd_type_text->bd_media_text);
        path.append(1);
        body_text = fetchPart(mail,path,true);
        target_body.setBodytext(body_text);
        target_body.setDescription(singlePart);
        break;
    case MAILIMAP_BODY_TYPE_1PART_BASIC:
        qDebug("Single attachment");
        target_body.setBodytext("");
        target_body.addPart(singlePart);
        break;
    default:
        break;
    }
    
    return;
}

QStringList IMAPwrapper::address_list_to_stringlist(clist*list)
{
    QStringList l;
    QString from;
    bool named_from;
    clistcell *current = NULL;
    mailimap_address * current_address=NULL;
    if (!list) {
        return l;
    }
    unsigned int count = 0;
    for (current=clist_begin(list);current!= NULL;current=clist_next(current)) {
        from = "";
        named_from = false;
        current_address=(mailimap_address*)current->data;
        if (current_address->ad_personal_name){
            from+=QString(current_address->ad_personal_name);
            from+=" ";
            named_from = true;
        }
        if (named_from && (current_address->ad_mailbox_name || current_address->ad_host_name)) {
            from+="<";
        }
        if (current_address->ad_mailbox_name) {
            from+=QString(current_address->ad_mailbox_name);
            from+="@";
        }
        if (current_address->ad_host_name) {
             from+=QString(current_address->ad_host_name);
        }
        if (named_from && (current_address->ad_mailbox_name || current_address->ad_host_name)) {
           from+=">";
        }
        l.append(QString(from));
        if (++count > 99) {
            break;
        }
    }
    return l;
}

QString IMAPwrapper::fetchPart(const RecMail&mail,const QValueList<int>&path,bool internal_call)
{
    QString body("");
    const char*mb;
    int err;
    mailimap_fetch_type *fetchType;
    mailimap_set *set;
    clistcell*current,*cur;

    login();
    if (!m_imap) {
        return body;
    }
    if (!internal_call) {
        mb = mail.getMbox().latin1();
        err = mailimap_select( m_imap, (char*)mb);
        if ( err != MAILIMAP_NO_ERROR ) {
            qDebug("error selecting mailbox: %s",m_imap->imap_response);
            return body;
        }
    }
    set = mailimap_set_new_single(mail.getNumber());
    clist*id_list=clist_new();
    for (unsigned j=0; j < path.count();++j) {
        uint32_t * p_id = (uint32_t *)malloc(sizeof(*p_id));
        *p_id = path[j];
        clist_append(id_list,p_id);
    }
    mailimap_section_part * section_part =  mailimap_section_part_new(id_list);
    mailimap_section_spec * section_spec =  mailimap_section_spec_new(MAILIMAP_SECTION_SPEC_SECTION_PART, NULL, section_part, NULL);
    mailimap_section * section = mailimap_section_new(section_spec);
    mailimap_fetch_att * fetch_att = mailimap_fetch_att_new_body_section(section);
    
    fetchType = mailimap_fetch_type_new_fetch_att(fetch_att);
    
    clist*result = clist_new();
    
    err = mailimap_fetch( m_imap, set, fetchType, &result );
    mailimap_set_free( set );
    mailimap_fetch_type_free( fetchType );
    
    if (err == MAILIMAP_NO_ERROR && (current=clist_begin(result)) ) {
        mailimap_msg_att * msg_att;
        msg_att = (mailimap_msg_att*)current->data;
        mailimap_msg_att_item*msg_att_item;
        for(cur = clist_begin(msg_att->att_list) ; cur != NULL ; cur = clist_next(cur)) {
            msg_att_item = (mailimap_msg_att_item*)clist_content(cur);
            if (msg_att_item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (msg_att_item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_BODY_SECTION) {
                    char*text = msg_att_item->att_data.att_static->att_data.att_body_section->sec_body_part;
                    msg_att_item->att_data.att_static->att_data.att_body_section->sec_body_part = 0L;
                    if (text) {
                        body = QString(text);
                        free(text);
                    } else {
                        body = "";
                    }
                }
            }
        }
         
    } else {
        qDebug("error fetching text: %s",m_imap->imap_response);
    }
    mailimap_fetch_list_free(result);
    return body;
}

void IMAPwrapper::searchBodyText(const RecMail&mail,mailimap_body_type_mpart*mailDescription,RecBody&target_body,int current_recursion,QValueList<int>recList)
{
    /* current_recursion is for avoiding ugly mails which has a to deep body-structure */
    if (!mailDescription||current_recursion==10) {
        return;
    }
    clistcell*current;
    mailimap_body*current_body;
    unsigned int count = 0;
    for (current=clist_begin(mailDescription->bd_list);current!=0;current=clist_next(current)) {
        /* the point in the message */
        ++count;
        current_body = (mailimap_body*)current->data;
        if (current_body->bd_type==MAILIMAP_BODY_MPART) {
            QValueList<int>clist = recList;
            clist.append(count);
            searchBodyText(mail,current_body->bd_data.bd_body_mpart,target_body,current_recursion+1,clist);
        } else if (current_body->bd_type==MAILIMAP_BODY_1PART){
            RecPart currentPart;
            fillSinglePart(currentPart,current_body->bd_data.bd_body_1part);
            QValueList<int>clist = recList;    
            clist.append(count);
            /* important: Check for is NULL 'cause a body can be empty! */
            if (currentPart.Type()=="text" && target_body.Bodytext().isNull() ) {
                QString body_text = fetchPart(mail,clist,true);
                target_body.setDescription(currentPart);
                target_body.setBodytext(body_text);
            } else {
                QString id("");
                for (unsigned int j = 0; j < clist.count();++j) {
                    id+=(j>0?" ":"");
                    id+=QString("%1").arg(clist[j]);
                }
                qDebug("ID= %s",id.latin1());
                currentPart.setIdentifier(id);
                currentPart.setPositionlist(clist);
                target_body.addPart(currentPart);
            }
        }
    }
}

void IMAPwrapper::fillSinglePart(RecPart&target_part,mailimap_body_type_1part*Description)
{
    if (!Description) {
        return;
    }
    switch (Description->bd_type) {
        case MAILIMAP_BODY_TYPE_1PART_TEXT:
            target_part.setType("text");
            fillSingleTextPart(target_part,Description->bd_data.bd_type_text);
            break;
        case MAILIMAP_BODY_TYPE_1PART_BASIC:
            fillSingleBasicPart(target_part,Description->bd_data.bd_type_basic);
            break;
        case MAILIMAP_BODY_TYPE_1PART_MSG:
            fillSingleMsgPart(target_part,Description->bd_data.bd_type_msg);
            break;
        default:
            break;
    }
}

void IMAPwrapper::fillSingleTextPart(RecPart&target_part,mailimap_body_type_text*which)
{
    if (!which) {
        return;
    }
    QString sub;
    sub = which->bd_media_text;
    target_part.setSubtype(sub.lower());
    target_part.setLines(which->bd_lines);
    fillBodyFields(target_part,which->bd_fields);
}

void IMAPwrapper::fillSingleMsgPart(RecPart&target_part,mailimap_body_type_msg*which)
{
    if (!which) {
        return;
    }
//    QString sub;
//    sub = which->bd_media_text;
//    target_part.setSubtype(sub.lower());
    qDebug("Message part");
    /* we set this type to text/plain */
    target_part.setType("text");
    target_part.setSubtype("plain");
    target_part.setLines(which->bd_lines);
    fillBodyFields(target_part,which->bd_fields);    
}

void IMAPwrapper::fillSingleBasicPart(RecPart&target_part,mailimap_body_type_basic*which)
{
    if (!which) {
        return;
    }
    QString type,sub;
    switch (which->bd_media_basic->med_type) {
    case MAILIMAP_MEDIA_BASIC_APPLICATION:
        type = "application";
        break;
    case MAILIMAP_MEDIA_BASIC_AUDIO:
        type = "audio";
        break;
    case MAILIMAP_MEDIA_BASIC_IMAGE:
        type = "image";
        break;
    case MAILIMAP_MEDIA_BASIC_MESSAGE:
        type = "message";
        break;
    case MAILIMAP_MEDIA_BASIC_VIDEO:
        type = "video";
        break;
    case MAILIMAP_MEDIA_BASIC_OTHER:
    default:
        if (which->bd_media_basic->med_basic_type) {
            type = which->bd_media_basic->med_basic_type;
        } else {
            type = "";
        }
        break;
    }
    if (which->bd_media_basic->med_subtype) {
        sub = which->bd_media_basic->med_subtype;
    } else {
        sub = "";
    }
    qDebug("Type = %s/%s",type.latin1(),sub.latin1());
    target_part.setType(type.lower());
    target_part.setSubtype(sub.lower());
    fillBodyFields(target_part,which->bd_fields);
}

void IMAPwrapper::fillBodyFields(RecPart&target_part,mailimap_body_fields*which)
{
    if (!which) return;
    if (which->bd_parameter && which->bd_parameter->pa_list && which->bd_parameter->pa_list->count>0) {
        clistcell*cur;
        mailimap_single_body_fld_param*param=0;
        for (cur = clist_begin(which->bd_parameter->pa_list);cur!=NULL;cur=clist_next(cur)) {
            param = (mailimap_single_body_fld_param*)cur->data;
            if (param) {
                target_part.addParameter(QString(param->pa_name).lower(),QString(param->pa_value));
            }
        }
    }
    mailimap_body_fld_enc*enc = which->bd_encoding;
    QString encoding("");
    switch (enc->enc_type) {
    case MAILIMAP_BODY_FLD_ENC_7BIT:
        encoding = "7bit";
        break;
    case MAILIMAP_BODY_FLD_ENC_8BIT:
        encoding = "8bit";
        break;
    case MAILIMAP_BODY_FLD_ENC_BINARY:
        encoding="binary";
        break;
    case MAILIMAP_BODY_FLD_ENC_BASE64:
        encoding="base64";
        break;
    case MAILIMAP_BODY_FLD_ENC_QUOTED_PRINTABLE:
        encoding="quoted-printable";
        break;
    case MAILIMAP_BODY_FLD_ENC_OTHER:
    default:
        if (enc->enc_value) {
            char*t=enc->enc_value;
            encoding=QString(enc->enc_value);
            enc->enc_value=0L;
            free(t);
        }
    }
    target_part.setEncoding(encoding);
    target_part.setSize(which->bd_size);
}

QString IMAPwrapper::fetchPart(const RecMail&mail,const RecPart&part)
{
    return fetchPart(mail,part.Positionlist(),false);
}

void IMAPwrapper::deleteMail(const RecMail&mail)
{
    mailimap_flag_list*flist;
    mailimap_set *set;
    mailimap_store_att_flags * store_flags;
    int err;
    login();
    if (!m_imap) {
        return;
    }
    const char *mb = mail.getMbox().latin1();
    err = mailimap_select( m_imap, (char*)mb);
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox for delete: %s",m_imap->imap_response);
        return;
    }
    flist = mailimap_flag_list_new_empty();
    mailimap_flag_list_add(flist,mailimap_flag_new_deleted());
    store_flags = mailimap_store_att_flags_new_set_flags(flist);
    set = mailimap_set_new_single(mail.getNumber());
    err = mailimap_store(m_imap,set,store_flags);
    mailimap_set_free( set );
    mailimap_store_att_flags_free(store_flags);

    if (err != MAILIMAP_NO_ERROR) {
        qDebug("error deleting mail: %s",m_imap->imap_response);
        return;
    }
    qDebug("deleting mail: %s",m_imap->imap_response);
    /* should we realy do that at this moment? */
    err = mailimap_expunge(m_imap);
    if (err != MAILIMAP_NO_ERROR) {
        qDebug("error deleting mail: %s",m_imap->imap_response);
    }
    qDebug("Delete successfull %s",m_imap->imap_response);
}

void IMAPwrapper::answeredMail(const RecMail&mail)
{
    mailimap_flag_list*flist;
    mailimap_set *set;
    mailimap_store_att_flags * store_flags;
    int err;
    login();
    if (!m_imap) {
        return;
    }
    const char *mb = mail.getMbox().latin1();
    err = mailimap_select( m_imap, (char*)mb);
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox for mark: %s",m_imap->imap_response);
        return;
    }
    flist = mailimap_flag_list_new_empty();
    mailimap_flag_list_add(flist,mailimap_flag_new_answered());
    store_flags = mailimap_store_att_flags_new_set_flags(flist);
    set = mailimap_set_new_single(mail.getNumber());
    err = mailimap_store(m_imap,set,store_flags);
    mailimap_set_free( set );
    mailimap_store_att_flags_free(store_flags);

    if (err != MAILIMAP_NO_ERROR) {
        qDebug("error marking mail: %s",m_imap->imap_response);
        return;
    }
}
