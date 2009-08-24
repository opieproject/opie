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

#ifndef RecordDialog_H
#define RecordDialog_H

#include <qdialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include <stdio.h>

#include <opie2/oprocess.h>

#include "lirchandler.h"

class RecordDialog : public QDialog
{
	Q_OBJECT
public:
	RecordDialog(QWidget *parent=0, const char *name=0);
	~RecordDialog(void);
public slots:
	void nextPressed();
	void incoming(Opie::Core::OProcess *proc, char *buffer, int len);
	void incomingErr(Opie::Core::OProcess *proc, char *buffer, int len);
	void writeFinished(Opie::Core::OProcess *proc);
	void done(Opie::Core::OProcess *proc);
	void accept();
	void reject();
protected:
	bool confirmClose();
	bool checkClose();
private:
	QMultiLineEdit *output;
	QLineEdit *input;
  QPushButton *nextbtn;
	Opie::Core::OProcess *record;
	int stepnum;
	bool outputenabled;
	bool decoding;
	bool writeok;
	bool ignorekill;
	QString errors;
	QString remotefile;
	LircHandler *lh;

	void writeToProcess(const char *buffer, int len);
};

#endif

