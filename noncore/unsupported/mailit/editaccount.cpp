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
***
**********************************************************************/

#include "editaccount.h"

EditAccount::EditAccount( QWidget* parent, const char* name, WFlags fl )
	: QDialog(parent, name, fl)
{
	setCaption( tr("Edit Account") );
	init();
    popPasswInput->setEchoMode(QLineEdit::Password);
}

void EditAccount::setAccount(MailAccount *in, bool newOne)
{
	account = in;
	if (newOne) {
		accountNameInput->setText("");
		nameInput->setText("");
		emailInput->setText("");
		popUserInput->setText("");
		popPasswInput->setText("");
		popServerInput->setText("");
		smtpServerInput->setText("");
		syncCheckBox->setChecked(TRUE);
	
		setCaption( tr("Create new Account") );
	} else {
		accountNameInput->setText(account->accountName);
		nameInput->setText(account->name);
		emailInput->setText(account->emailAddress);
		popUserInput->setText(account->popUserName);
		popPasswInput->setText(account->popPasswd);
		popServerInput->setText(account->popServer);
		smtpServerInput->setText(account->smtpServer);
		syncCheckBox->setChecked(account->synchronize);
	}
}

void EditAccount::init()
{
    grid = new QGridLayout(this);
    grid->setSpacing( 6 );
    grid->setMargin( 11 );

    accountNameInputLabel = new QLabel(tr("Account name"), this);
	grid->addWidget( accountNameInputLabel, 0, 0 );
    accountNameInput = new QLineEdit( this, "account nameInput" );
    grid->addWidget( accountNameInput, 0, 1 );

    nameInputLabel = new QLabel(tr("Your name"), this);
    grid->addWidget( nameInputLabel, 1, 0 );
    nameInput = new QLineEdit( this, "nameInput" );
    grid->addWidget( nameInput, 1, 1 );

    emailInputLabel = new QLabel("Email",  this);
    grid->addWidget(emailInputLabel, 2, 0 );
    emailInput = new QLineEdit( this, "emailInput" );
    grid->addWidget( emailInput, 2, 1 );

    popUserInputLabel = new QLabel("POP username", this);
    grid->addWidget( popUserInputLabel, 3, 0 );
    popUserInput = new QLineEdit( this, "popUserInput" );
    grid->addWidget( popUserInput, 3, 1 );

    popPasswInputLabel = new QLabel( "POP password", this);
    grid->addWidget( popPasswInputLabel, 4, 0 );
    popPasswInput = new QLineEdit( this, "popPasswInput" );
    grid->addWidget( popPasswInput, 4, 1 );

    popServerInputLabel = new QLabel("POP server", this);
    grid->addWidget( popServerInputLabel, 5, 0 );
    popServerInput = new QLineEdit( this, "popServerInput" );
    grid->addWidget( popServerInput, 5, 1 );

    smtpServerInputLabel = new QLabel("SMTP server", this );
    grid->addWidget( smtpServerInputLabel, 6, 0 );
    smtpServerInput = new QLineEdit( this, "smtpServerInput" );
    grid->addWidget( smtpServerInput, 6, 1 );

    syncCheckBox = new QCheckBox( tr( "Synchronize with server"  ), this);
    syncCheckBox->setChecked( TRUE );
    grid->addMultiCellWidget( syncCheckBox, 7, 7, 0, 1 );
}


void EditAccount::accept()
{
	account->accountName = accountNameInput->text();
	account->name = nameInput->text();
	account->emailAddress = emailInput->text();
	account->popUserName = popUserInput->text();
	account->popPasswd = popPasswInput->text();
	account->popServer = popServerInput->text();
	account->smtpServer = smtpServerInput->text();
	account->synchronize = syncCheckBox->isChecked();
	
	QDialog::accept();
}

void EditAccount::reject()
{
}
