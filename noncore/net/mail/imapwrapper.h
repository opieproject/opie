#ifndef __IMAPWRAPPER
#define __IMAPWRAPPER

#include <qlist.h>
#include "mailwrapper.h"

struct mailimap;
struct mailimap_body_type_1part;
class RecMail;
class RecBody;

class IMAPwrapper : public QObject
{
    Q_OBJECT

public:
    IMAPwrapper( IMAPaccount *a );
    virtual ~IMAPwrapper();
    QList<IMAPFolder>* listFolders();
    void listMessages(const QString & mailbox,QList<RecMail>&target );
    RecBody fetchBody(const RecMail&mail);
    static void imap_progress( size_t current, size_t maximum );

protected:
    RecMail*parse_list_result(mailimap_msg_att*);
    void login();
    void logout();
    QString searchBodyText(const RecMail&mail,mailimap_body_type_1part*mailDescription);
    QString getPlainBody(const RecMail&mail);

private:
    IMAPaccount *account;
    mailimap *m_imap;
};

#endif
