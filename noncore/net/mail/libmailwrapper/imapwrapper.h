#ifndef __IMAPWRAPPER
#define __IMAPWRAPPER

#include <qlist.h>
#include "mailwrapper.h"
#include "abstractmail.h"

struct mailimap;
struct mailimap_body_type_1part;
struct mailimap_body_type_text;
struct mailimap_body_type_basic;
struct mailimap_body_type_msg;
struct mailimap_body_type_mpart;
struct mailimap_body_fields;
struct mailimap_msg_att;

class IMAPwrapper : public AbstractMail
{
    Q_OBJECT
public:
    IMAPwrapper( IMAPaccount *a );
    virtual ~IMAPwrapper();
    virtual QList<Folder>* listFolders();
    virtual void listMessages(const QString & mailbox,QList<RecMail>&target );
    virtual RecBody fetchBody(const RecMail&mail);
    virtual QString fetchPart(const RecMail&mail,const RecPart&part);
    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);

    static void imap_progress( size_t current, size_t maximum );

protected:
    RecMail*parse_list_result(mailimap_msg_att*);
    void login();
    void logout();

    virtual QString fetchPart(const RecMail&mail,const QValueList<int>&path,bool internal_call=false);
    
    void searchBodyText(const RecMail&mail,mailimap_body_type_1part*mailDescription,RecBody&target_body);
    void searchBodyText(const RecMail&mail,mailimap_body_type_mpart*mailDescription,RecBody&target_body,int current_recursion=0,QValueList<int>recList=QValueList<int>());

    void fillSinglePart(RecPart&target_part,mailimap_body_type_1part*Description);
    void fillSingleTextPart(RecPart&target_part,mailimap_body_type_text*which);
    void fillSingleBasicPart(RecPart&target_part,mailimap_body_type_basic*which);
    void fillSingleMsgPart(RecPart&target_part,mailimap_body_type_msg*which);
    
    /* just helpers */
    static void fillBodyFields(RecPart&target_part,mailimap_body_fields*which);
    static QStringList address_list_to_stringlist(clist*list);

private:
    IMAPaccount *account;
    mailimap *m_imap;
};

#endif
