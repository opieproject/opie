#include "mboxwrapper.h"
#include "mailtypes.h"
#include "mailwrapper.h"
#include <libetpan/libetpan.h>
#include <qdir.h>
#include <stdlib.h>
#include <qpe/global.h>

using namespace Opie::Core;
MBOXwrapper::MBOXwrapper(const QString & mbox_dir,const QString&mbox_name)
    : Genericwrapper(),MBOXPath(mbox_dir),MBOXName(mbox_name)
{
    QDir dir(MBOXPath);
    if (!dir.exists()) {
        dir.mkdir(MBOXPath);
    }
}

MBOXwrapper::~MBOXwrapper()
{
}

void MBOXwrapper::listMessages(const QString & mailbox, QValueList<RecMailP> &target )
{
    mailstorage*storage = mailstorage_new(NULL);
    QString p = MBOXPath+"/";
    p+=mailbox;

    int r = mbox_mailstorage_init(storage,(char*)p.latin1(),0,0,0);
    mailfolder*folder;
    folder = mailfolder_new( storage,(char*)p.latin1(),NULL);
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error initializing mbox");
        mailfolder_free(folder);
        mailstorage_free(storage);
        return;
    }

    parseList(target,folder->fld_session,mailbox);

    mailfolder_disconnect(folder);
    mailfolder_free(folder);
    mailstorage_free(storage);
    Global::statusMessage(tr("Mailbox has %1 mail(s)").arg(target.count()));
}

QValueList<Opie::Core::OSmartPointer<Folder> >* MBOXwrapper::listFolders()
{
    QValueList<Opie::Core::OSmartPointer<Folder> >* folders = new QValueList<Opie::Core::OSmartPointer<Folder> >();
    QDir dir(MBOXPath);
    if (!dir.exists()) return folders;
    dir.setFilter(QDir::Files|QDir::Writable|QDir::Readable);
    QStringList entries = dir.entryList();
    QStringList::ConstIterator it = entries.begin();
    for (;it!=entries.end();++it) {
        FolderP inb=new Folder(*it,"/");
        folders->append(inb);
    }
    return folders;
}

void MBOXwrapper::deleteMail(const RecMailP & mail)
{
    mailstorage*storage = mailstorage_new(NULL);
    QString p = MBOXPath+"/";
    p+=mail->getMbox();
    int r = mbox_mailstorage_init(storage,(char*)p.latin1(),0,0,0);
    mailfolder*folder;
    folder = mailfolder_new( storage,(char*)p.latin1(),NULL);
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error initializing mbox");
        mailfolder_free(folder);
        mailstorage_free(storage);
        return;
    }
    r = mailsession_remove_message(folder->fld_session,mail->getNumber());
    if (r != MAIL_NO_ERROR) {
        qDebug("error deleting mail");
    }
    mailfolder_free(folder);
    mailstorage_free(storage);
}

void MBOXwrapper::answeredMail(const RecMailP&)
{
}

RecBody MBOXwrapper::fetchBody( const RecMailP &mail )
{
    RecBody body;
    mailstorage*storage = mailstorage_new(NULL);
    QString p = MBOXPath+"/";
    p+=mail->getMbox();
    mailmessage * msg;
    char*data=0;
    size_t size;

    int r = mbox_mailstorage_init(storage,(char*)p.latin1(),0,0,0);
    mailfolder*folder;
    folder = mailfolder_new( storage,(char*)p.latin1(),NULL);
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error initializing mbox");
        mailfolder_free(folder);
        mailstorage_free(storage);
        return body;
    }
    r = mailsession_get_message(folder->fld_session, mail->getNumber(), &msg);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error fetching mail %i",mail->getNumber());
        mailfolder_free(folder);
        mailstorage_free(storage);
        return body;
    }
    r = mailmessage_fetch(msg,&data,&size);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error fetching mail %i",mail->getNumber());
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

int MBOXwrapper::createMbox(const QString&folder,const FolderP&,const QString&,bool )
{
    QString p = MBOXPath+"/";
    p+=folder;
    QFileInfo fi(p);
    if (fi.exists()) {
        Global::statusMessage(tr("Mailbox exists."));
        return 0;
    }
    mailmbox_folder*f = 0;
    if (mailmbox_init(p.latin1(),0,1,0,&f) != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error init folder"));
        return 0;
    }
    if (f) mailmbox_done(f);
    return 1;
}

void MBOXwrapper::storeMessage(const char*msg,size_t length, const QString&folder)
{
    QString p = MBOXPath+"/";
    p+=folder;
    mailmbox_folder*f = 0;
    int r = mailmbox_init(p.latin1(),0,1,0,&f);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error init folder"));
        return;
    }
    r = mailmbox_append_message(f,msg,length);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error writing to message folder"));
    }
    mailmbox_done(f);
}

encodedString* MBOXwrapper::fetchRawBody(const RecMailP&mail)
{
    RecBody body;
    mailstorage*storage = mailstorage_new(NULL);
    QString p = MBOXPath+"/";
    p+=mail->getMbox();
    mailmessage * msg;
    char*data=0;
    size_t size;

    int r = mbox_mailstorage_init(storage,(char*)p.latin1(),0,0,0);
    mailfolder*folder;
    folder = mailfolder_new( storage,(char*)p.latin1(),NULL);
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error initializing mbox"));
        mailfolder_free(folder);
        mailstorage_free(storage);
        return 0;
    }
    r = mailsession_get_message(folder->fld_session, mail->getNumber(), &msg);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error fetching mail %i").arg(mail->getNumber()));
        mailfolder_free(folder);
        mailstorage_free(storage);
        return 0;
    }
    r = mailmessage_fetch(msg,&data,&size);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error fetching mail %i").arg(mail->getNumber()));
        mailfolder_free(folder);
        mailstorage_free(storage);
        mailmessage_free(msg);
        return 0;
    }
    encodedString*result = new encodedString(data,size);

    mailfolder_free(folder);
    mailstorage_free(storage);
    mailmessage_free(msg);
    return result;
}

void MBOXwrapper::deleteMails(const QString & mailbox,const QValueList<RecMailP> &target)
{
    QString p = MBOXPath+"/";
    p+=mailbox;
    mailmbox_folder*f = 0;
    int r = mailmbox_init(p.latin1(),0,1,0,&f);
    if (r != MAIL_NO_ERROR) {
        qDebug("Error init folder");
        return;
    }
    deleteMails(f,target);
    mailmbox_done(f);
}

void MBOXwrapper::deleteMails(mailmbox_folder*f,const QValueList<RecMailP> &target)
{
    if (!f) return;
    int r;
    QValueList<RecMailP>::ConstIterator it;
    for (it=target.begin(); it != target.end();++it) {
        r = mailmbox_delete_msg(f,(*it)->getNumber());
        if (r!=MAILMBOX_NO_ERROR) {
            qDebug("error delete mail");
        }
    }
    r = mailmbox_expunge(f);
    if (r != MAILMBOX_NO_ERROR) {
        qDebug("error expunge mailbox");
    }
}

int MBOXwrapper::deleteAllMail(const FolderP&tfolder)
{
    if (!tfolder) return 0;
    QString p = MBOXPath+"/"+tfolder->getDisplayName();
    int res = 1;

    mailfolder*folder = 0;
    mailmessage_list*l=0;
    mailstorage*storage = mailstorage_new(NULL);
    int r = mbox_mailstorage_init(storage,(char*)p.latin1(),0,0,0);
    if (r != MAIL_NO_ERROR) {
        Global::statusMessage(tr("Error initializing mbox"));
        res = 0;
    }
    if (res) {
        folder = mailfolder_new( storage,(char*)p.latin1(),NULL);
        r = mailfolder_connect(folder);
        if (r != MAIL_NO_ERROR) {
            Global::statusMessage(tr("Error initializing mbox"));
            res = 0;
        }
    }
    if (res) {
        r = mailsession_get_messages_list(folder->fld_session,&l);
        if (r != MAIL_NO_ERROR) {
            qDebug("Error message list");
            res=0;
        }
    }
    for(unsigned int i = 0 ; l!= 0 && res==1 && i < carray_count(l->msg_tab) ; ++i) {
        r = mailsession_remove_message(folder->fld_session,i+1);
        if (r != MAIL_NO_ERROR) {
            Global::statusMessage(tr("Error deleting mail %1").arg(i+1));
            res = 0;
            break;
        }
    }
    if (l) mailmessage_list_free(l);
    if (folder) mailfolder_free(folder);
    if (storage) mailstorage_free(storage);
    return res;
}

int MBOXwrapper::deleteMbox(const FolderP&tfolder)
{
    if (!tfolder) return 0;
    QString p = MBOXPath+"/"+tfolder->getDisplayName();
    QFile fi(p);
    if (!fi.exists()) {
        Global::statusMessage(tr("Mailbox doesn't exist."));
        return 0;
    }
    if (!fi.remove()) {
        Global::statusMessage(tr("Error deleting Mailbox."));
        return 0;
    }
    return 1;
}

void MBOXwrapper::statusFolder(folderStat&target_stat,const QString & mailbox)
{
    mailfolder*folder = 0;
    mailstorage*storage = mailstorage_new(NULL);
    target_stat.message_count = 0;
    target_stat.message_unseen = 0;
    target_stat.message_recent = 0;
    QString p = MBOXPath+"/"+mailbox;
    QFile fi(p);
    if (!fi.exists()) {
        Global::statusMessage(tr("Mailbox doesn't exist."));
        return;
    }
    int r = mbox_mailstorage_init(storage,(char*)p.latin1(),0,0,0);
    folder = mailfolder_new( storage,(char*)p.latin1(),NULL);
    r = mailfolder_connect(folder);
    r = mailsession_status_folder(folder->fld_session,(char*)mailbox.latin1(),&target_stat.message_count,
        &target_stat.message_recent,&target_stat.message_unseen);
    if (folder) mailfolder_free(folder);
    if (storage) mailstorage_free(storage);
}

MAILLIB::ATYPE MBOXwrapper::getType()const
{
    return MAILLIB::A_MBOX;
}

const QString&MBOXwrapper::getName()const
{
    return MBOXName;
}
