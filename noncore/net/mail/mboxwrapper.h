#ifndef __MBOX_WRAPPER_H
#define __MBOX_WRAPPER_H

#include "genericwrapper.h"
#include <qstring.h>

class RecMail;
class RecBody;

class MBOXwrapper : public Genericwrapper
{
    Q_OBJECT
public:
    MBOXwrapper(const QString & dir);
    virtual ~MBOXwrapper();
    
    virtual void listMessages(const QString & mailbox, QList<RecMail> &target );
    virtual QList<Folder>* listFolders();

    virtual void deleteMail(const RecMail&mail);
    virtual void answeredMail(const RecMail&mail);

    virtual RecBody fetchBody( const RecMail &mail );
    static void mbox_progress( size_t current, size_t maximum );

protected:
    QString MBOXPath;
};

#endif
