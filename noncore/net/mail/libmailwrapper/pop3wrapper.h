#ifndef __POP3WRAPPER
#define __POP3WRAPPER

#include "mailwrapper.h"
#include "abstractmail.h"

class RecMail;
class RecBody;
struct mailpop3;

class POP3wrapper : public AbstractMail
{
    Q_OBJECT

public:
    POP3wrapper( POP3account *a );
    virtual ~POP3wrapper();
    /* mailbox will be ignored */
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();
    virtual QString fetchPart(const RecMail&mail,const RecPart&part);
    virtual void deleteMail(const RecMail&mail);

    RecBody fetchBody( const RecMail &mail );
    static void pop3_progress( size_t current, size_t maximum );

protected:
    void login();
    void logout();

private:
    RecMail *parseHeader( const char *header );
    RecBody parseBody( const char *message );
    QString parseMailboxList( mailimf_mailbox_list *list );
    QString parseMailbox( mailimf_mailbox *box );
    QString parseGroup( mailimf_group *group );
    QString parseAddressList( mailimf_address_list *list );
    QString parseDateTime( mailimf_date_time *date );
    POP3account *account;
    mailpop3 *m_pop3;
};

#endif
