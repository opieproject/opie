#ifndef __POP3WRAPPER
#define __POP3WRAPPER

#include "mailwrapper.h"
#include "genericwrapper.h"
#include <qstring.h>


class POP3wrapper : public Genericwrapper
{
    Q_OBJECT

public:
    POP3wrapper( POP3account *a );
    virtual ~POP3wrapper();
    /* mailbox will be ignored */
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();

    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);

    virtual RecBody fetchBody( const RecMail &mail );
    static void pop3_progress( size_t current, size_t maximum );

protected:
    void login();
    void logout();
    
};

#endif
