#ifndef __POP3WRAPPER
#define __POP3WRAPPER

#include "mailwrapper.h"
#include "abstractmail.h"
#include <qmap.h>
#include <qstring.h>

class RecMail;
class RecBody;
class encodedString;
struct mailpop3;
struct mailmessage;
struct mailmime;
struct mailmime_mechanism;

class POP3wrapper : public AbstractMail
{
    Q_OBJECT

public:
    POP3wrapper( POP3account *a );
    virtual ~POP3wrapper();
    /* mailbox will be ignored */
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();
    virtual QString fetchTextPart(const RecMail&mail,const RecPart&part);
    virtual encodedString* fetchDecodedPart(const RecMail&mail,const RecPart&part);
    virtual encodedString* fetchRawPart(const RecMail&mail,const RecPart&part);

    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);

    RecBody fetchBody( const RecMail &mail );
    static void pop3_progress( size_t current, size_t maximum );

protected:
    void login();
    void logout();
    
    RecMail *parseHeader( const char *header );
    RecBody parseMail( char *message );
    QString parseMailboxList( mailimf_mailbox_list *list );
    QString parseMailbox( mailimf_mailbox *box );
    QString parseGroup( mailimf_group *group );
    QString parseAddressList( mailimf_address_list *list );
    QString parseDateTime( mailimf_date_time *date );
    
    void cleanUpCache();
    
    void traverseBody(RecBody&target,mailmessage*message,mailmime*mime,unsigned int current_rek=0);
    static void fillSingleBody(RecPart&target,mailmessage*message,mailmime*mime);
    static void fillParameters(RecPart&target,clist*parameters);
    static QString POP3wrapper::getencoding(mailmime_mechanism*aEnc);

    POP3account *account;
    mailpop3 *m_pop3;
    QString msgTempName;
    unsigned int last_msg_id;
    QMap<QString,encodedString*> bodyCache;
};

#endif
