#ifndef __MAIL_STATUS_H
#define __MAIL_STATUS_H

#include "settings.h"
#include "abstractmail.h"
#include "mailtypes.h"

class StatusMail
{
public:
    StatusMail(QList<Account>&list);
    virtual ~StatusMail();
    
    /* this should be called if opiemail is starte or a mailbox touched - may be trough
       a qcop signal or if tab on the taskbar applet*/
    virtual void reset_status();
    virtual void check_current_stat(folderStat&targetStat);

protected:
    void initAccounts(QList<Account>&accounts);
    /* this must be cause we have to calculate the recent for pop3*/
    folderStat currentImapStat;
    /* currentPop3Stat is the sum of messages in POP3 accounts in CURRENT loop
       the recent are calculated to the difference of the LAST loop */
    folderStat currentPop3Stat;
    /* lastPop3Stat is the sum of messages in LAST loop */
    folderStat lastPop3Stat;
    QList<AbstractMail> connectionList;
};

#endif
