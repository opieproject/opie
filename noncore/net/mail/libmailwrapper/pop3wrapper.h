#ifndef __POP3WRAPPER
#define __POP3WRAPPER

#include "mailwrapper.h"

class RecMail;

class POP3wrapper : public QObject
{
    Q_OBJECT

public:
    POP3wrapper( POP3account *a );
    virtual ~POP3wrapper();
    void listMessages( QList<RecMail> &target );
    static void pop3_progress( size_t current, size_t maximum );

protected:
    void login();
    void logout();

private:
    POP3account *account;
    mailpop3 *m_pop3;

};

#endif
