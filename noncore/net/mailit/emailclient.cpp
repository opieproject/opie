/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include <qapplication.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qcheckbox.h>
#include <qmenubar.h>
#include <qaction.h>
#include "resource.h"
#include "emailclient.h"

QCollection::Item AccountList::newItem(QCollection::Item d)
{
  return dupl( (MailAccount *) d);
}

MailAccount* AccountList::dupl(MailAccount *in)
{
  ac = new MailAccount(*in);
  return ac;
}

EmailClient::EmailClient( QWidget* parent,  const char* name, WFlags fl )
  : QMainWindow( parent, name, fl )
{
  emailHandler = new EmailHandler();
  addressList = new AddressList( getPath(FALSE) + "mail_adr");
  
  sending = FALSE;
  receiving = FALSE;
  previewingMail = FALSE;
  mailIdCount = 1;
  accountIdCount = 1;
  allAccounts = FALSE;
  
  init();
  
  connect(emailHandler, SIGNAL(mailSent()), this, SLOT(mailSent()) );
  
  connect(emailHandler, SIGNAL(smtpError(int)), this,
      SLOT(smtpError(int)) );
  connect(emailHandler, SIGNAL(popError(int)), this,
      SLOT(popError(int)) );
  
  connect(inboxView, SIGNAL(clicked(QListViewItem *)), this, SLOT(inboxItemSelected()) );
  connect(outboxView, SIGNAL(clicked(QListViewItem *)), this, SLOT(outboxItemSelected()) );
  
  connect(emailHandler, SIGNAL(mailArrived(const Email &, bool)), this,
      SLOT(mailArrived(const Email &, bool)) );
  connect(emailHandler, SIGNAL(mailTransfered(int)), this,
      SLOT(allMailArrived(int)) );
  
  mailconf = new Config("mailit");
  //In case Synchronize is not defined in settings.txt

  readSettings();
  
  updateAccounts();
      
  lineShift = "\n";
  readMail();
  lineShift = "\r\n";
  
}


EmailClient::~EmailClient()
{
  //needs to be moved from destructor to closewindow event
  saveMail(getPath(FALSE) + "inbox.txt", inboxView);
  //does not currently work.  Defining outbox in the same
  //format as inbox is not a good solution as they have
  //different properties
  saveMail(getPath(FALSE) + "outbox.txt", outboxView);
  saveSettings();
  
  mailconf->write();
  delete mailconf;
  
}

void EmailClient::init()
{
  statusBar = new QStatusBar(this);
  statusBar->setSizeGripEnabled(FALSE);
  
  status1Label = new QLabel( tr("Idle"), statusBar);
  status2Label = new QLabel("", statusBar);
  connect(emailHandler, SIGNAL(updatePopStatus(const QString &)),
    status2Label, SLOT(setText(const QString &)) );
  connect(emailHandler, SIGNAL(updateSmtpStatus(const QString &)),
    status2Label, SLOT(setText(const QString &)) );
  
  progressBar = new QProgressBar(statusBar);
  connect(emailHandler, SIGNAL(mailboxSize(int)),
    this, SLOT(setTotalSize(int)) );
  connect(emailHandler, SIGNAL(currentMailSize(int)),
    this, SLOT(setMailSize(int)) );
  connect(emailHandler, SIGNAL(downloadedSize(int)),
    this, SLOT(setDownloadedSize(int)) );

  statusBar->addWidget(status1Label);
  statusBar->addWidget(progressBar);
  statusBar->addWidget(status2Label);
  
  setToolBarsMovable(FALSE);
  
  bar = new QToolBar(this);
  bar->setHorizontalStretchable( TRUE );

  mb = new QMenuBar( bar );

  QPopupMenu *mail = new QPopupMenu(mb);
  mb->insertItem( tr( "&Mail" ), mail);

  QPopupMenu *configure = new QPopupMenu(mb);
  mb->insertItem( tr( "Accounts" ), configure);
  
  selectAccountMenu = new QPopupMenu(mb);
  editAccountMenu = new QPopupMenu(mb);
  deleteAccountMenu = new QPopupMenu(mb);

  mail->insertItem(tr("Get Mail in"), selectAccountMenu);
  configure->insertItem(tr("Edit account"), editAccountMenu);
  configure->insertItem(tr("Delete account"), deleteAccountMenu);

  bar = new QToolBar(this);

  getMailButton = new QAction(tr("Get all mail"), QPixmap("getmail.xpm"), QString::null, 0, this, 0);
  connect(getMailButton, SIGNAL(activated()), this, SLOT(getAllNewMail()) );
  getMailButton->addTo(bar);
  getMailButton->addTo(mail);
  
  sendMailButton = new QAction(tr("Send mail"), QPixmap("sendmail.xpm"), QString::null, 0, this, 0);
  connect(sendMailButton, SIGNAL(activated()), this, SLOT(sendQuedMail()) );
  sendMailButton->addTo(bar);
  sendMailButton->addTo(mail);
    
  composeButton = new QAction(tr("Compose"), Resource::loadPixmap("new"), QString::null, 0, this, 0);
  connect(composeButton, SIGNAL(activated()), this, SLOT(compose()) );
  composeButton->addTo(bar);
  composeButton->addTo(mail);
  
  cancelButton = new QAction(tr("Cancel transfer"), Resource::loadPixmap("reset"), QString::null, 0, this, 0);
  connect(cancelButton, SIGNAL(activated()), this, SLOT(cancel()) );
  cancelButton->addTo(mail);
  cancelButton->setEnabled(FALSE);
  
  mailboxView = new QTabWidget( this, "mailboxView" );

  QWidget* widget = new QWidget( mailboxView, "widget" );
  grid_2 = new QGridLayout( widget );
//  grid_2->setSpacing(6);
//  grid_2->setMargin( 11 );

  inboxView = new QListView( widget, "inboxView" );
  inboxView->addColumn( tr( "From" ) );
  inboxView->addColumn( tr( "Subject" ) );
  inboxView->addColumn( tr( "Date" ) );
  inboxView->setMinimumSize( QSize( 0, 0 ) );
  inboxView->setAllColumnsShowFocus(TRUE);

  grid_2->addWidget( inboxView, 2, 0 );
  mailboxView->insertTab( widget, tr( "Inbox" ) );

  QWidget* widget_2 = new QWidget( mailboxView, "widget_2" );
  grid_3 = new QGridLayout( widget_2 );
//  grid_3->setSpacing(6);
//  grid_3->setMargin( 11 );

  outboxView = new QListView( widget_2, "outboxView" );
  outboxView->addColumn( tr( "To" ) );
  outboxView->addColumn( tr( "Subject" ) );
  outboxView->setAllColumnsShowFocus(TRUE);

  grid_3->addWidget( outboxView, 0, 0 );
  mailboxView->insertTab( widget_2, tr( "Outbox" ) );
  
  setCentralWidget(mailboxView);
}

void EmailClient::compose()
{
  emit composeRequested();
}

void EmailClient::cancel()
{
  emailHandler->cancel();
}

AddressList* EmailClient::getAdrListRef()
{
  return addressList;
}

//this needs to be rewritten to syncronize with outboxView
void EmailClient::enqueMail(const Email &mail)
{
  if (accountList.count() > 0) {
    currentAccount = accountList.first();
    qWarning("using account " + currentAccount->name);
  }
  
  Email addMail = mail;
  addMail.from = currentAccount->name;
  addMail.fromMail = currentAccount->emailAddress;
  addMail.rawMail.prepend("From: " + addMail.from + "<" + addMail.fromMail + ">\n");
  item = new EmailListItem(outboxView, addMail, false);
  
}

void EmailClient::sendQuedMail()
{
  int count = 0;

  if (accountList.count() == 0) {
    QMessageBox::warning(qApp->activeWindow(),
      "No account selected", "You must create an account", "OK\n");
    return;
  }
  //traverse listview, find messages to send
  if (! sending) {
    item = (EmailListItem *) outboxView->firstChild();
    if (item != NULL) {
      while (item != NULL) {
        quedMessages.append(item->getMail());
        item = (EmailListItem *) item->nextSibling();
        count++;
      }
      setMailAccount();
      emailHandler->sendMail(&quedMessages);
      sending = TRUE;
      sendMailButton->setEnabled(FALSE);
      cancelButton->setEnabled(TRUE);
    } else {
      qWarning("sendQuedMail(): no messages to send");
    }
  }
}

void EmailClient::setMailAccount()
{
  emailHandler->setAccount(*currentAccount);
}

void EmailClient::mailSent()
{
  sending = FALSE;
  sendMailButton->setEnabled(TRUE);
  
  quedMessages.clear();
  outboxView->clear();    //should be moved to an sentBox
}

void EmailClient::getNewMail() {
  
  if (accountList.count() == 0) {
    QMessageBox::warning(qApp->activeWindow(),"No account selected",
      "You must create an account", "OK\n");
    return;
  }
  
  setMailAccount();
  
  receiving = TRUE;
  previewingMail = TRUE;
  getMailButton->setEnabled(FALSE);
  cancelButton->setEnabled(TRUE);
  selectAccountMenu->setEnabled(FALSE);
  
  status1Label->setText(currentAccount->accountName + " headers");
  progressBar->reset();
    
  //get any previous mails not downloaded and add to queue
  mailDownloadList.clear();
  Email *mailPtr;
  item = (EmailListItem *) inboxView->firstChild();
  while (item != NULL) {
    mailPtr = item->getMail();
    if ( (!mailPtr->downloaded) && (mailPtr->fromAccountId == currentAccount->id) ) {
      mailDownloadList.sizeInsert(mailPtr->serverId, mailPtr->size);
    }
    item = (EmailListItem *) item->nextSibling();
  }
  
  emailHandler->getMailHeaders();
}

void EmailClient::getAllNewMail()
{
  allAccounts = TRUE;
  currentAccount = accountList.first();
  getNewMail();
}

void EmailClient::mailArrived(const Email &mail, bool fromDisk)
{
  Enclosure *ePtr;
  Email newMail;
  int thisMailId;
  emailHandler->parse(mail.rawMail, lineShift, &newMail);
  
  mailconf->setGroup(newMail.id);
  
  if (fromDisk) {
    newMail.downloaded = mailconf->readBoolEntry("downloaded");
    newMail.size = mailconf->readNumEntry("size");
    newMail.serverId = mailconf->readNumEntry("serverid");
    newMail.fromAccountId = mailconf->readNumEntry("fromaccountid");
  } else {  //mail arrived from server
    newMail.serverId = mail.serverId;
    newMail.size = mail.size;
    newMail.downloaded = mail.downloaded;
    
    newMail.fromAccountId = currentAccount->id;
    mailconf->writeEntry("fromaccountid", newMail.fromAccountId);
  }
  
  //add if read or not
  newMail.read = mailconf->readBoolEntry("mailread");
    
  //check if new mail
  if ( (thisMailId = mailconf->readNumEntry("internalmailid", -1)) == -1) {
    thisMailId = mailIdCount;
    mailIdCount++;
    
    //set server count, so that if the user aborts, the new
    //header is not reloaded
    if (currentAccount->synchronize)
      currentAccount->lastServerMailCount++;
    
    mailconf->writeEntry("internalmailid", thisMailId);
    mailconf->writeEntry("downloaded", newMail.downloaded);
    mailconf->writeEntry("size", (int) newMail.size);
    mailconf->writeEntry("serverid", newMail.serverId);
    
    addressList->addContact(newMail.fromMail, newMail.from);
  } else if (!fromDisk) {   //body to header arrived
    mailconf->writeEntry("downloaded", TRUE);
  }
  QString stringMailId;
  stringMailId.setNum(thisMailId);
  //se if any attatchments needs to be stored
  for ( ePtr=newMail.files.first(); ePtr != 0; ePtr=newMail.files.next() ) {
    QString stringId;
    stringId.setNum(ePtr->id);
    
    int id = mailconf->readNumEntry("enclosureid_" + stringId);
    if (id != ePtr->id) {     //new entry
      mailconf->writeEntry("enclosureid_" + stringId, ePtr->id);
      mailconf->writeEntry("name_" + stringId, ePtr->originalName);
      mailconf->writeEntry("contenttype_" + stringId, ePtr->contentType);
      mailconf->writeEntry("contentattribute_" + stringId, ePtr->contentAttribute);
      mailconf->writeEntry("saved_" + stringId, ePtr->saved);
      mailconf->writeEntry("installed_" + stringId, FALSE);
      
            ePtr->name = stringMailId + "_" + stringId;
            ePtr->path = getPath(TRUE);
            if (emailHandler->getEnclosure(ePtr)) { //file saved
        ePtr->saved = TRUE;
        mailconf->writeEntry("saved_" + stringId, ePtr->saved);
        mailconf->writeEntry("filename_" + stringId, ePtr->name);
        mailconf->writeEntry("path_" + stringId, ePtr->path);
            } else {
              ePtr->saved = FALSE;
        mailconf->writeEntry("saved_" + stringId, ePtr->saved);
            }
    } else {
      ePtr->saved = mailconf->readBoolEntry("saved_" + stringId);
      ePtr->installed = mailconf->readBoolEntry("installed_" + stringId);
      if (ePtr->saved) {
        ePtr->name = mailconf->readEntry("filename_" + stringId);
        ePtr->path = mailconf->readEntry("path_" + stringId);
      }   
    }
  }
  if (!previewingMail && !fromDisk) {
    Email *mailPtr;
    item = (EmailListItem *) inboxView->firstChild();
    while (item != NULL) {
      mailPtr = item->getMail();
      if (mailPtr->id == newMail.id) {
        item->setMail(newMail);
        emit mailUpdated(item->getMail());
      }
      item = (EmailListItem *) item->nextSibling();
    }
  } else {
    item = new EmailListItem(inboxView, newMail, TRUE);
    if (!newMail.downloaded)
      mailDownloadList.sizeInsert(newMail.serverId, newMail.size);
  }
  
}

void EmailClient::allMailArrived(int count)
{
  // not previewing means all mailtransfer has been done
  if (!previewingMail) {
    if ( (allAccounts) && ( (currentAccount = accountList.next()) !=0 ) ) {
      emit newCaption("Mailit - " + currentAccount->accountName);
      getNewMail();
      return;
    } else {
      allAccounts = FALSE;
      receiving = FALSE;
      getMailButton->setEnabled(TRUE);
      cancelButton->setEnabled(FALSE);
      selectAccountMenu->setEnabled(TRUE);
      status1Label->setText("Idle");
  
      progressBar->reset();
      return;
    }
  }
  
  // all headers downloaded from server, start downloading remaining mails
  previewingMail = FALSE;
  status1Label->setText(currentAccount->accountName);
  progressBar->reset();

  emailHandler->getMailByList(&mailDownloadList);
}

void EmailClient::moveMailFront(Email *mailPtr)
{
  if ( (receiving) && (mailPtr->fromAccountId == currentAccount->id) ) {
    mailDownloadList.moveFront(mailPtr->serverId, mailPtr->size);
  }
}

void EmailClient::smtpError(int code)
{
  QString temp;
  
  if (code == ErrUnknownResponse)
    temp = "Unknown response from server";
  
  if (code == QSocket::ErrHostNotFound)
    temp = "host not found";
  if (code == QSocket::ErrConnectionRefused)
    temp = "connection refused";
  if (code == QSocket::ErrSocketRead)
    temp = "socket packet error";
  
  if (code != ErrCancel) {
    QMessageBox::warning(qApp->activeWindow(), "Sending error", temp, "OK\n");
  } else {
    status2Label->setText("Aborted by user");
  }
  
  sending = FALSE;
  sendMailButton->setEnabled(TRUE);
  cancelButton->setEnabled(FALSE);
  quedMessages.clear();
}

void EmailClient::popError(int code)
{
  QString temp;
  
  if (code == ErrUnknownResponse)
    temp = "Unknown response from server";
  if (code == ErrLoginFailed)
    temp = "Login failed\nCheck user name and password";
  
  if (code == QSocket::ErrHostNotFound)
    temp = "host not found";
  if (code == QSocket::ErrConnectionRefused)
    temp = "connection refused";
  if (code == QSocket::ErrSocketRead)
    temp = "socket packet error";
    
  if (code != ErrCancel) {
    QMessageBox::warning(qApp->activeWindow(), "Receiving error", temp, "OK\n");
  } else {
    status2Label->setText("Aborted by user");
  }
  
  receiving = FALSE;
  getMailButton->setEnabled(TRUE);
  cancelButton->setEnabled(FALSE);
  selectAccountMenu->setEnabled(TRUE);
}

void EmailClient::inboxItemSelected()
{
  item = (EmailListItem*) inboxView->selectedItem();
  if (item != NULL) {
    emit viewEmail(inboxView, item->getMail());
  }
}

void EmailClient::outboxItemSelected()
{
  item = (EmailListItem*) outboxView->selectedItem();
  if (item != NULL) {
    emit viewEmail(outboxView, item->getMail());
  }

}

void EmailClient::readMail()
{
  Email mail;
  int start, stop;
  QString s, del;

  QFile f(getPath(FALSE) + "inbox.txt");
//  QFileInfo fi(f);
  qDebug( f.name());

  if ( f.open(IO_ReadOnly) ) {    // file opened successfully
    QTextStream t( &f );        // use a text stream
    s = t.read();
    f.close();
    
    start = 0;
    del = "\n.\n";
    while ((uint) start < s.length()) {
      stop = s.find(del, start);
      if (stop == -1)
        stop = s.length() - del.length();
    
      mail.rawMail = s.mid(start, stop + del.length() - start );
      start = stop + del.length();
      mailArrived(mail, TRUE);
    }
  }
  
  QFile fo(getPath(FALSE) + "outbox.txt");
  if ( fo.open(IO_ReadOnly) ) {    // file opened successfully
    QTextStream t( &fo );        // use a text stream
    s = t.read();
    fo.close();
    
    start = 0;
    del = "\n.\n";
    while ((uint) start < s.length()) {
      stop = s.find(del, start);
      if (stop == -1)
        stop = s.length() - del.length();
    
      mail.rawMail = s.mid(start, stop + del.length() - start );
      start = stop + del.length();
      emailHandler->parse(mail.rawMail, lineShift, &mail);
      mail.sent = false;
      mail.received = false;
          enqueMail(mail);
      
    }
  }
}

void EmailClient::saveMail(QString fileName, QListView *view)
{
  QFile f(fileName);
  Email *mail;
  
  if (! f.open(IO_WriteOnly) ) {
    qWarning("could not open file");
    return;
  }
  item = (EmailListItem *) view->firstChild();
  qDebug (QString("Write : ") );
  QTextStream t(&f);
  while (item != NULL) {
    mail = item->getMail();
    qDebug(mail->rawMail);
    qDebug(mail->recipients.first());
    t << mail->rawMail;
    
    mailconf->setGroup(mail->id);
    mailconf->writeEntry("mailread", mail->read);
    
    item = (EmailListItem *) item->nextSibling();
  }
  f.close();
}

//paths for mailit, is settings, inbox, enclosures
QString EmailClient::getPath(bool enclosurePath)
{
  QString basePath = "qtmail";
  QString enclosures = "enclosures";
  
  QDir dir = (QString(getenv("HOME")) + "/Applications/" + basePath);
  if ( !dir.exists() )
    dir.mkdir( dir.path() );
  
  if (enclosurePath) {
    dir = (QString(getenv("HOME")) + "/Applications/" + basePath + "/" + enclosures);
    
    if ( !dir.exists() )
      dir.mkdir( dir.path() );
    
    return (dir.path() + "/");
  
  }
  return (dir.path() + "/");
}

void EmailClient::readSettings()
{
  TextParser *p;
  QString s;
  int pos, accountPos, y;
  QFile f( getPath(FALSE) + "settings.txt");
  
  if ( f.open(IO_ReadOnly) ) {    // file opened successfully
    QTextStream t( &f );        // use a text stream
    s = t.read();
    f.close();
    
    p = new TextParser(s, "\n");
    
    accountPos = 0;
    while ( (accountPos = p->find("ACCOUNTSTART",';', accountPos, TRUE)) != -1 ) {
      accountPos++;
      if ( (pos = p->find("ACCOUNTNAME",':', accountPos, TRUE)) != -1 )
        account.accountName = p->getString(& ++pos, 'z', TRUE);
      if ( (pos = p->find("NAME",':', accountPos, TRUE)) != -1)
        account.name = p->getString(& ++pos, 'z', TRUE);
      if ( (pos = p->find("EMAIL",':', accountPos, TRUE)) != -1)
        account.emailAddress = p->getString(& ++pos, 'z', TRUE);
      if ( (pos = p->find("POPUSER",':', accountPos, TRUE)) != -1)
        account.popUserName = p->getString(& ++pos, 'z', TRUE);
      if ( (pos = p->find("POPPASSWORD",':', accountPos, TRUE)) != -1)
        account.popPasswd = p->getString(& ++pos, 'z', TRUE);
      if ( (pos = p->find("POPSERVER",':', accountPos, TRUE)) != -1)
        account.popServer = p->getString(& ++pos, 'z', TRUE);
      if ( (pos = p->find("SMTPSERVER",':', accountPos, TRUE)) != -1)
        account.smtpServer = p->getString(& ++pos, 'z', TRUE);
      if ( (pos = p->find("ACCOUNTID",':', accountPos, TRUE)) != -1) {
        s = p->getString(& ++pos, 'z', TRUE);
        account.id = s.toInt();
      }
    
      account.lastServerMailCount = 0;
      account.synchronize = FALSE;
      if ( (pos = p->find("SYNCHRONIZE",':', accountPos, TRUE)) != -1) {
        if (p->getString(& ++pos, 'z', TRUE).upper() == "YES") {
          account.synchronize = TRUE;
          if ( (pos = p->find("LASTSERVERMAILCOUNT",':', accountPos, TRUE)) != -1) {
            s = p->getString(& ++pos, 'z', TRUE);
            account.lastServerMailCount = s.toInt();
          }
        }
      }
      accountList.append(&account);
    }
    delete p;
  }
  mailconf->setGroup("mailitglobal");
  if ( (y = mailconf->readNumEntry("mailidcount", -1)) != -1) {
    mailIdCount = y;
  }
  if ( (y = mailconf->readNumEntry("accountidcount", -1)) != -1) {
    accountIdCount = y;
  }
}

void EmailClient::saveSettings()
{
  QString temp;
  QFile f( getPath(FALSE) + "settings.txt");
  MailAccount *accountPtr;
  
  if (! f.open(IO_WriteOnly) ) {
    qWarning("could not save settings file");
    return;
  }
  QTextStream t(&f);
  t << "#Settings for QPE Mailit program\n";
  
  for (accountPtr = accountList.first(); accountPtr != 0;
      accountPtr = accountList.next()) {

    t << "accountStart;\n";
    t << "AccountName: " + accountPtr->accountName + "\n";
    t << "Name: " + accountPtr->name + "\n";
    t << "Email: " + accountPtr->emailAddress + "\n";
    t << "POPUser: " + accountPtr->popUserName + "\n";
    t << "POPPAssword: " + accountPtr->popPasswd + "\n";
    t << "POPServer: " + accountPtr->popServer + "\n";
    t << "SMTPServer: " + accountPtr->smtpServer + "\n";
    t << "AccountId: " << accountPtr->id << "\n";
    if (accountPtr->synchronize) {
      t << "Synchronize: Yes\n";
      t << "LastServerMailCount: ";
      t << accountPtr->lastServerMailCount << "\n";
    } else {
      t << "Synchronize: No\n";
    }
    t << "accountEnd;\n";
  }
  f.close();
  
  mailconf->setGroup("mailitglobal");
  mailconf->writeEntry("mailidcount", mailIdCount);
  mailconf->writeEntry("accountidcount", accountIdCount);
}

void EmailClient::selectAccount(int id)
{
  if (accountList.count() > 0) {
    currentAccount = accountList.at(id);
    emit newCaption("Mailit - " + currentAccount->accountName);
    getNewMail();
  } else {
    emit newCaption("Mailit ! No account defined");
  }
}

void EmailClient::editAccount(int id)
{
  MailAccount *newAccount;
    
  editAccountView = new EditAccount(this, "account", TRUE);
  if (id == newAccountId) {   //new account
    newAccount = new MailAccount;
    editAccountView->setAccount(newAccount);
  } else {
    newAccount = accountList.at(id);
    editAccountView->setAccount(newAccount, FALSE);
  }
  
  editAccountView->showMaximized();
  editAccountView->exec();
  
  if (editAccountView->result() == QDialog::Accepted) {
    if (id == newAccountId) {
      newAccount->id = accountIdCount;
      accountIdCount++;
      accountList.append(newAccount);
      updateAccounts();
    } else {
      updateAccounts();
    }
  }
  
  delete editAccountView;
}

void EmailClient::deleteAccount(int id)
{
  MailAccount *newAccount;
  QString message;
  
  newAccount = accountList.at(id);
  message = "Delete account:\n" + newAccount->accountName;
  switch( QMessageBox::warning( this, "Mailit", message,
      "Yes", "No", 0, 0, 1 ) ) {
    
    case 0: accountList.remove(id);
        updateAccounts();
        break;
    case 1:
        break;
  }
}

void EmailClient::updateAccounts()
{
  MailAccount *accountPtr;
  
  //rebuild menus, clear all first
  editAccountMenu->clear();
  selectAccountMenu->clear();
  deleteAccountMenu->clear();

  newAccountId = editAccountMenu->insertItem("New", this,
      SLOT(editAccount(int)) );
  editAccountMenu->insertSeparator();
  
  idCount = 0;
  for (accountPtr = accountList.first(); accountPtr != 0;
      accountPtr = accountList.next()) {
    
    editAccountMenu->insertItem(accountPtr->accountName,
      this, SLOT(editAccount(int)), 0, idCount);
    selectAccountMenu->insertItem(accountPtr->accountName,
      this, SLOT(selectAccount(int)), 0, idCount);
    deleteAccountMenu->insertItem(accountPtr->accountName,
      this, SLOT(deleteAccount(int)), 0, idCount);
    idCount++;
  }
}

void EmailClient::deleteMail(EmailListItem *mailItem, bool &inbox)
{
  Email *mPtr;
  Enclosure *ePtr;
  
  if (inbox) {
    mPtr = mailItem->getMail();
    
    //if mail is in queue for download, remove it from
    //queue if possible
    if ( (receiving) && (mPtr->fromAccountId == currentAccount->id) ) {
      if ( !mPtr->downloaded )
        mailDownloadList.remove(mPtr->serverId, mPtr->size);
    }
    
    mailconf->setGroup(mPtr->id);
    mailconf->clearGroup();
    
    //delete any temporary attatchemnts storing
    for ( ePtr=mPtr->files.first(); ePtr != 0; ePtr=mPtr->files.next() ) {
      if (ePtr->saved) {
        QFile::remove( (ePtr->path + ePtr->name) );
      }
    }
    inboxView->takeItem(mailItem);
  } else {
    outboxView->takeItem(mailItem);
  }
}

void EmailClient::setMailSize(int size)
{
        progressBar->reset();
        progressBar->setTotalSteps(size);
}

void EmailClient::setTotalSize(int size)
{
  
}

void EmailClient::setDownloadedSize(int size)
{
        int total = progressBar->totalSteps();

        if (size < total) {
    progressBar->setProgress(size);
        } else {
          progressBar->setProgress(total);
        }
}
