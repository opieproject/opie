#ifndef __NNTP_GROUP_DLG_H
#define __NNTP_GROUP_DLG_H

#include <qdialog.h>

class NNTPGroups;
class NNTPaccount;

class NNTPGroupsDlg : public QDialog
{
    Q_OBJECT
public:
    NNTPGroupsDlg(NNTPaccount *account,QWidget * parent=0, const char * name=0);
    virtual ~NNTPGroupsDlg();

protected:
    NNTPGroups*groupsWidget;
    NNTPaccount*m_Account;

protected slots:
    virtual void accept ();
};

#endif
