#ifndef __POP3WRAPPER
#define __POP3WRAPPER

#include "mailwrapper.h"
#include "genericwrapper.h"
#include <qstring.h>

class encodedString;
struct mailstorage;
struct mailfolder;

class POP3wrapper : public Genericwrapper
{
    Q_OBJECT

public:
    POP3wrapper( POP3account *a );
    virtual ~POP3wrapper();
    /* mailbox will be ignored */
    virtual void listMessages(const QString & mailbox, QValueList<Opie::OSmartPointer<RecMail> > &target );
    virtual QValueList<Opie::OSmartPointer<Folder> >* listFolders();
    /* mailbox will be ignored */
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMailP&mail);
    virtual void answeredMail(const RecMailP&mail);
    virtual int deleteAllMail(const Opie::OSmartPointer<Folder>&);

    virtual RecBody fetchBody( const RecMailP &mail );
    virtual encodedString* fetchRawBody(const RecMailP&mail);
    virtual void logout();
    virtual MAILLIB::ATYPE getType()const;
    virtual const QString&getName()const;
    static void pop3_progress( size_t current, size_t maximum );

protected:
    void login();
    POP3account *account;
    mailstorage*m_pop3;
};

#endif
