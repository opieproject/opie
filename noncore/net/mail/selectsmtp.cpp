#include "selectsmtp.h"
#include <libmailwrapper/mailwrapper.h>
#include <qlist.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qcombobox.h>

selectsmtp::selectsmtp(QWidget* parent, const char* name, bool modal, WFlags fl)
    : selectstoreui(parent,name,modal,fl)
{
    //m_smtpList.setAutoDelete(false);
    m_smtpList = 0;
    //headlabel->setText(tr("<center>Select SMTP account to use</center>"));
    headlabel->hide();
    folderSelection->hide();
    folderLabel->hide();
    accountlabel->setText("<center>SMTP Accounts</center>");
    Line1->hide();
    newFoldersel->hide();
    newFolderedit->hide();
    newFolderLabel->hide();
    Line2->hide();
    selMove->hide();
    m_current_smtp = 0;
    setCaption(tr("Select SMTP Account"));
}

selectsmtp::~selectsmtp()
{
}

void selectsmtp::slotAccountselected(int which)
{
    if (!m_smtpList || (unsigned)which>=m_smtpList->count() || which < 0) {
        m_current_smtp = 0;
        return;
    }
    m_current_smtp = m_smtpList->at(which);
}

void selectsmtp::setSelectionlist(QList<SMTPaccount>*list)
{
    m_smtpList = list;
    accountSelection->clear();
    if (!m_smtpList || m_smtpList->count()==0) {
        accountSelection->setEnabled(false);
        return;
    }
    accountSelection->setEnabled(true);
    for (unsigned i = 0; m_smtpList!=0 && i < m_smtpList->count(); ++i) {
        accountSelection->insertItem( m_smtpList->at(i)->getAccountName());
    }
    m_current_smtp = m_smtpList->at(0);
}

SMTPaccount*selectsmtp::selected_smtp()
{
    return m_current_smtp;
}
