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
#ifndef MailItWindow_H
#define MailItWindow_H

#include <qmainwindow.h>
#include <qwidgetstack.h>
#include <qevent.h>
//#include <qlayout.h>
#include "emailclient.h"
#include "writemail.h"
#include "readmail.h"
#include "addresslist.h"

class MailItWindow: public QMainWindow
{
	Q_OBJECT
public:
	MailItWindow(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);
	~MailItWindow();

signals:
public slots:
	void compose();
	void composeReply(Email &, bool&);
	void composeForward(Email &);	
	void showEmailClient();
	void viewMail(QListView *, Email *mail);
	void updateMailView(Email *mail);
	void closeEvent(QCloseEvent *e);
	void updateCaption(const QString &);
	void setDocument(const QString &);
	/*void reply(Email&);
	void replyAll(Email&);
	void forward(Email&);
	void remove(Email&);*/
	
private:
	EmailClient *emailClient;
	WriteMail *writeMail;
	ReadMail *readMail;
	QWidgetStack *views;
	QString currentCaption;
	bool viewingMail;
};

#endif
