#include "mboxwrapper.h"
#include "mailtypes.h"
#include "mailwrapper.h"
#include "libetpan/libetpan.h"
#include "libetpan/mailstorage.h"
#include <qdir.h>

MBOXwrapper::MBOXwrapper(const QString & mbox_dir)
    : Genericwrapper(),MBOXPath(mbox_dir)
{
    QDir dir(MBOXPath);
    if (!dir.exists()) {
        dir.mkdir(MBOXPath);
    }
}
    
MBOXwrapper::~MBOXwrapper()
{
}

void MBOXwrapper::listMessages(const QString & mailbox, QList<RecMail> &target )
{
    mailstorage*storage = mailstorage_new(NULL);
    QString p = MBOXPath+"/";
    p+=mailbox;

    int r = mbox_mailstorage_init(storage,strdup(p.latin1()),0,0,0);
    mailfolder*folder;
    folder = mailfolder_new( storage,strdup(p.latin1()),NULL);   
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error initializing mbox");
        mailfolder_free(folder);
        mailstorage_free(storage);
        return;
    }
    mailmessage_list * env_list = 0;
    r = mailsession_get_messages_list(folder->fld_session,&env_list);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error message list");
        mailfolder_free(folder);
        mailstorage_free(storage);
        return;
    }
    r = mailsession_get_envelopes_list(folder->fld_session, env_list);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error filling message list");
        if (env_list) {
            mailmessage_list_free(env_list);
        }
        mailfolder_free(folder);
        mailstorage_free(storage);
        return;
    }
    mailimf_references * refs;

    for(int i = 0 ; i < carray_count(env_list->msg_tab) ; ++i) {
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
            qDebug("flag fetching not implemented");
            mFlags.setBit(FLAG_SEEN);
        }
        mailimf_single_fields single_fields;
        mailimf_single_fields_init(&single_fields, msg->msg_fields);
        mail->setMsgsize(msg->msg_size);
        mail->setFlags(mFlags);
        mail->setMbox(mailbox);
        mail->setNumber(i+1);
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
        if (single_fields.fld_message_id->mid_value)
            mail->setMsgid(QString(single_fields.fld_message_id->mid_value));
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
    mailfolder_disconnect(folder);
    mailfolder_free(folder);
    mailstorage_free(storage);
}

QList<Folder>* MBOXwrapper::listFolders()
{
    QList<Folder> * folders = new QList<Folder>();
    folders->setAutoDelete( false );
    QDir dir(MBOXPath);
    if (!dir.exists()) return folders;
    dir.setFilter(QDir::Files|QDir::Writable|QDir::Readable);
    QStringList entries = dir.entryList();
    QStringList::ConstIterator it = entries.begin();
    for (;it!=entries.end();++it) {
        Folder*inb=new Folder(*it,"/");
        folders->append(inb);
    }
    return folders;
}

void MBOXwrapper::deleteMail(const RecMail&mail)
{
    mailstorage*storage = mailstorage_new(NULL);
    QString p = MBOXPath+"/";
    p+=mail.getMbox();
    mailmessage * msg;
    int r = mbox_mailstorage_init(storage,strdup(p.latin1()),0,0,0);
    mailfolder*folder;
    folder = mailfolder_new( storage,strdup(p.latin1()),NULL);   
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error initializing mbox");
        mailfolder_free(folder);
        mailstorage_free(storage);
        return;
    }
    r = mailsession_remove_message(folder->fld_session,mail.getNumber());
    if (r != MAIL_NO_ERROR) {
        qDebug("error deleting mail");
    }
    mailfolder_free(folder);
    mailstorage_free(storage);
}

void MBOXwrapper::answeredMail(const RecMail&)
{
}

RecBody MBOXwrapper::fetchBody( const RecMail &mail )
{
    RecBody body;
    mailstorage*storage = mailstorage_new(NULL);
    QString p = MBOXPath+"/";
    p+=mail.getMbox();
    mailmessage * msg;
    char*data=0;
    size_t size;

    int r = mbox_mailstorage_init(storage,strdup(p.latin1()),0,0,0);
    mailfolder*folder;
    folder = mailfolder_new( storage,strdup(p.latin1()),NULL);   
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error initializing mbox");
        mailfolder_free(folder);
        mailstorage_free(storage);
        return body;
    }
    r = mailsession_get_message(folder->fld_session, mail.getNumber(), &msg);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error fetching mail %i",mail.getNumber());
        mailfolder_free(folder);
        mailstorage_free(storage);
        return body;
    }
    r = mailmessage_fetch(msg,&data,&size);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error fetching mail %i",mail.getNumber());
        mailfolder_free(folder);
        mailstorage_free(storage);
        mailmessage_free(msg);
        return body;
    }
    body = parseMail(msg);    
    mailmessage_fetch_result_free(msg,data);
    mailfolder_free(folder);
    mailstorage_free(storage);
    
    return body;
}

void MBOXwrapper::mbox_progress( size_t current, size_t maximum )
{
    qDebug("MBOX %i von %i",current,maximum);
}
