#include <stdlib.h>
#include <libetpan/libetpan.h>
#include <qpe/global.h>

#include "imapwrapper.h"
#include "mailtypes.h"
#include "logindialog.h"

IMAPwrapper::IMAPwrapper( IMAPaccount *a )
    : AbstractMail()
{
    account = a;
    m_imap = 0;
    m_Lastmbox = "";
}

IMAPwrapper::~IMAPwrapper()
{
    logout();
}

/* to avoid to often select statements in loops etc.
   we trust that we are logged in and connection is established!*/
int IMAPwrapper::selectMbox(const QString&mbox)
{
    if (mbox == m_Lastmbox) {
        return MAILIMAP_NO_ERROR;
    }
    int err = mailimap_select( m_imap, (char*)mbox.latin1());
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox: %s",m_imap->imap_response);
        m_Lastmbox = "";
        return err;
    }
    m_Lastmbox = mbox;
    return err;
}

void IMAPwrapper::imap_progress( size_t current, size_t maximum )
{
    qDebug( "IMAP: %i of %i", current, maximum );
}

bool IMAPwrapper::start_tls(bool force_tls)
{
    int err;
    bool try_tls;
    mailimap_capability_data * cap_data = 0;

    err = mailimap_capability(m_imap,&cap_data);
    if (err != MAILIMAP_NO_ERROR) {
        Global::statusMessage("error getting capabilities!");
        qDebug("error getting capabilities!");
        return false;
    }
    clistiter * cur;
    for(cur = clist_begin(cap_data->cap_list) ; cur != NULL;cur = clist_next(cur)) {
        struct mailimap_capability * cap;
        cap  = (struct mailimap_capability *)clist_content(cur);
        if (cap->cap_type == MAILIMAP_CAPABILITY_NAME) {
            if (strcasecmp(cap->cap_data.cap_name, "STARTTLS") == 0) {
                try_tls = true;
                break;
            }
        }
    }
    if (cap_data) {
        mailimap_capability_data_free(cap_data);
    }
    if (try_tls) {
        err =  mailimap_starttls(m_imap);
        if (err != MAILIMAP_NO_ERROR && force_tls) {
            Global::statusMessage(tr("Server has no TLS support!"));
            qDebug("Server has no TLS support!");
            try_tls = false;
        } else {
            mailstream_low * low;
            mailstream_low * new_low;
            low = mailstream_get_low(m_imap->imap_stream);
            if (!low) {
                try_tls = false;
            } else {
                int fd = mailstream_low_get_fd(low);
                if (fd > -1 && (new_low = mailstream_low_ssl_open(fd))!=0) {
                    mailstream_low_free(low);
                    mailstream_set_low(m_imap->imap_stream, new_low);
                } else {
                    try_tls = false;
                }
            }
        }
   }
   return try_tls;
}

void IMAPwrapper::login()
{
    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILIMAP_NO_ERROR;

    if (account->getOffline()) return;
    /* we are connected this moment */
    /* TODO: setup a timer holding the line or if connection closed - delete the value */
    if (m_imap) {
        err = mailimap_noop(m_imap);
        if (err!=MAILIMAP_NO_ERROR) {
            logout();
        } else {
            mailstream_flush(m_imap->imap_stream);
            return;
        }
    }
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
		qDebug( "IMAP: Login canceled" );
		return;
	  }
	} else {
	  user = account->getUser().latin1();
	  pass = account->getPassword().latin1();
	}

    m_imap = mailimap_new( 20, &imap_progress );

    /* connect */
    bool ssl = false;
    bool try_tls = false;
    bool force_tls = false;

    if  ( account->ConnectionType() == 2 ) {
        ssl = true;
    }
    if (account->ConnectionType()==1) {
        force_tls = true;
    }

    if ( ssl ) {
        qDebug(  "using ssl" );
        err = mailimap_ssl_connect( m_imap, (char*)server, port );
    } else {
        err = mailimap_socket_connect( m_imap, (char*)server, port );
    }

    if ( err != MAILIMAP_NO_ERROR &&
         err != MAILIMAP_NO_ERROR_AUTHENTICATED &&
         err != MAILIMAP_NO_ERROR_NON_AUTHENTICATED ) {
         QString failure = "";
         if (err == MAILIMAP_ERROR_CONNECTION_REFUSED) {
            failure="Connection refused";
         } else {
            failure="Unknown failure";
         }
         Global::statusMessage(tr("error connecting imap server: %1").arg(failure));
         mailimap_free( m_imap );
         m_imap = 0;
         return;
    }

    if (!ssl) {
        try_tls = start_tls(force_tls);
    }

    bool ok = true;
    if (force_tls && !try_tls) {
        Global::statusMessage(tr("Server has no TLS support!"));
        qDebug("Server has no TLS support!");
        ok = false;
    }


    /* login */

    if (ok) {
        err = mailimap_login_simple( m_imap, (char*)user, (char*)pass );
        if ( err != MAILIMAP_NO_ERROR ) {
            Global::statusMessage(tr("error logging in imap server: %1").arg(m_imap->imap_response));
            ok = false;
        }
    }
    if (!ok) {
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
    m_Lastmbox = "";
}

void IMAPwrapper::listMessages(const QString&mailbox,QList<RecMail> &target )
{
    int err = MAILIMAP_NO_ERROR;
    clist *result = 0;
    clistcell *current;
    mailimap_fetch_type *fetchType = 0;
    mailimap_set *set = 0;

    login();
    if (!m_imap) {
        return;
    }
    /* select mailbox READONLY for operations */
    err = selectMbox(mailbox);
    if ( err != MAILIMAP_NO_ERROR ) {
        return;
    }

    int last = m_imap->imap_selection_info->sel_exists;

    if (last == 0) {
        Global::statusMessage(tr("Mailbox has no mails"));
        return;
    } else {
    }

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
        Global::statusMessage(tr("Mailbox has %1 mails").arg(target.count()));
    } else {
        Global::statusMessage(tr("Error fetching headers: %1").arg(m_imap->imap_response));
    }
    if (result) mailimap_fetch_list_free(result);
}

QList<Folder>* IMAPwrapper::listFolders()
{
    const char *path, *mask;
    int err = MAILIMAP_NO_ERROR;
    clist *result = 0;
    clistcell *current = 0;
    clistcell*cur_flag = 0;
    mailimap_mbx_list_flags*bflags = 0;

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
    mailimap_mailbox_list *list;
    err = mailimap_list( m_imap, (char*)"", (char*)mask, &result );
    QString del;
    bool selectable = true;
    bool no_inferiors = false;
    if ( err == MAILIMAP_NO_ERROR ) {
        current = result->first;
        for ( int i = result->count; i > 0; i-- ) {
            list = (mailimap_mailbox_list *) current->data;
            // it is better use the deep copy mechanism of qt itself
            // instead of using strdup!
            temp = list->mb_name;
            del = list->mb_delimiter;
            current = current->next;
            if ( (bflags = list->mb_flag) ) {
                selectable = !(bflags->mbf_type==MAILIMAP_MBX_LIST_FLAGS_SFLAG&&
                            bflags->mbf_sflag==MAILIMAP_MBX_LIST_SFLAG_NOSELECT);
                for(cur_flag=clist_begin(bflags->mbf_oflags);cur_flag;cur_flag=clist_next(cur_flag)) {
                    if ( ((mailimap_mbx_list_oflag*)cur_flag->data)->of_type==MAILIMAP_MBX_LIST_OFLAG_NOINFERIORS) {
                        no_inferiors = true;
                    }
                }
            }
            folders->append( new IMAPFolder(temp,del,selectable,no_inferiors,account->getPrefix()));
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
    qDebug(path);
    err = mailimap_list( m_imap, (char*)path, (char*)mask, &result );
    if ( err == MAILIMAP_NO_ERROR ) {
        current = result->first;
        for ( current=clist_begin(result);current!=NULL;current=clist_next(current)) {
            no_inferiors = false;
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
                for(cur_flag=clist_begin(bflags->mbf_oflags);cur_flag;cur_flag=clist_next(cur_flag)) {
                    if ( ((mailimap_mbx_list_oflag*)cur_flag->data)->of_type==MAILIMAP_MBX_LIST_OFLAG_NOINFERIORS) {
                        no_inferiors = true;
                    }
                }
            }
            del = list->mb_delimiter;
            folders->append(new IMAPFolder(temp,del,selectable,no_inferiors,account->getPrefix()));
        }
    } else {
        qDebug("error fetching folders %s",m_imap->imap_response);
    }
    if (result) mailimap_list_result_free( result );
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
            m->setSubject(convert_String((const char*)head->env_subject));
            //m->setSubject(head->env_subject);
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
#if 0
            mailimap_date_time*d = item->att_data.att_static->att_data.att_internal_date;
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
    clist *result = 0;
    clistcell *current;
    mailimap_fetch_att *fetchAtt = 0;
    mailimap_fetch_type *fetchType = 0;
    mailimap_set *set = 0;
    mailimap_body*body_desc = 0;

    mb = mail.getMbox().latin1();

    login();
    if (!m_imap) {
        return body;
    }
    err = selectMbox(mail.getMbox());
    if ( err != MAILIMAP_NO_ERROR ) {
        return body;
    }

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
        QValueList<int> path;
        body_desc = item->att_data.att_static->att_data.att_body;
        traverseBody(mail,body_desc,body,0,path);
    } else {
        qDebug("error fetching body: %s",m_imap->imap_response);
    }
    if (result) mailimap_fetch_list_free(result);
    return body;
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
            from+=convert_String((const char*)current_address->ad_personal_name);
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

encodedString*IMAPwrapper::fetchRawPart(const RecMail&mail,const QValueList<int>&path,bool internal_call)
{
    encodedString*res=new encodedString;
    int err;
    mailimap_fetch_type *fetchType;
    mailimap_set *set;
    clistcell*current,*cur;
    mailimap_section_part * section_part =  0;
    mailimap_section_spec * section_spec =  0;
    mailimap_section * section = 0;
    mailimap_fetch_att * fetch_att = 0;

    login();
    if (!m_imap) {
        return res;
    }
    if (!internal_call) {
        err = selectMbox(mail.getMbox());
        if ( err != MAILIMAP_NO_ERROR ) {
            return res;
        }
    }
    set = mailimap_set_new_single(mail.getNumber());

    clist*id_list = 0;

    /* if path == empty then its a request for the whole rfc822 mail and generates
       a "fetch <id> (body[])" statement on imap server */
    if (path.count()>0 ) {
        id_list = clist_new();
        for (unsigned j=0; j < path.count();++j) {
            uint32_t * p_id = (uint32_t *)malloc(sizeof(*p_id));
            *p_id = path[j];
            clist_append(id_list,p_id);
        }
        section_part =  mailimap_section_part_new(id_list);
        section_spec =  mailimap_section_spec_new(MAILIMAP_SECTION_SPEC_SECTION_PART, NULL, section_part, NULL);
    }

    section =  mailimap_section_new(section_spec);
    fetch_att = mailimap_fetch_att_new_body_section(section);
    fetchType = mailimap_fetch_type_new_fetch_att(fetch_att);

    clist*result = 0;

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
                    /* detach - we take over the content */
                    msg_att_item->att_data.att_static->att_data.att_body_section->sec_body_part = 0L;
                    res->setContent(text,msg_att_item->att_data.att_static->att_data.att_body_section->sec_length);
                }
            }
        }
    } else {
        qDebug("error fetching text: %s",m_imap->imap_response);
    }
    if (result) mailimap_fetch_list_free(result);
    return res;
}

/* current_recursion is for recursive calls.
   current_count means the position inside the internal loop! */
void IMAPwrapper::traverseBody(const RecMail&mail,mailimap_body*body,RecBody&target_body,
    int current_recursion,QValueList<int>recList,int current_count)
{
    if (!body || current_recursion>=10) {
        return;
    }
    switch (body->bd_type) {
    case MAILIMAP_BODY_1PART:
    {
        QValueList<int>countlist = recList;
        countlist.append(current_count);
        RecPart currentPart;
        mailimap_body_type_1part*part1 = body->bd_data.bd_body_1part;
        QString id("");
        currentPart.setPositionlist(countlist);
        for (unsigned int j = 0; j < countlist.count();++j) {
            id+=(j>0?" ":"");
            id+=QString("%1").arg(countlist[j]);
        }
        qDebug("ID = %s",id.latin1());
        currentPart.setIdentifier(id);
        fillSinglePart(currentPart,part1);
        /* important: Check for is NULL 'cause a body can be empty!
           And we put it only into the mail if it is the FIRST part */
        if (part1->bd_type==MAILIMAP_BODY_TYPE_1PART_TEXT && target_body.Bodytext().isNull() && countlist[0]==1) {
            QString body_text = fetchTextPart(mail,countlist,true,currentPart.Encoding());
            target_body.setDescription(currentPart);
            target_body.setBodytext(body_text);
            if (countlist.count()>1) {
                target_body.addPart(currentPart);
            }
        } else {
            target_body.addPart(currentPart);
        }
        if (part1->bd_type==MAILIMAP_BODY_TYPE_1PART_MSG) {
            traverseBody(mail,part1->bd_data.bd_type_msg->bd_body,target_body,current_recursion+1,countlist);
        }
    }
    break;
    case MAILIMAP_BODY_MPART:
    {
        QValueList<int>countlist = recList;
        clistcell*current=0;
        mailimap_body*current_body=0;
        unsigned int ccount = 1;
        mailimap_body_type_mpart*mailDescription = body->bd_data.bd_body_mpart;
        for (current=clist_begin(mailDescription->bd_list);current!=0;current=clist_next(current)) {
            current_body = (mailimap_body*)current->data;
            if (current_body->bd_type==MAILIMAP_BODY_MPART) {
                RecPart targetPart;
                targetPart.setType("multipart");
                fillMultiPart(targetPart,mailDescription);
                countlist.append(current_count);
                targetPart.setPositionlist(countlist);
                target_body.addPart(targetPart);
                QString id("");
                for (unsigned int j = 0; j < countlist.count();++j) {
                    id+=(j>0?" ":"");
                    id+=QString("%1").arg(countlist[j]);
                }
                qDebug("ID(mpart) = %s",id.latin1());
            }
            traverseBody(mail,current_body,target_body,current_recursion+1,countlist,ccount);
            if (current_body->bd_type==MAILIMAP_BODY_MPART) {
                countlist = recList;
            }
            ++ccount;
        }
    }
    break;
    default:
    break;
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
            target_part.setType("message");
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
    qDebug("Type= text/%s",which->bd_media_text);
    target_part.setSubtype(sub.lower());
    target_part.setLines(which->bd_lines);
    fillBodyFields(target_part,which->bd_fields);
}

void IMAPwrapper::fillSingleMsgPart(RecPart&target_part,mailimap_body_type_msg*which)
{
    if (!which) {
        return;
    }
    target_part.setSubtype("rfc822");
    qDebug("Message part");
    /* we set this type to text/plain */
    target_part.setLines(which->bd_lines);
    fillBodyFields(target_part,which->bd_fields);
}

void IMAPwrapper::fillMultiPart(RecPart&target_part,mailimap_body_type_mpart*which)
{
    if (!which) return;
    QString sub = which->bd_media_subtype;
    target_part.setSubtype(sub.lower());
    if (which->bd_ext_mpart && which->bd_ext_mpart->bd_parameter && which->bd_ext_mpart->bd_parameter->pa_list) {
        clistcell*cur = 0;
        mailimap_single_body_fld_param*param=0;
        for (cur = clist_begin(which->bd_ext_mpart->bd_parameter->pa_list);cur!=NULL;cur=clist_next(cur)) {
            param = (mailimap_single_body_fld_param*)cur->data;
            if (param) {
                target_part.addParameter(QString(param->pa_name).lower(),QString(param->pa_value));
            }
        }
    }
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
    if (which->bd_description) {
        target_part.setDescription(QString(which->bd_description));
    }
    target_part.setEncoding(encoding);
    target_part.setSize(which->bd_size);
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
    err = selectMbox(mail.getMbox());
    if ( err != MAILIMAP_NO_ERROR ) {
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
    err = selectMbox(mail.getMbox());
    if ( err != MAILIMAP_NO_ERROR ) {
        return;
    }
    flist = mailimap_flag_list_new_empty();
    mailimap_flag_list_add(flist,mailimap_flag_new_answered());
    store_flags = mailimap_store_att_flags_new_add_flags(flist);
    set = mailimap_set_new_single(mail.getNumber());
    err = mailimap_store(m_imap,set,store_flags);
    mailimap_set_free( set );
    mailimap_store_att_flags_free(store_flags);

    if (err != MAILIMAP_NO_ERROR) {
        qDebug("error marking mail: %s",m_imap->imap_response);
        return;
    }
}

QString IMAPwrapper::fetchTextPart(const RecMail&mail,const QValueList<int>&path,bool internal_call,const QString&enc)
{
    QString body("");
    encodedString*res = fetchRawPart(mail,path,internal_call);
    encodedString*r = decode_String(res,enc);
    delete res;
    if (r) {
        if (r->Length()>0) {
            body = r->Content();
        }
        delete r;
    }
    return body;
}

QString IMAPwrapper::fetchTextPart(const RecMail&mail,const RecPart&part)
{
    return fetchTextPart(mail,part.Positionlist(),false,part.Encoding());
}

encodedString* IMAPwrapper::fetchDecodedPart(const RecMail&mail,const RecPart&part)
{
    encodedString*res = fetchRawPart(mail,part.Positionlist(),false);
    encodedString*r = decode_String(res,part.Encoding());
    delete res;
    return r;
}

encodedString* IMAPwrapper::fetchRawPart(const RecMail&mail,const RecPart&part)
{
    return fetchRawPart(mail,part.Positionlist(),false);
}

int IMAPwrapper::deleteAllMail(const Folder*folder)
{
    login();
    if (!m_imap) {
        return 0;
    }
    mailimap_flag_list*flist;
    mailimap_set *set;
    mailimap_store_att_flags * store_flags;
    int err = selectMbox(folder->getName());
    if ( err != MAILIMAP_NO_ERROR ) {
        return 0;
    }

    int last = m_imap->imap_selection_info->sel_exists;
    if (last == 0) {
        Global::statusMessage(tr("Mailbox has no mails!"));
        return 0;
    }
    flist = mailimap_flag_list_new_empty();
    mailimap_flag_list_add(flist,mailimap_flag_new_deleted());
    store_flags = mailimap_store_att_flags_new_set_flags(flist);
    set = mailimap_set_new_interval( 1, last );
    err = mailimap_store(m_imap,set,store_flags);
    mailimap_set_free( set );
    mailimap_store_att_flags_free(store_flags);
    if (err != MAILIMAP_NO_ERROR) {
        Global::statusMessage(tr("error deleting mail: %s").arg(m_imap->imap_response));
        return 0;
    }
    qDebug("deleting mail: %s",m_imap->imap_response);
    /* should we realy do that at this moment? */
    err = mailimap_expunge(m_imap);
    if (err != MAILIMAP_NO_ERROR) {
        Global::statusMessage(tr("error deleting mail: %s").arg(m_imap->imap_response));
        return 0;
    }
    qDebug("Delete successfull %s",m_imap->imap_response);
    return 1;
}

int IMAPwrapper::createMbox(const QString&folder,const Folder*parentfolder,const QString& delemiter,bool getsubfolder)
{
    if (folder.length()==0) return 0;
    login();
    if (!m_imap) {return 0;}
    QString pre = account->getPrefix();
    if (delemiter.length()>0 && pre.findRev(delemiter)!=pre.length()-1) {
        pre+=delemiter;
    }
    if (parentfolder) {
        pre += parentfolder->getDisplayName()+delemiter;
    }
    pre+=folder;
    if (getsubfolder) {
        if (delemiter.length()>0) {
            pre+=delemiter;
        } else {
            Global::statusMessage(tr("Cannot create folder %1 for holding subfolders").arg(pre));
            return 0;
        }
    }
    qDebug("Creating %s",pre.latin1());
    int res = mailimap_create(m_imap,pre.latin1());
    if (res != MAILIMAP_NO_ERROR) {
        Global::statusMessage(tr("%1").arg(m_imap->imap_response));
        return 0;
    }
    return 1;
}

int IMAPwrapper::deleteMbox(const Folder*folder)
{
    if (!folder) return 0;
    login();
    if (!m_imap) {return 0;}
    int res = mailimap_delete(m_imap,folder->getName());
    if (res != MAILIMAP_NO_ERROR) {
        Global::statusMessage(tr("%1").arg(m_imap->imap_response));
        return 0;
    }
    return 1;
}

void IMAPwrapper::statusFolder(folderStat&target_stat,const QString & mailbox)
{
    mailimap_status_att_list * att_list =0;
    mailimap_mailbox_data_status * status=0;
    clistiter * cur = 0;
    int r = 0;
    int res = 0;
    target_stat.message_count = 0;
    target_stat.message_unseen = 0;
    target_stat.message_recent = 0;
    login();
    if (!m_imap) {
        return;
    }
    att_list = mailimap_status_att_list_new_empty();
    if (!att_list) return;
    r = mailimap_status_att_list_add(att_list, MAILIMAP_STATUS_ATT_MESSAGES);
    r = mailimap_status_att_list_add(att_list, MAILIMAP_STATUS_ATT_RECENT);
    r = mailimap_status_att_list_add(att_list, MAILIMAP_STATUS_ATT_UNSEEN);
    r = mailimap_status(m_imap, mailbox.latin1(), att_list, &status);
    if (r==MAILIMAP_NO_ERROR&&status->st_info_list!=0) {
        for (cur = clist_begin(status->st_info_list);
                cur != NULL ; cur = clist_next(cur)) {
            mailimap_status_info * status_info;
            status_info = (mailimap_status_info *)clist_content(cur);
            switch (status_info->st_att) {
            case MAILIMAP_STATUS_ATT_MESSAGES:
                target_stat.message_count = status_info->st_value;
            break;
            case MAILIMAP_STATUS_ATT_RECENT:
                target_stat.message_recent = status_info->st_value;
            break;
            case MAILIMAP_STATUS_ATT_UNSEEN:
                target_stat.message_unseen = status_info->st_value;
            break;
            }
        }
    } else {
        qDebug("Error retrieving status");
    }
    if (status) mailimap_mailbox_data_status_free(status);
    if (att_list) mailimap_status_att_list_free(att_list);
}

void IMAPwrapper::storeMessage(const char*msg,size_t length, const QString&folder)
{
    login();
    if (!m_imap) return;
    if (!msg) return;
    int r = mailimap_append(m_imap,(char*)folder.latin1(),0,0,msg,length);
    if (r != MAILIMAP_NO_ERROR) {
        Global::statusMessage("Error storing mail!");
    }
}

MAILLIB::ATYPE IMAPwrapper::getType()const
{
    return account->getType();
}

const QString&IMAPwrapper::getName()const
{
    qDebug("Get name: %s",account->getAccountName().latin1());
    return account->getAccountName();
}

encodedString* IMAPwrapper::fetchRawBody(const RecMail&mail)
{
    // dummy
    QValueList<int> path;
    return fetchRawPart(mail,path,false);
}

void IMAPwrapper::mvcpAllMails(Folder*fromFolder,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    if (targetWrapper != this) {
        AbstractMail::mvcpAllMails(fromFolder,targetFolder,targetWrapper,moveit);
        qDebug("Using generic");
        return;
    }
    mailimap_set *set = 0;
    login();
    if (!m_imap) {
        return;
    }
    int err = selectMbox(fromFolder->getName());
    if ( err != MAILIMAP_NO_ERROR ) {
        return;
    }
    int last = m_imap->imap_selection_info->sel_exists;
    set = mailimap_set_new_interval( 1, last );
    err = mailimap_copy(m_imap,set,targetFolder.latin1());
    mailimap_set_free( set );
    if ( err != MAILIMAP_NO_ERROR ) {
        QString error_msg = tr("error copy mails: %1").arg(m_imap->imap_response);
        Global::statusMessage(error_msg);
        qDebug(error_msg);
        return;
    }
    if (moveit) {
        deleteAllMail(fromFolder);
    }
}

void IMAPwrapper::mvcpMail(const RecMail&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    if (targetWrapper != this) {
        qDebug("Using generic");
        AbstractMail::mvcpMail(mail,targetFolder,targetWrapper,moveit);
        return;
    }
    mailimap_set *set = 0;
    login();
    if (!m_imap) {
        return;
    }
    int err = selectMbox(mail.getMbox());
    if ( err != MAILIMAP_NO_ERROR ) {
        return;
    }
    set = mailimap_set_new_single(mail.getNumber());
    err = mailimap_copy(m_imap,set,targetFolder.latin1());
    mailimap_set_free( set );
    if ( err != MAILIMAP_NO_ERROR ) {
        QString error_msg = tr("error copy mail: %1").arg(m_imap->imap_response);
        Global::statusMessage(error_msg);
        qDebug(error_msg);
        return;
    }
    if (moveit) {
        deleteMail(mail);
    }
}
