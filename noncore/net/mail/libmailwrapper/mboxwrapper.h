#ifndef __MBOX_WRAPPER_H
#define __MBOX_WRAPPER_H

#include "genericwrapper.h"
#include <qstring.h>

class RecMail;
class RecBody;
struct mailmbox_folder;

class MBOXwrapper : public Genericwrapper
{
    Q_OBJECT
public:
    MBOXwrapper(const QString & dir);
    virtual ~MBOXwrapper();
    
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);
    
    virtual int createMbox(const QString&folder,const Folder*f=0,const QString&d="",bool s=false);
    virtual int deleteMbox(const Folder*);

    virtual void storeMessage(const char*msg,size_t length, const QString&folder);

    virtual RecBody fetchBody( const RecMail &mail );
    static void mbox_progress( size_t current, size_t maximum );

    virtual void fetchRawBody(const RecMail&mail,char**target,size_t*length);
    virtual void deleteMails(const QString & mailbox,QList<RecMail> &target);
    virtual int deleteAllMail(const Folder*);
    virtual const QString&getType()const;

protected:
    static void deleteMails(mailmbox_folder*f,QList<RecMail> &target);
    QString MBOXPath;
    static const QString wrapperType;
};

#endif
