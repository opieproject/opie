/*
Opie-Remote.  emulates remote controlls on an iPaq (and maybe a Zaurus) in Opie.
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

#include <qdialog.h>
#include <qtextview.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qmessagebox.h>

#include <stdio.h>

#include <opie/oprocess.h>

class RecordDialog : public QDialog
{
	Q_OBJECT
public:
	RecordDialog(QWidget *parent=0, const char *name=0);
public slots:
	void retPressed();
	void incoming(OProcess *proc, char *buffer, int len);
	void done(OProcess *proc);
private:
	QTextView *output;
	QLineEdit *input;
	OProcess *record;
	int where;
};