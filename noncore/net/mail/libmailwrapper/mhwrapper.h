#ifndef __MH_WRAPPER_H
#define __MH_WRAPPER_H

#include "genericwrapper.h"
#include <qstring.h>

class RecMail;
class RecBody;
class encodedString;
struct mailmbox_folder;
class OProcess;

class MHwrapper : public Genericwrapper
{
    Q_OBJECT
public:
    MHwrapper(const QString & dir,const QString&name);
    virtual ~MHwrapper();
    
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);
    virtual void mvcpMail(const RecMail&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);
    virtual void mvcpAllMails(Folder*fromFolder,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);
    
    virtual int createMbox(const QString&folder,const Folder*f=0,const QString&d="",bool s=false);
    virtual int deleteMbox(const Folder*);

    virtual void storeMessage(const char*msg,size_t length, const QString&folder);

    virtual RecBody fetchBody( const RecMail &mail );
    static void mbox_progress( size_t current, size_t maximum );

    virtual encodedString* fetchRawBody(const RecMail&mail);
    virtual void deleteMails(const QString & FolderName,QList<RecMail> &target);
    virtual int deleteAllMail(const Folder*);
    virtual const QString&getType()const;
    virtual const QString&getName()const;

public slots:
    /* for deleting maildirs we are using a system call */
    virtual void oprocessStderr(OProcess*, char *buffer, int );
    virtual void processEnded(OProcess *);
protected:
    QString buildPath(const QString&p);
    QString MHPath;
    QString MHName;
    static const QString wrapperType;
    
    void init_storage();
    void clean_storage();
    
    bool removeMboxfailed;
};

#endif
