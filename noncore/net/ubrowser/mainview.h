/*
Opie-uBrowser.  a very small web browser, using on QTextBrowser for html display/parsing
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <qpe/resource.h>
#include <qpe/qpetoolbar.h>

#include <qmainwindow.h>
#include <qwidget.h>
#include <qtextbrowser.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <stdio.h>

#include "httpfactory.h"

class MainView : public QMainWindow
{
	Q_OBJECT
public:
	MainView(QWidget *parent=0, const char *name=0);
public slots:
	void goClicked();
	void textChanged();
private:
	QToolButton *back;
	QToolButton *forward;
	QToolButton *home;
	QToolButton *go;
	QComboBox *location;
	QTextBrowser *browser;
	HttpFactory *http;
};
