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
#ifndef WRITEMAIL_H
#define WRITEMAIL_H

#include <qmainwindow.h>
#include <qaction.h>
#include <qlabel.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qlistview.h>
#include <qcombobox.h>

#include "emailhandler.h"
#include "addresslist.h"
#include "addatt.h"

class WriteMail : public QMainWindow
{ 
	Q_OBJECT

public:
	WriteMail( QWidget* parent, const char* name, WFlags fl = 0 );
	~WriteMail();
	void reply(Email replyMail, bool replyAll);
 	void setRecipient(const QString &recipient);
 	void setAddressList(AddressList *list);
	void forward(Email forwMail);

signals:
	void sendMailRequested(const Email &mail);
	void cancelMail();

	
public slots:
	void getAddress();
	void attachFile();
	void addRecipients();
	void newMail();
	void accept();
	void reject();
	void changeRecipients(int);

private:
 	bool getRecipients(bool);
	void init();
	void addRecipients(bool);
	
	Email mail;
	AddAtt *addAtt;
	AddressList *addressList;
        bool showingAddressList;
	
	QToolBar *bar;
	QMenuBar *menu;
	QPopupMenu *addMenu, *mailMenu;
	QListView *addressView;
	
	QToolButton *okButton;
	QWidget *widget;
	QAction *attachButton;
	QAction *confirmButton;
	QAction *newButton;
	QLabel* subjetLabel;
	QToolButton* ToolButton13_2;
	QComboBox* recipientsBox;
	QLineEdit *subjectInput;
	QLineEdit *toInput;
	QLineEdit *ccInput;
	QToolButton* addressButton;
	QMultiLineEdit* emailInput;
	QGridLayout* grid;
};

#endif // WRITEMAIL_H
