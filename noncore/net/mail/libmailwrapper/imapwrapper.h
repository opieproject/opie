#ifndef __IMAPWRAPPER
#define __IMAPWRAPPER

#include <qlist.h>
#include "mailwrapper.h"

struct mailimap;
struct mailimap_body_type_1part;
struct mailimap_body_type_text;
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
    void searchBodyText(const RecMail&mail,mailimap_body_type_1part*mailDescription,RecBody&target_body);
    void fillPlainBody(const RecMail&mail,RecBody&target_body, mailimap_body_type_text * text_body);
    static QStringList address_list_to_stringlist(clist*list);

private:
    IMAPaccount *account;
    mailimap *m_imap;
};

#endif
