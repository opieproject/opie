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
#include "writemail.h"
#include "resource.h"

WriteMail::WriteMail( QWidget* parent,  const char* name, WFlags fl )
	: QMainWindow( parent, name, fl )
{
	showingAddressList = FALSE;
	init();
	
	addAtt = new AddAtt(0, "Add Attatchments");
}

WriteMail::~WriteMail()
{
	delete addAtt;
}

void WriteMail::setAddressList(AddressList *list)
{
	Contact *cPtr;
	
	addressList = list;
	
	addressView->clear();
	QList<Contact> *cListPtr = addressList->getContactList();
	QListViewItem *item;
	for (cPtr = cListPtr->first(); cPtr != 0; cPtr = cListPtr->next() ) {
		item = new QListViewItem(addressView, cPtr->email, cPtr->name);
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
	attatchButton = new QAction(tr("Attatchment"), Resource::loadPixmap("mailit/attach"), QString::null, 0, this, 0);
	attatchButton->addTo(bar);
	attatchButton->addTo(addMenu);
	connect( attatchButton, SIGNAL( activated() ), this, SLOT( attatchFile() ) );
	
	confirmButton = new QAction(tr("Enque mail"), Resource::loadPixmap("OKButton"), QString::null, 0, this, 0);
	confirmButton->addTo(bar);
	confirmButton->addTo(mailMenu);
	connect( confirmButton, SIGNAL( activated() ), this, SLOT( accept() ) );
	
	newButton = new QAction(tr("New mail"), Resource::loadPixmap("new"), QString::null, 0, this, 0);
	newButton->addTo(mailMenu);
	connect( newButton, SIGNAL( activated() ), this, SLOT( newMail() ) );
	
	widget = new QWidget(this, "widget");
	grid = new QGridLayout( widget );

	recipientsBox = new QComboBox( FALSE, widget, "toLabel" );
	recipientsBox->insertItem( tr( "To:"  ) );
	recipientsBox->insertItem( tr( "CC:"  ) );
	recipientsBox->setCurrentItem(0);
	grid->addWidget( recipientsBox, 0, 0 );
	
	subjetLabel = new QLabel( widget, "subjetLabel" );
	subjetLabel->setText( tr( "Subject:"  ) );

	grid->addWidget( subjetLabel, 1, 0 );

	ToolButton13_2 = new QToolButton( widget, "ToolButton13_2" );
	ToolButton13_2->setText( tr( "..."  ) );
	grid->addWidget( ToolButton13_2, 1, 2 );

	subjectInput = new QLineEdit( widget, "subjectInput" );
	grid->addWidget( subjectInput, 1, 1 );

	toInput = new QLineEdit( widget, "toInput" );
	grid->addWidget( toInput, 0, 1 );

	addressButton = new QToolButton( widget, "addressButton" );
	addressButton->setPixmap( Resource::loadPixmap("AddressBook") );
	addressButton->setToggleButton(TRUE);
	grid->addWidget( addressButton, 0, 2 );
	connect(addressButton, SIGNAL(clicked()), this, SLOT(getAddress()) );

	emailInput = new QMultiLineEdit( widget, "emailInput" );
	grid->addMultiCellWidget( emailInput, 2, 2, 0, 2);
	
	addressView = new QListView( widget, "addressView");
	addressView->addColumn("Email");
	addressView->addColumn("Name");
	addressView->setAllColumnsShowFocus(TRUE);
	addressView->setMultiSelection(TRUE);
	addressView->hide();
	grid->addMultiCellWidget( addressView, 3, 3, 0, 2);
	
	okButton = new QToolButton(bar, "ok");
	okButton->setPixmap( Resource::loadPixmap("enter") );
	okButton->hide();
	connect(okButton, SIGNAL(clicked()), this, SLOT(addRecipients()) );
	
	setCentralWidget(widget);
}

void WriteMail::reject()
{
	emit cancelMail();
}

// need to insert date
void WriteMail::accept()
{
	QStringList attatchedFiles, attatchmentsType;
	int idCount = 0;
	
	if (toInput->text() == "") {
	 	QMessageBox::warning(this,"No recipient", "Send mail to whom?", "OK\n");
		return;
	}
	if (! getRecipients() ) {
	 	QMessageBox::warning(this,"Incorrect recipient separator",
	 			"Recipients must be separated by ;\nand be valid emailaddresses", "OK\n");
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
	mail.rawMail += mail.from;
	mail.rawMail += "\nSubject: ";
	mail.rawMail += mail.subject;
	mail.rawMail += "\n\n";
	
	attatchedFiles = addAtt->returnAttatchedFiles();
	attatchmentsType = addAtt->returnFileTypes();

	QStringList::Iterator itType = attatchmentsType.begin();
	
	Enclosure e;
	for ( QStringList::Iterator it = attatchedFiles.begin(); it != attatchedFiles.end(); ++it ) {
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

void WriteMail::attatchFile()
{
	addAtt->showMaximized();
}

void WriteMail::reply(Email replyMail)
{
	int pos;
	
	mail = replyMail;
	mail.files.clear();
	
	toInput->setText(mail.fromMail);
	subjectInput->setText("Re: " + mail.subject);
	
	pos = 0;
	mail.body.insert(pos, ">>");
	while (pos != -1) {
		pos = mail.body.find('\n', pos);
		if (pos != -1)
			mail.body.insert(++pos, ">>");
	}
	
	emailInput->setText(mail.body);
}

bool WriteMail::getRecipients()
{
	QString str, temp;
	int pos = 0;
	
	mail.recipients.clear();
	
	temp = toInput->text();
	while ( (pos = temp.find(';')) != -1) {
		str = temp.left(pos).stripWhiteSpace();
		temp = temp.right(temp.length() - (pos + 1));
		if ( str.find('@') == -1)
			return false;
		mail.recipients.append(str);
		addressList->addContact(str, "");
	}
	temp = temp.stripWhiteSpace();
	if ( temp.find('@') == -1)
		return false;
	mail.recipients.append(temp);
	addressList->addContact(temp, "");
	
	return TRUE;
}
			

void WriteMail::addRecipients()
{
	QString recipients = "";
	
	mail.recipients.clear();
	QListViewItem *item = addressView->firstChild();
	while (item != NULL) {
		if ( item->isSelected() ) {
			if (recipients == "") {
				recipients = item->text(0);
			} else {
				recipients += "; " + item->text(0);
			}
		}
		item = item->nextSibling();
	}
	toInput->setText(recipients);
	
	addressView->hide();
	okButton->hide();
	emailInput->show();
	addressButton->setOn(FALSE);
	showingAddressList = !showingAddressList;
}

void WriteMail::setRecipient(const QString &recipient)
{
    toInput->setText(recipient);
}

void WriteMail::newMail()
{
	toInput->clear();
	subjectInput->clear();
	emailInput->clear();
	//to clear selected
	setAddressList(addressList);
}
