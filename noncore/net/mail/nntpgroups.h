#ifndef __NNTPGROUPS_WINDOW__
#define __NNTPGROUPS_WINDOW__

#include "nntpgroupsui.h"

#include <libmailwrapper/nntpwrapper.h>

class NNTPaccount;
class QStringList;

class NNTPGroups:public NNTPGroupsUI
{
    Q_OBJECT
public:
    NNTPGroups(NNTPaccount *account, QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    virtual ~NNTPGroups();
    /* must be called from external.
     * it will store the new subscription list into the account
     * but don't save them, this must be done by the calling class.
     */
    void storeValues();

protected slots:
    virtual void slotGetNG();
    
protected:
    virtual void fillGroups();

    NNTPaccount*m_Account;
    QStringList subscribedGroups;
};

#endif
