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

class AbstractMail:public QObject
{
    Q_OBJECT
public:
    AbstractMail(){};
    virtual ~AbstractMail(){}
    virtual QList<Folder>* listFolders()=0;
    virtual void listMessages(const QString & mailbox,QList<RecMail>&target )=0;
    virtual RecBody fetchBody(const RecMail&mail)=0;
    virtual QString fetchPart(const RecMail&mail,const QValueList<int>&path,bool internal_call=false)=0;
    virtual QString fetchPart(const RecMail&mail,const RecPart&part)=0;
    
    static AbstractMail* getWrapper(IMAPaccount *a);
    static AbstractMail* getWrapper(POP3account *a);
};

#endif
