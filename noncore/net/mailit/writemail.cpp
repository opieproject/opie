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
#include <qmessagebox.h>
#include <qwhatsthis.h>
#include "writemail.h"
#include <qpe/resource.h>

WriteMail::WriteMail( QWidget* parent,  const char* name, WFlags fl ):QMainWindow( parent, name, fl )
{
  showingAddressList = FALSE;
  init();
  
  addAtt = new AddAtt(0, "Add Attachments");
}

WriteMail::~WriteMail()
{
  delete addAtt;
}

void WriteMail::setAddressList(AddressList *list)
{
  AContact *cPtr;
  
  addressList = list;
  
  addressView->clear();
  QList<AContact> *cListPtr = addressList->getContactList();
  QListViewItem *item;
  for (cPtr = cListPtr->first(); cPtr != 0; cPtr = cListPtr->next() ) {
    item = new QListViewItem(addressView, cPtr->name, cPtr->email);
  }
}

void WriteMail::init()
{
  setToolBarsMovable(FALSE);
  
  bar = new QToolBar(this);
  bar->setHorizontalStretchable( TRUE );

  menu = new QMenuBar( bar );

  mailMenu = new QPopupMenu(menu);
  menu->insertItem( tr( "&Mail" ), mailMenu);
  addMenu = new QPopupMenu(menu);
  menu->insertItem( tr( "&Add" ), addMenu);
  
  bar = new QToolBar(this);
  attachButton = new QAction(tr("Attachment"), Resource::loadPixmap("mailit/attach"), QString::null, 0, this, 0);
  attachButton->addTo(bar);
  attachButton->addTo(addMenu);
  connect( attachButton, SIGNAL( activated() ), this, SLOT( attachFile() ) );
  attachButton->setWhatsThis(tr("Click here to attach files to your mail"));
    
  confirmButton = new QAction(tr("Enque mail"), Resource::loadPixmap("OKButton"), QString::null, 0, this, 0);
  confirmButton->addTo(bar);
  confirmButton->addTo(mailMenu);
  connect( confirmButton, SIGNAL( activated() ), this, SLOT( accept() ) );
  confirmButton->setWhatsThis(tr("This button puts your mail in the send queue"));
  
  newButton = new QAction(tr("New mail"), Resource::loadPixmap("new"), QString::null, 0, this, 0);
  newButton->addTo(mailMenu);
  connect( newButton, SIGNAL( activated() ), this, SLOT( newMail() ) );
  newButton->setWhatsThis(tr("Click here to create a new mail"));

  widget = new QWidget(this, "widget");
  grid = new QGridLayout( widget );

  recipientsBox = new QComboBox( FALSE, widget, "toLabel" );
  recipientsBox->insertItem( tr( "To:"  ) );
  recipientsBox->insertItem( tr( "CC:"  ) );  
  recipientsBox->setCurrentItem(0);
  grid->addWidget( recipientsBox, 0, 0 );
  connect(recipientsBox,SIGNAL(activated(int)),this, SLOT(changeRecipients(int)));
  
  subjetLabel = new QLabel( widget, "subjetLabel" );
  subjetLabel->setText( tr( "Subject:"  ) );

  grid->addWidget( subjetLabel, 1, 0 );

  ToolButton13_2 = new QToolButton( widget, "ToolButton13_2" );
  ToolButton13_2->setText( tr( "..."  ) );
  grid->addWidget( ToolButton13_2, 1, 2 );

  subjectInput = new QLineEdit( widget, "subjectInput" );
  grid->addWidget( subjectInput, 1, 1 );
  QWhatsThis::add(subjectInput,QWidget::tr("The mail subject should be entered here"));

  toInput = new QLineEdit( widget, "toInput" );
  grid->addWidget( toInput, 0, 1 );
  QWhatsThis::add(recipientsBox,QWidget::tr("Recipients can be entered here"));

  ccInput = new QLineEdit( widget, "ccInput" );
  ccInput->hide();
  grid->addWidget( ccInput, 0, 1 );
  QWhatsThis::add(ccInput,QWidget::tr("If you would like to send copies of your mail they can be entered here"));

  addressButton = new QToolButton( widget, "addressButton" );
  addressButton->setPixmap( Resource::loadPixmap("AddressBook") );
  addressButton->setToggleButton(TRUE);
  grid->addWidget( addressButton, 0, 2 );
  connect(addressButton, SIGNAL(clicked()), this, SLOT(getAddress()) );
  QWhatsThis::add(addressButton,QWidget::tr("This button opens the address selector with all mail adresses from your Opie addressbook"));

  emailInput = new QMultiLineEdit( widget, "emailInput" );
  grid->addMultiCellWidget( emailInput, 2, 2, 0, 2);
  QWhatsThis::add(emailInput,QWidget::tr("Enter your mail text here"));

  addressView = new QListView( widget, "addressView");
  addressView->addColumn(tr("Name"));
  addressView->addColumn(tr("EMail") );
  addressView->setAllColumnsShowFocus(TRUE);
  addressView->setMultiSelection(TRUE);
  addressView->hide();
  grid->addMultiCellWidget( addressView, 3, 3, 0, 2);
  QWhatsThis::add(recipientsBox,QWidget::tr("Choose the recipients from this list"));

  okButton = new QToolButton(bar, "ok");
  okButton->setPixmap( Resource::loadPixmap("enter") );
  okButton->hide();
  connect(okButton, SIGNAL(clicked()), this, SLOT(addRecipients()) );
  QWhatsThis::add(okButton,QWidget::tr("Queue your mail by clicking here"));
  
  setCentralWidget(widget);
}

void WriteMail::reject()
{
  emit cancelMail();
}

void WriteMail::accept()
{
  QStringList attachedFiles, attachmentsType;
  int idCount = 0;
  
  if (toInput->text() == "") 
  {
    QMessageBox::warning(this,tr("No recipient"), tr("Send mail to whom?"), tr("OK\n"));
    return;
  }
  
  if (! getRecipients(false) ) 
  {
    QMessageBox::warning(this,tr("Incorrect recipient separator"),
        tr("Recipients must be separated by ;\nand be valid emailaddresses"), tr("OK\n"));
    return;
  }
  
  if ((ccInput->text()!="") && (! getRecipients(true) )) 
  {
    QMessageBox::warning(this,tr("Incorrect carbon copy separator"),
        tr("CC Recipients must be separated by ;\nand be valid emailaddresses"), tr("OK\n"));
    return;
  }
  
  mail.subject = subjectInput->text();
  mail.body = emailInput->text();
  mail.sent = false;
  mail.received = false;
  
  mail.rawMail = "To: ";
  
  for (QStringList::Iterator it = mail.recipients.begin();
    it != mail.recipients.end(); ++it) {
    
    mail.rawMail += (*it);
    mail.rawMail += ",\n";
  }
  
  mail.rawMail.truncate(mail.rawMail.length()-2);
    
  mail.rawMail += "\nCC: ";
  
  for (QStringList::Iterator it = mail.carbonCopies.begin();
    it != mail.carbonCopies.end(); ++it) {
    
    mail.rawMail += (*it);
    mail.rawMail += ",\n";
  }

  mail.rawMail += mail.from;
  mail.rawMail += "\nSubject: ";
  mail.rawMail += mail.subject;
  mail.rawMail += "\n\n";
  
  attachedFiles = addAtt->returnattachedFiles();
  attachmentsType = addAtt->returnFileTypes();

  QStringList::Iterator itType = attachmentsType.begin();
  
  Enclosure e;
  for ( QStringList::Iterator it = attachedFiles.begin(); it != attachedFiles.end(); ++it ) {
    e.id = idCount;
    e.originalName = (*it).latin1();
    e.contentType = (*itType).latin1();
    e.contentAttribute = (*itType).latin1();
    e.saved = TRUE;
    mail.addEnclosure(&e);
    
    itType++;
    idCount++;
  }
  mail.rawMail += mail.body;
  mail.rawMail += "\n";
  mail.rawMail += ".\n";
  emit sendMailRequested(mail);
  addAtt->clear();
}

void WriteMail::getAddress()
{
  showingAddressList = !showingAddressList;
  
  if (showingAddressList) {
    emailInput->hide();
    addressView->show();
    okButton->show();
    
  } else {
    addressView->hide();
    okButton->hide();
    emailInput->show();
  }
}

void WriteMail::attachFile()
{
  addAtt->showMaximized();
}

void WriteMail::reply(Email replyMail, bool replyAll)
{
  int pos;
  QString ccRecipients;
  
  mail = replyMail;
  mail.files.clear();
  
  toInput->setText(mail.fromMail);
  
  if (replyAll)
  { 
    for (QStringList::Iterator it = mail.carbonCopies.begin();it != mail.carbonCopies.end(); ++it) 
    {
      ccRecipients.append(*it);
      ccRecipients.append(";");
    }
    ccRecipients.truncate(ccRecipients.length()-1);   //no ; at the end
    ccInput->setText(ccRecipients);
  }
  else ccInput->clear();
  
  subjectInput->setText(tr("Re: ") + mail.subject);
  
  QString citation=mail.fromMail;
  citation.append(tr(" wrote on "));
  citation.append(mail.date);
  citation.append(":\n");
  

  //mail.body.insert(0,tr("On"));
  pos = 0;
  mail.body.insert(pos, ">");
  while (pos != -1) {
    pos = mail.body.find('\n', pos);
    if (pos != -1)
      mail.body.insert(++pos, ">");
  }
  mail.body.insert(0,citation); 
  emailInput->setText(mail.body);
}

void WriteMail::forward(Email forwMail)
{
   // int pos=0;
  
  QString fwdBody=tr("======forwarded message from ");
  fwdBody.append(forwMail.fromMail);
  fwdBody.append(tr(" starts======\n\n"));
  
  mail=forwMail;
  toInput->setText("");
  ccInput->setText("");
  subjectInput->setText(tr("FWD: ") + mail.subject);
  
  fwdBody+=mail.body;
  fwdBody+=QString(tr("======end of forwarded message======\n\n"));
  
  emailInput->setText(fwdBody);
}

bool WriteMail::getRecipients(bool ccField)
{
  QString str, temp;
  int pos = 0;
  
  if (ccField)
  {
    mail.carbonCopies.clear();
    temp = ccInput->text();
  } 
  else 
  {
    mail.recipients.clear(); 
    temp=toInput->text() ;
  }
  
  while ( (pos = temp.find(';')) != -1) {
    str = temp.left(pos).stripWhiteSpace();
    temp = temp.right(temp.length() - (pos + 1));
    if ( str.find('@') == -1)
      return false;
    ccField ? mail.carbonCopies.append(str) : mail.recipients.append(str);
    //addressList->addContact(str, "");
  }
  temp = temp.stripWhiteSpace();
  if ( temp.find('@') == -1)
    return false;
  ccField ? mail.carbonCopies.append(temp) : mail.recipients.append(temp);
  //addressList->addContact(temp, "");
  
  return TRUE;
}

void WriteMail::addRecipients()
{
  toInput->isVisible() ? addRecipients(false) : addRecipients(true);
}     
       
void WriteMail::addRecipients(bool ccField)
{
  QString recipients = "";
  
  mail.recipients.clear();
  
  QListViewItem *item = addressView->firstChild();
  while (item != NULL) {
    if ( item->isSelected() ) {
      if (recipients == "") {
        recipients = item->text(1);
      } else {
        recipients += "; " + item->text(1);
      }
    }
    item = item->nextSibling();
  }
  
  ccField ? ccInput->setText(recipients):toInput->setText(recipients);
  
  addressView->hide();
  okButton->hide();
  emailInput->show();
  addressButton->setOn(FALSE);
  showingAddressList = !showingAddressList;
}

void WriteMail::changeRecipients(int selection)
{
  if (selection==0)
  {
    toInput->show();
    ccInput->hide();
  }
  else if (selection==1)
  {
    toInput->hide();
    ccInput->show();
  }
}

void WriteMail::setRecipient(const QString &recipient)
{
    toInput->setText(recipient);
}

void WriteMail::newMail()
{
  toInput->clear();
  ccInput->clear();
  subjectInput->clear();
  emailInput->clear();
  setAddressList(addressList);
}
