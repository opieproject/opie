
#include <stdlib.h>

#include "imapwrapper.h"


IMAPwrapper::IMAPwrapper( IMAPaccount *a )
{
    account = a;
}

void imap_progress( size_t current, size_t maximum )
{
    qDebug( "IMAP: %i of %i", current, maximum );
}

void IMAPwrapper::listMessages(const QString&mailbox,Maillist&target )
{
    const char *server, *user, *pass, *mb;
    uint16_t port;
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current;
    mailimap_fetch_att *fetchAtt,*fetchAttFlags,*fetchAttDate;
    mailimap_fetch_type *fetchType;
    mailimap_set *set;
    
    mb = mailbox.latin1();
    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();
    
    mailimap *imap = mailimap_new( 20, &imap_progress ); 
    if ( imap == NULL ) {
        qDebug("IMAP Memory error");
        return;
    }

    /* connect */
    err = mailimap_socket_connect( imap, (char*)server, port );
    if ( err != MAILIMAP_NO_ERROR &&  
         err != MAILIMAP_NO_ERROR_AUTHENTICATED &&
         err != MAILIMAP_NO_ERROR_NON_AUTHENTICATED ) {
         qDebug("error connecting server: %s",imap->response);
         mailimap_free( imap );
         return;
    }
    
    /* login */
    err = mailimap_login_simple( imap, (char*)user, (char*)pass );
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error logging in imap: %s",imap->response);
        err = mailimap_close( imap );
        mailimap_free( imap );
        return;
    }
    
    /* select mailbox READONLY for operations */
    err = mailimap_examine( imap, (char*)mb);
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox: %s",imap->response);
        err = mailimap_logout( imap );
        err = mailimap_close( imap );
        mailimap_free( imap );
        return;
    }

    int last = imap->selection_info->exists;
    if (last == 0) {
        qDebug("mailbox has no mails");
        err = mailimap_logout( imap );
        err = mailimap_close( imap );
        mailimap_free( imap );
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
    
    err = mailimap_fetch( imap, set, fetchType, &result );
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
                target.append(m);
            }
            current = current->next;
        }
    } else {
        qDebug("Error fetching headers: %s",imap->response);
    }   

    err = mailimap_logout( imap );
    err = mailimap_close( imap );
    clist_free(result);
    mailimap_free( imap );
}

QList<IMAPFolder>* IMAPwrapper::listFolders()
{
    const char *server, *user, *pass, *path, *mask;
    uint16_t port;
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current;
   
    QList<IMAPFolder> * folders = new QList<IMAPFolder>();
    folders->setAutoDelete( true );
        
    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();
    path = account->getPrefix().latin1();
    
    mailimap *imap = mailimap_new( 20, &imap_progress ); 
    if ( imap == NULL ) {
        qDebug("error mailimap_new");
        return folders;
    }
    
    err = mailimap_socket_connect( imap, (char*)server, port );
    if ( err != MAILIMAP_NO_ERROR &&  
         err != MAILIMAP_NO_ERROR_AUTHENTICATED &&
         err != MAILIMAP_NO_ERROR_NON_AUTHENTICATED ) {
         mailimap_free(imap);
         qDebug("error imap_socket_connect: %s",imap->response);
         return folders;
    }
        
    err = mailimap_login_simple( imap, (char*)user, (char*)pass );
    if ( err != MAILIMAP_NO_ERROR ) {
        mailimap_free(imap);
        qDebug("error logging in: %s",imap->response);
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
    err = mailimap_list( imap, (char*)"", (char*)mask, &result );
    if ( err == MAILIMAP_NO_ERROR ) {
        current = result->first;
        for ( int i = result->count; i > 0; i-- ) {
            list = (mailimap_mailbox_list *) current->data;
            // it is better use the deep copy mechanism of qt itself
            // instead of using strdup!
            temp = list->mb;
            folders->append( new IMAPFolder(temp));
            current = current->next;
        }
    } else {
        qDebug("error fetching folders: %s",imap->response);
    }
    mailimap_list_result_free( result );
    
/*
 * second stage - get the other then inbox folders
 */
    mask = "*" ;    
    result = clist_new();
    err = mailimap_list( imap, (char*)path, (char*)mask, &result );
    if ( err == MAILIMAP_NO_ERROR ) {
        current = result->first;
        for ( int i = result->count; i > 0; i-- ) {
            list = (mailimap_mailbox_list *) current->data;
            // it is better use the deep copy mechanism of qt itself
            // instead of using strdup!
            temp = list->mb;
            current = current->next;
            if (temp.lower()=="inbox")
                continue;
            folders->append(new IMAPFolder(temp));
            
        }
    } else {
        qDebug("error fetching folders");
    }
    mailimap_list_result_free( result );
    err = mailimap_logout( imap );
    err = mailimap_close( imap );
    mailimap_free( imap );     
    return folders;
}

RecMail*IMAPwrapper::parse_list_result(mailimap_msg_att* m_att)
{
    RecMail * m = 0;
    mailimap_msg_att_item *item=0;
    bool named_from = false;
    QString from,date,subject;
    date = from = subject = "";
    clistcell *current,*c,*cf, *current_from = NULL;
    mailimap_address * current_address = NULL;
    mailimap_msg_att_dynamic*flist;
    mailimap_flag_fetch*cflag;
    QBitArray mFlags(7);
    
    if (!m_att) {
        return m;
    }

#if 0
   MAILIMAP_FLAG_KEYWORD,   /* keyword flag */
   MAILIMAP_FLAG_EXTENSION, /* \extension flag */
#endif
    c = clist_begin(m_att->list);
    while ( c ) {
        current = c;
        c = c->next;
        item = (mailimap_msg_att_item*)current->data;
        if (item->type!=MAILIMAP_MSG_ATT_ITEM_STATIC) {
            flist = (mailimap_msg_att_dynamic*)item->msg_att_dyn;
            if (!flist->list) {
                continue;
            }
            cf = flist->list->first;
            while (cf) {
                cflag = (mailimap_flag_fetch*)cf->data;
                if (cflag->type==MAILIMAP_FLAG_FETCH_OTHER && cflag->flag!=0) {
                    switch (cflag->flag->type) {
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
                } else if (cflag->type==MAILIMAP_FLAG_FETCH_RECENT) {
                    mFlags.setBit(FLAG_RECENT);
                }
                
                cf = cf->next;
            }
            continue;
        }
        if ( item->msg_att_static->type == MAILIMAP_MSG_ATT_RFC822_HEADER ) {
            qDebug( "header: \n%s", item->msg_att_static->rfc822_header );
        } else if (item->msg_att_static->type==MAILIMAP_MSG_ATT_ENVELOPE) {
            mailimap_envelope * head = item->msg_att_static->env;
            date = head->date;
            subject = head->subject;
            if (head->from!=NULL)
                current_from = head->from->list->first;
            while (current_from != NULL) {
                from = "";
                named_from = false;
                current_address=(mailimap_address*)current_from->data;
                current_from = current_from->next;
                if (current_address->personal_name){
                    from+=QString(current_address->personal_name);
                    from+=" ";
                    named_from = true;
                }
                if (named_from && (current_address->mailbox_name || current_address->host_name)) {
                    from+="<";
                }
                if (current_address->mailbox_name) {
                    from+=QString(current_address->mailbox_name);
                    from+="@";
                }
                if (current_address->host_name) {
                    from+=QString(current_address->host_name);
                }
                if (named_from && (current_address->mailbox_name || current_address->host_name)) {
                    from+=">";
                }
            }
            qDebug("header: \nFrom: %s\nSubject: %s\nDate: %s",
                   from.latin1(),
                   subject.latin1(),date.latin1());
            m = new RecMail();
            m->setSubject(subject);
            m->setFrom(from);
            m->setDate(date);
        } else if (item->msg_att_static->type==MAILIMAP_MSG_ATT_INTERNALDATE) {
            mailimap_date_time*d = item->msg_att_static->internal_date;
            QDateTime da(QDate(d->year,d->month,d->day),QTime(d->hour,d->min,d->sec));
            qDebug("%i %i %i - %i %i %i",d->year,d->month,d->day,d->hour,d->min,d->sec);
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

QString IMAPwrapper::fetchBody(const QString & mailbox,const RecMail&mail)
{
    QString body = "";
    const char *server, *user, *pass, *mb;
    uint16_t port;
    int err = MAILIMAP_NO_ERROR;
    clist *result;
    clistcell *current;
    mailimap_fetch_att *fetchAtt;
    mailimap_fetch_type *fetchType;
    mailimap_set *set;
    
    mb = mailbox.latin1();
    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();
    
    mailimap *imap = mailimap_new( 20, &imap_progress ); 
    if ( imap == NULL ) {
        qDebug("IMAP Memory error");
        return body;
    }

    /* connect */
    err = mailimap_socket_connect( imap, (char*)server, port );
    if ( err != MAILIMAP_NO_ERROR &&  
         err != MAILIMAP_NO_ERROR_AUTHENTICATED &&
         err != MAILIMAP_NO_ERROR_NON_AUTHENTICATED ) {
         qDebug("error connecting server: %s",imap->response);
         mailimap_free( imap );
         return body;
    }
    
    /* login */
    err = mailimap_login_simple( imap, (char*)user, (char*)pass );
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error logging in imap: %s",imap->response);
        err = mailimap_close( imap );
        mailimap_free( imap );
        return body;
    }
    
    /* select mailbox READONLY for operations */
    err = mailimap_examine( imap, (char*)mb);
    if ( err != MAILIMAP_NO_ERROR ) {
        qDebug("error selecting mailbox: %s",imap->response);
        err = mailimap_logout( imap );
        err = mailimap_close( imap );
        mailimap_free( imap );
        return body;
    }
    result = clist_new();  
    /* the range has to start at 1!!! not with 0!!!! */
    set = mailimap_set_new_interval( mail.getNumber(),mail.getNumber() );
    fetchAtt = mailimap_fetch_att_new_rfc822_text();
    fetchType = mailimap_fetch_type_new_fetch_att(fetchAtt);
    err = mailimap_fetch( imap, set, fetchType, &result );
    mailimap_set_free( set );
    mailimap_fetch_type_free( fetchType );
    
    if (err == MAILIMAP_NO_ERROR && (current=clist_begin(result)) ) {
        mailimap_msg_att * msg_att;
        msg_att = (mailimap_msg_att*)current->data;
        mailimap_msg_att_item*item = (mailimap_msg_att_item*)msg_att->list->first->data;

        if (item->msg_att_static && item->msg_att_static->rfc822_text) {
            body = item->msg_att_static->rfc822_text;
        }
    } else {
        qDebug("error fetching text: %s",imap->response);
    }
    
    err = mailimap_logout( imap );
    err = mailimap_close( imap );
    mailimap_free( imap );
    clist_free(result);
    
    return body;
}

