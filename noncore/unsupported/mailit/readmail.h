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
#ifndef READMAIL_H
#define READMAIL_H

#include <qaction.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtextview.h>

#include "emailhandler.h"
#include "emaillistitem.h"
#include "viewatt.h"

class ReadMail : public QMainWindow
{ 
	Q_OBJECT

public:
	ReadMail( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~ReadMail();
	void update(QListView *thisView, Email *mailIn);
	void updateView();
	void mailUpdated(Email *mailIn);

signals:
	void cancelView();
	void replyRequested(Email &, bool &);
	void forwardRequested(Email&);
	void removeItem(EmailListItem *, bool &);
	void viewingMail(Email *);

public slots:
	void close();
	void next();
	void previous();
	void deleteItem();
	void shiftText();
	void viewAttatchments();
	void reply();
	void forward();
	
private:
	void init();
	void updateButtons();
	
private:
	QListView *view;
	EmailListItem *item;
	bool plainTxt, inbox;
	Email *mail;
	ViewAtt *viewAtt;
	
	QToolBar *bar;
	QMenuBar *menu;
	QPopupMenu *viewMenu, *mailMenu;
	QAction *deleteButton;
	QMimeSourceFactory *mime;
	QAction *plainTextButton;
	QAction *nextButton;
	QTextView *emailView;
	QAction *attatchmentsButton;
	QAction *previousButton;
	QAction *replyButton;
	QAction *forwardButton;
};

#endif // READMAIL_H
