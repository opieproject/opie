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
    virtual QValueList<Opie::Core::OSmartPointer<Folder> >* listFolders();
    virtual void listMessages(const QString & mailbox,QValueList<Opie::Core::OSmartPointer<RecMail> >&target );
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMailP&mail);
    virtual void answeredMail(const RecMailP&mail);
    virtual int deleteAllMail(const Opie::Core::OSmartPointer<Folder>&folder);
    virtual void storeMessage(const char*msg,size_t length, const QString&folder);
    virtual void mvcpAllMails(const Opie::Core::OSmartPointer<Folder>&fromFolder,
        const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);
    virtual void mvcpMail(const RecMailP&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);

    virtual RecBodyP fetchBody(const RecMailP&mail);
    virtual QString fetchTextPart(const RecMailP&mail,const RecPartP&part);
    virtual encodedString* fetchDecodedPart(const RecMailP&mail,const RecPartP&part);
    virtual encodedString* fetchRawPart(const RecMailP&mail,const RecPartP&part);
    virtual encodedString* fetchRawBody(const RecMailP&mail);

    virtual int createMbox(const QString&,const Opie::Core::OSmartPointer<Folder>&parentfolder=0,
        const QString& delemiter="/",bool getsubfolder=false);
    virtual int deleteMbox(const Opie::Core::OSmartPointer<Folder>&folder);

    static void imap_progress( size_t current, size_t maximum );

    virtual void logout();
    virtual MAILLIB::ATYPE getType()const;
    virtual const QString&getName()const;

protected:
    RecMail*parse_list_result(mailimap_msg_att*);
    void login();
    bool start_tls(bool force=true);

    virtual QString fetchTextPart(const RecMailP&mail,const QValueList<int>&path,bool internal_call=false,const QString&enc="");
    virtual encodedString*fetchRawPart(const RecMailP&mail,const QValueList<int>&path,bool internal_call);
    int selectMbox(const QString&mbox);

    void fillSinglePart(RecPartP&target_part,mailimap_body_type_1part*Description);
    void fillSingleTextPart(RecPartP&target_part,mailimap_body_type_text*which);
    void fillSingleBasicPart(RecPartP&target_part,mailimap_body_type_basic*which);
    void fillSingleMsgPart(RecPartP&target_part,mailimap_body_type_msg*which);
    void fillMultiPart(RecPartP&target_part,mailimap_body_type_mpart*which);
    void traverseBody(const RecMailP&mail,mailimap_body*body,RecBodyP&target_body,int current_recursion,QValueList<int>recList,int current_count=1);

    /* just helpers */
    static void fillBodyFields(RecPartP&target_part,mailimap_body_fields*which);
    static QStringList address_list_to_stringlist(clist*list);


    IMAPaccount *account;
    mailimap *m_imap;
    QString m_Lastmbox;
};

#endif
