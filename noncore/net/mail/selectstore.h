#ifndef _SELECTSTORE_H
#define _SELECTSTORE_H

#include "selectstoreui.h"
#include <qvaluelist.h>
#include <qmap.h>
#include <qstringlist.h>

class AbstractMail;

class Selectstore:public selectstoreui
{
    Q_OBJECT
public:
    Selectstore(QWidget* parent = 0, const char* name = 0);
    virtual ~Selectstore();
    virtual void addAccounts(AbstractMail*mail,const QStringList&folders);
    virtual QString currentFolder();
    virtual AbstractMail*currentMail();
    bool newFolder();
    bool moveMails();

protected slots:
    virtual void slotCreateNewFolder();
    virtual void slotMoveMail();
    virtual void slotAccountselected(int);

protected:
    QMap<int,QStringList> folderMap;
    QMap<int,AbstractMail*> mailMap;
    AbstractMail*cMail;
};
#endif
