#ifndef __abstract_mail_
#define __abstract_mail_

#include "maildefines.h"

#include "settings.h"

#include <qobject.h>
#include <opie2/osmartpointer.h>
#include "mailtypes.h"

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
    virtual QValueList<Opie::Core::OSmartPointer<Folder> >* listFolders()=0;
    virtual void listMessages(const QString & mailbox,QValueList<RecMailP>&target )=0;
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX")=0;
    virtual RecBodyP fetchBody(const RecMailP&mail)=0;
    virtual QString fetchTextPart(const RecMailP&mail,const RecPartP&part)=0;
    virtual encodedString* fetchDecodedPart(const RecMailP&mail,const RecPartP&part)=0;
    virtual encodedString* fetchRawPart(const RecMailP&mail,const RecPartP&part)=0;
    virtual encodedString* fetchRawBody(const RecMailP&mail)=0;

    virtual void deleteMail(const RecMailP&mail)=0;
    virtual void answeredMail(const RecMailP&mail)=0;
    virtual int deleteAllMail(const Opie::Core::OSmartPointer<Folder>&)=0;
    virtual void deleteMails(const QString & FolderName,const QValueList<Opie::Core::OSmartPointer<RecMail> >&target);
    virtual int deleteMbox(const Opie::Core::OSmartPointer<Folder>&)=0;
    virtual void storeMessage(const char*msg,size_t length, const QString&folder)=0;

    virtual void mvcpAllMails(const Opie::Core::OSmartPointer<Folder>&fromFolder,
        const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);
    virtual void mvcpMail(const RecMailP&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);

    virtual void cleanMimeCache(){};
    /* mail box methods */
    /* parameter is the box to create.
     * if the implementing subclass has prefixes,
     * them has to be appended automatic.
     */
    virtual int createMbox(const QString&,const Opie::Core::OSmartPointer<Folder>&parentfolder=0,
        const QString& delemiter="/",bool getsubfolder=false);
    virtual void logout()=0;

    static AbstractMail* getWrapper(IMAPaccount *a);
    static AbstractMail* getWrapper(POP3account *a);
    static AbstractMail* getWrapper(NNTPaccount *a);
    /* mbox only! */
    static AbstractMail* getWrapper(const QString&a,const QString&name="Local Folders");
    static AbstractMail* getWrapper(Account*a);

    static QString defaultLocalfolder();
    static QString draftFolder();

    virtual MAILLIB::ATYPE getType()const=0;
    virtual const QString&getName()const=0;

protected:
    static encodedString*decode_String(const encodedString*text,const QString&enc);
    static QString convert_String(const char*text);
    static QString gen_attachment_id();
};
#endif
