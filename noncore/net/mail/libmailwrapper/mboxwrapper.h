#ifndef __MBOX_WRAPPER_H
#define __MBOX_WRAPPER_H

#include "genericwrapper.h"
#include <qstring.h>

class encodedString;
struct mailmbox_folder;

class MBOXwrapper : public Genericwrapper
{
    Q_OBJECT

public:
    MBOXwrapper(const QString & dir,const QString&name);
    virtual ~MBOXwrapper();

    virtual void listMessages(const QString & mailbox, QValueList<RecMailP>&target );
    virtual QValueList<Opie::OSmartPointer<Folder> >* listFolders();
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMailP&mail);
    virtual void answeredMail(const RecMailP&mail);

    virtual int createMbox(const QString&folder,const Opie::OSmartPointer<Folder>&f=0,
        const QString&d="",bool s=false);
    virtual int deleteMbox(const Opie::OSmartPointer<Folder>&);

    virtual void storeMessage(const char*msg,size_t length, const QString&folder);

    virtual RecBody fetchBody( const RecMailP &mail );
    static void mbox_progress( size_t current, size_t maximum );

    virtual encodedString* fetchRawBody(const RecMailP&mail);
    virtual void deleteMails(const QString & FolderName,const QValueList<RecMailP> &target);
    virtual int deleteAllMail(const Opie::OSmartPointer<Folder>&);
    virtual MAILLIB::ATYPE getType()const;
    virtual const QString&getName()const;

protected:
    static void deleteMails(mailmbox_folder*f,const QValueList<RecMailP> &target);
    QString MBOXPath;
    QString MBOXName;
};

#endif
