
#include <stdlib.h>

#include "imapwrapper.h"
#include "mailtypes.h"
#include <libetpan/mailimap.h>

IMAPwrapper::IMAPwrapper( IMAPaccount *a )
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
    logout();
    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILIMAP_NO_ERROR;

    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();

    m_imap = mailimap_new( 20, &imap_progress );
    /* connect */
  //  err = mailimap_socket_connect( m_imap, (char*)server, port );
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
    mailimap_fetch_att *fetchAtt,*fetchAttFlags,*fetchAttDate;
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
        logout();
        return;
    }

    int last = m_imap->imap_selection_info->sel_exists;

    if (last == 0) {
        qDebug("mailbox has no mails");
        logout();
        return;
    }

    result = clist_new();
    /* the range has to start at 1!!! not with 0!!!! */
    set = mailimap_set_new_interval( 1, last );
    fetchAtt = mailimap_fetch_att_new_envelope();
    fetchAttFlags = mailimap_fetch_att_new_flags();
    fetchAttDate = mailimap_fetch_att_new_internaldate();

    //fetchType = mailimap_fetch_type_new_fetch_att(fetchAtt);
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType,fetchAtt);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType,fetchAttFlags);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType,fetchAttDate);

    err = mailimap_fetch( m_imap, set, fetchType, &result );
    mailimap_set_free( set );
    /* cleans up the fetch_att's too! */
    mailimap_fetch_type_free( fetchType );

    QString date,subject,from;

    if ( err == MAILIMAP_NO_ERROR ) {
        current = clist_begin(result);
        mailimap_msg_att * msg_att;
        int i = 0;
        while ( current != 0 ) {
            ++i;
            msg_att = (mailimap_msg_att*)current->data;
            RecMail*m = parse_list_result(msg_att);
            if (m) {
                m->setNumber(i);
                m->setMbox(mailbox);
                target.append(m);
            }
            current = current->next;
        }
    } else {
        qDebug("Error fetching headers: %s",m_imap->imap_response);
    }
    logout();
    clist_free(result);
}

QList<IMAPFolder>* IMAPwrapper::listFolders()
{
    const char *path, *mask;
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current;

    QList<IMAPFolder> * folders = new QList<IMAPFolder>();
    folders->setAutoDelete( true );
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
    err = mailimap_list( m_imap, (char*)path, (char*)mask, &result );
    if ( err == MAILIMAP_NO_ERROR ) {
        current = result->first;
        for ( int i = result->count; i > 0; i-- ) {
            list = (mailimap_mailbox_list *) current->data;
            // it is better use the deep copy mechanism of qt itself
            // instead of using strdup!
            temp = list->mb_name;
            current = current->next;
            if (temp.lower()=="inbox")
                continue;
            folders->append(new IMAPFolder(temp));

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
    QBitArray mFlags(7);
    QStringList addresslist;

    if (!m_att) {
        return m;
    }

    c = clist_begin(m_att->att_list);
    while ( c ) {
        current = c;
        c = c->next;
        item = (mailimap_msg_att_item*)current->data;
        if (item->att_type!=MAILIMAP_MSG_ATT_ITEM_STATIC) {
            flist = (mailimap_msg_att_dynamic*)item->att_data.att_dyn;
            if (!flist->att_list) {
                continue;
            }
            cf = flist->att_list->first;
            while (cf) {
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
                cf = cf->next;
            }
            continue;
        }
        if ( item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_RFC822_HEADER ) {
            qDebug( "header: \n%s", item->att_data.att_static->att_data.att_rfc822_header );
        } else if (item->att_data.att_static->att_type==MAILIMAP_MSG_ATT_ENVELOPE) {
            mailimap_envelope * head = item->att_data.att_static->att_data.att_env;
            m = new RecMail();
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
            QDateTime da(QDate(d->dt_year,d->dt_month,d->dt_day),QTime(d->dt_hour,d->dt_min,d->dt_sec));
            qDebug("%i %i %i - %i %i %i",d->dt_year,d->dt_month,d->dt_day,d->dt_hour,d->dt_min,d->dt_sec);
            qDebug(da.toString());
        } else {
            qDebug("Another type");
        }
    }
    /* msg is already deleted */
    if (mFlags.testBit(FLAG_DELETED) && m) {
        delete m;
        m = 0;
    }
    if (m) {
        m->setFlags(mFlags);
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
    /* select mailbox READONLY for operations */
    err = mailimap_examine( m_imap, (char*)mb);
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox: %s",m_imap->imap_response);
        logout();
        return body;
    }
    result = clist_new();
    /* the range has to start at 1!!! not with 0!!!! */
    set = mailimap_set_new_interval( mail.getNumber(),mail.getNumber() );
    fetchAtt = mailimap_fetch_att_new_body();
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
        } else {
        }

    } else {
        qDebug("error fetching body: %s",m_imap->imap_response);
    }

    clist_free(result);
    logout();
    return body;
}

void IMAPwrapper::searchBodyText(const RecMail&mail,mailimap_body_type_1part*mailDescription,RecBody&target_body)
{
    if (!mailDescription) {
        return;
    }
    switch (mailDescription->bd_type) {
    case MAILIMAP_BODY_TYPE_1PART_TEXT:
            fillPlainBody(mail,target_body,mailDescription->bd_data.bd_type_text);
        break;
    default:
        break;
    }
    return;
}

void IMAPwrapper::fillPlainBody(const RecMail&mail,RecBody&target_body, mailimap_body_type_text * bd)
{
    const char *mb;
    QString body="";
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current,*cur;
    mailimap_fetch_att *fetchAtt;
    mailimap_fetch_type *fetchType;
    mailimap_set *set;

    mb = mail.getMbox().latin1();

    if (!m_imap) {
        return;
    }

    result = clist_new();
    set =  set = mailimap_set_new_single(mail.getNumber());
    mailimap_section * section = mailimap_section_new_text();
    fetchAtt = mailimap_fetch_att_new_body_peek_section(section);
    fetchType = mailimap_fetch_type_new_fetch_att(fetchAtt);
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
                    int length = msg_att_item->att_data.att_static->att_data.att_body_section->sec_length;
                    msg_att_item->att_data.att_static->att_data.att_body_section->sec_body_part = 0L;
                    body = QString(text);
                    free(text);
                }
            }
        }
         
    } else {
        qDebug("error fetching text: %s",m_imap->imap_response);
    }
    //clist_free(result);
    mailimap_fetch_list_free(result);
    target_body.setBodytext(body);
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
    current = clist_begin(list);
    unsigned int count = 0;
    while (current!= NULL) {
        from = "";
        named_from = false;
        current_address=(mailimap_address*)current->data;
        current = current->next;
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
