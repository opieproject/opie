#ifndef __IMAPWRAPPER
#define __IMAPWRAPPER

#include "mailwrapper.h"
#include <libetpan/mailimap.h>

class IMAPwrapper : public QObject
{
    Q_OBJECT

public:
    IMAPwrapper( IMAPaccount *a );
    QList<IMAPFolder>* listFolders();
    void listMessages(const QString & mailbox,Maillist&target );
    QString fetchBody(const QString & mailbox,const RecMail&mail);

protected:
    RecMail*parse_list_result(mailimap_msg_att*);

private:
    IMAPaccount *account;
    
};

#endif
