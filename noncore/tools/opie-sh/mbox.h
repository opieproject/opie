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

#include <qpe/resource.h>
#include <qdialog.h>
#include <qstring.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <unistd.h>

class MBox : public QDialog
{
	Q_OBJECT
public:
	MBox(int w, int h, int type, QString title, QString message, QString *btext0 = 0, QString *btext1= 0, QString *btext2 = 0, QWidget *parent=0, const char*name=0, bool modal=true, WFlags f=0);
public slots:
	void b0pressed(){done(2);};
	void b1pressed(){done(3);};
	void b2pressed(){done(4);};
};
