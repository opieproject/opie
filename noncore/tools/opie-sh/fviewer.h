/*
Opie-sh.  convinience app to allow you to use qdialogs in scripts (mainly shell scripts)
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

#include <qwidget.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qdir.h>
#include <qpe/resource.h>

class FViewer :public QWidget
{
	Q_OBJECT
public:
	FViewer(QString icon, QString filename, QString title, QWidget *parent=0, const char*name=0);
private:
	QFile *file;
	QTextBrowser *textView;
	QTextStream *stream;
	QTimer *timer;
};
