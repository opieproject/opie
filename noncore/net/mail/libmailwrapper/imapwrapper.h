#ifndef __IMAPWRAPPER
#define __IMAPWRAPPER

#include <qlist.h>
#include "mailwrapper.h"
#include "abstractmail.h"
#include <libetpan/clist.h>

struct mailimap;
struct mailimap_body;
struct mailimap_body_type_1part;
struct mailimap_body_type_text;
struct mailimap_body_type_basic;
struct mailimap_body_type_msg;
struct mailimap_body_type_mpart;
struct mailimap_body_fields;
struct mailimap_msg_att;
class encodedString;

class IMAPwrapper : public AbstractMail
{
    Q_OBJECT
public:
    IMAPwrapper( IMAPaccount *a );
    virtual ~IMAPwrapper();
    virtual QList<Folder>* listFolders();
    virtual void listMessages(const QString & mailbox,QList<RecMail>&target );
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");
    
    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);
    virtual int deleteAllMail(const Folder*folder);
    virtual void storeMessage(const char*msg,size_t length, const QString&folder);

    virtual RecBody fetchBody(const RecMail&mail);
    virtual QString fetchTextPart(const RecMail&mail,const RecPart&part);
    virtual encodedString* fetchDecodedPart(const RecMail&mail,const RecPart&part);
    virtual encodedString* fetchRawPart(const RecMail&mail,const RecPart&part);
    virtual encodedString* fetchRawBody(const RecMail&mail);

    virtual int createMbox(const QString&,const Folder*parentfolder=0,const QString& delemiter="/",bool getsubfolder=false);
    virtual int deleteMbox(const Folder*folder);
    
    static void imap_progress( size_t current, size_t maximum );
    
    virtual void logout();
    virtual const QString&getType()const;
protected:
    RecMail*parse_list_result(mailimap_msg_att*);
    void login();

    virtual QString fetchTextPart(const RecMail&mail,const QValueList<int>&path,bool internal_call=false,const QString&enc="");
    virtual encodedString*fetchRawPart(const RecMail&mail,const QValueList<int>&path,bool internal_call);
    
    void fillSinglePart(RecPart&target_part,mailimap_body_type_1part*Description);
    void fillSingleTextPart(RecPart&target_part,mailimap_body_type_text*which);
    void fillSingleBasicPart(RecPart&target_part,mailimap_body_type_basic*which);
    void fillSingleMsgPart(RecPart&target_part,mailimap_body_type_msg*which);
    void fillMultiPart(RecPart&target_part,mailimap_body_type_mpart*which);
    void traverseBody(const RecMail&mail,mailimap_body*body,RecBody&target_body,int current_recursion,QValueList<int>recList,int current_count=1);
    
    /* just helpers */
    static void fillBodyFields(RecPart&target_part,mailimap_body_fields*which);
    static QStringList address_list_to_stringlist(clist*list);


    IMAPaccount *account;
    mailimap *m_imap;
};

#endif
