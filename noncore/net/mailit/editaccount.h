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
#ifndef EDITACCOUNT_H
#define EDITACCOUNT_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include "emailhandler.h"

class EditAccount : public QDialog
{ 
    Q_OBJECT

public:
    EditAccount( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    void setAccount(MailAccount *in, bool newOne = TRUE);
    void init();

public slots:
	void accept();
	void reject();

private:
	MailAccount thisAccount, *account;

    QLabel *accountNameInputLabel;
    QLabel *nameInputLabel;
    QLabel *emailInputLabel;
    QLabel *popUserInputLabel;
    QLabel *popPasswInputLabel;
    QLabel *popServerInputLabel;
    QLabel *smtpServerInputLabel;

    QLineEdit *accountNameInput;
    QLineEdit *nameInput;
    QLineEdit *emailInput;
    QLineEdit *popUserInput;
    QLineEdit *popPasswInput;
    QLineEdit *popServerInput;
    QLineEdit *smtpServerInput;
    QCheckBox *syncCheckBox;

    QGridLayout *grid;
};

#endif
