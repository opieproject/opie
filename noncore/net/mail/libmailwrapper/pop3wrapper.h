#ifndef __POP3WRAPPER
#define __POP3WRAPPER

#include "mailwrapper.h"
#include "genericwrapper.h"
#include <qstring.h>

struct mailstorage;
struct mailfolder;

class POP3wrapper : public Genericwrapper
{
    Q_OBJECT

public:
    POP3wrapper( POP3account *a );
    virtual ~POP3wrapper();
    /* mailbox will be ignored */
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();
    /* mailbox will be ignored */
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);
    virtual int deleteAllMail(const Folder*);

    virtual RecBody fetchBody( const RecMail &mail );
    static void pop3_progress( size_t current, size_t maximum );

protected:
    void login();
    void logout();
    POP3account *account;
    mailstorage*m_pop3;
    mailfolder*m_folder;
};

#endif
