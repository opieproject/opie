#include "nntpgroupsdlg.h"
#include "nntpgroups.h"

#include <libmailwrapper/settings.h>

#include <qlayout.h>

NNTPGroupsDlg::NNTPGroupsDlg(NNTPaccount *account,QWidget * parent, const char * name)
    : QDialog(parent,name,true,WStyle_ContextHelp)
{
    setCaption(tr("Subscribed newsgroups"));
    m_Account = account;
    QVBoxLayout*dlglayout = new QVBoxLayout(this);
    dlglayout->setSpacing(2);
    dlglayout->setMargin(1);
    groupsWidget = new NNTPGroups(account,this);
    dlglayout->addWidget(groupsWidget);
}

NNTPGroupsDlg::~NNTPGroupsDlg()
{
}

void NNTPGroupsDlg::accept()
{
    groupsWidget->storeValues();
    m_Account->save();
    QDialog::accept();
}
