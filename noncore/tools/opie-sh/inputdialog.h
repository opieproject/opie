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

#include <qdialog.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlistbox.h>

class InputDialog : public QDialog
{
	Q_OBJECT
public:
	InputDialog(int w, int h, int newtype, QString labelString, QString title, QString filename, bool edit, QWidget *parent=0, const char *name=0, bool modal=true, WFlags f=0);
	QString getString();
private:
	int type;
	QLineEdit *lineEdit;
	QComboBox *comboBox;
	QListBox *listBox;
};
