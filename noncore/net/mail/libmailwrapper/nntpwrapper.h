#ifndef __NNTPWRAPPER
#define __NNTPWRAPPER

#include "mailwrapper.h"
#include "genericwrapper.h"
#include <qstring.h>
#include <libetpan/clist.h>

class encodedString;
struct mailstorage;
struct mailfolder;

class NNTPwrapper : public Genericwrapper
{

 Q_OBJECT

public:
    NNTPwrapper( NNTPaccount *a );
    virtual ~NNTPwrapper();

   /* mailbox will be ignored */
    virtual void listMessages(const QString & mailbox, QValueList<Opie::OSmartPointer<RecMail> > &target );
    /* should only get the subscribed one */
    virtual QValueList<Opie::OSmartPointer<Folder> >* listFolders();
    /* mailbox will be ignored */
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");
    QStringList listAllNewsgroups(const QString&mask = QString::null);
    virtual void deleteMail(const RecMailP&mail);
    virtual void answeredMail(const RecMailP&mail);
    virtual int deleteAllMail(const Opie::OSmartPointer<Folder>&);

    virtual RecBody fetchBody( const RecMailP &mail );
    virtual encodedString* fetchRawBody(const RecMailP&mail);
    virtual void logout();
    virtual MAILLIB::ATYPE getType()const;
    virtual const QString&getName()const;
    static void nntp_progress( size_t current, size_t maximum );

protected:
    void login();
    NNTPaccount *account;
    mailstorage* m_nntp;


};

#endif
