#include "selectstore.h"
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <libmailwrapper/abstractmail.h>

Selectstore::Selectstore(QWidget* parent, const char* name)
    :selectstoreui(parent,name,true)
{
    folderMap.clear();
    mailMap.clear();
    cMail = 0;
}

Selectstore::~Selectstore()
{
}

void Selectstore::slotCreateNewFolder()
{
    bool how = newFoldersel->isChecked();
    folderSelection->setEnabled(!how);
    newFolderedit->setEnabled(how);
}

void Selectstore::slotMoveMail()
{
}

void Selectstore::addAccounts(AbstractMail*mail,const QStringList&folders)
{
    accountSelection->insertItem(mail->getName());
    mailMap[accountSelection->count()-1]=mail;
    folderMap[accountSelection->count()-1]=folders;
    if (accountSelection->count()==1) {
        cMail = mail;
        folderSelection->insertStringList(folders);
    }
}

void Selectstore::slotAccountselected(int i)
{
    folderSelection->clear();
    folderSelection->insertStringList(folderMap[i]);
    cMail = mailMap[i];
}

QString Selectstore::currentFolder()
{
    if (newFoldersel->isChecked()) {
        return newFolderedit->text();
    } else {
        return folderSelection->currentText();
    }
}

AbstractMail*Selectstore::currentMail()
{
    return cMail;
}

bool Selectstore::newFolder()
{
    return newFoldersel->isChecked();
}

bool Selectstore::moveMails()
{
    return selMove->isChecked();
}
