#ifndef __abstract_mail_
#define __abstract_mail_

#include <qobject.h>
#include "settings.h"

class RecMail;
class RecBody;
class RecPart;
class IMAPwrapper;
class POP3wrapper;
class Folder;
class encodedString;
struct folderStat;

class AbstractMail:public QObject
{
    Q_OBJECT
public:
    AbstractMail(){};
    virtual ~AbstractMail(){}
    virtual QList<Folder>* listFolders()=0;
    virtual void listMessages(const QString & mailbox,QList<RecMail>&target )=0;
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX")=0;
    virtual RecBody fetchBody(const RecMail&mail)=0;
    virtual QString fetchTextPart(const RecMail&mail,const RecPart&part)=0;
    virtual encodedString* fetchDecodedPart(const RecMail&mail,const RecPart&part)=0;
    virtual encodedString* fetchRawPart(const RecMail&mail,const RecPart&part)=0;

    virtual void deleteMail(const RecMail&mail)=0;
    virtual void answeredMail(const RecMail&mail)=0;
    virtual int deleteAllMail(const Folder*)=0;
    virtual int deleteMbox(const Folder*)=0;
    virtual void storeMessage(const char*msg,size_t length, const QString&folder)=0;

    virtual void cleanMimeCache(){};    
    /* mail box methods */
    /* parameter is the box to create.
     * if the implementing subclass has prefixes, 
     * them has to be appended automatic.
     */
    virtual int createMbox(const QString&,const Folder*parentfolder=0,const QString& delemiter="/",bool getsubfolder=false);
    virtual void logout()=0;
    
    static AbstractMail* getWrapper(IMAPaccount *a);
    static AbstractMail* getWrapper(POP3account *a);
    /* mbox only! */
    static AbstractMail* getWrapper(const QString&a);

    static QString defaultLocalfolder();
    
protected:
    static encodedString*decode_String(const encodedString*text,const QString&enc);
    static QString convert_String(const char*text);
    static QString gen_attachment_id();
};
#endif
