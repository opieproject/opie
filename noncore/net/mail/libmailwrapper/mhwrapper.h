#ifndef __MH_WRAPPER_H
#define __MH_WRAPPER_H

#include "maildefines.h"

#include "genericwrapper.h"
#include <qstring.h>

class encodedString;
struct mailmbox_folder;
namespace Opie {namespace Core {class OProcess;}}

class MHwrapper : public Genericwrapper
{
    Q_OBJECT
public:
    MHwrapper(const QString & dir,const QString&name);
    virtual ~MHwrapper();

    virtual void listMessages(const QString & mailbox, QValueList<Opie::Core::OSmartPointer<RecMail> > &target );
    virtual QValueList<Opie::Core::OSmartPointer<Folder> >* listFolders();
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMailP&mail);
    virtual void answeredMail(const RecMailP&mail);
    virtual void mvcpMail(const RecMailP&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);
    virtual void mvcpAllMails(const Opie::Core::OSmartPointer<Folder>&fromFolder,
        const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);

    virtual int createMbox(const QString&folder,const Opie::Core::OSmartPointer<Folder>&f=0,
        const QString&d="",bool s=false);
    virtual int deleteMbox(const Opie::Core::OSmartPointer<Folder>&);

    virtual void storeMessage(const char*msg,size_t length, const QString&folder);

    virtual RecBody fetchBody( const RecMailP &mail );
    static void mbox_progress( size_t current, size_t maximum );

    virtual encodedString* fetchRawBody(const RecMailP&mail);
    virtual void deleteMails(const QString & FolderName,const QValueList<Opie::Core::OSmartPointer<RecMail> > &target);
    virtual int deleteAllMail(const Opie::Core::OSmartPointer<Folder>&);
    virtual MAILLIB::ATYPE getType()const;
    virtual const QString&getName()const;

public slots:
    /* for deleting maildirs we are using a system call */
    virtual void oprocessStderr(Opie::Core::OProcess*, char *buffer, int );
    virtual void processEnded(Opie::Core::OProcess *);
protected:
    QString buildPath(const QString&p);
    QString MHPath;
    QString MHName;

    void init_storage();
    void clean_storage();

    bool removeMboxfailed;
};

#endif
