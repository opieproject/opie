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
#include "inputdialog.h"

InputDialog::InputDialog(int w, int h, int newtype, QString labelString, QString title, QString filename, bool edit, QWidget *parent, const char *name, bool modal, WFlags f):QDialog(parent, name, modal, f)
{
	type = newtype;
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addStrut(32);
	QLabel *label = new QLabel(labelString, this, "label");
	setCaption(title);
	int x, y;

	layout->addSpacing(5);
	layout->addWidget(label);
	layout->addSpacing(5);

	switch(type)
	{
	case 0:
		lineEdit = new QLineEdit(this, "line edit");
		layout->addWidget(lineEdit);
		break;
	case 1:
		comboBox = new QComboBox(edit, this, "combo box");
		layout->addWidget(comboBox);
		if(!filename.isNull())
		{
			QFile file(filename);
			file.open(IO_ReadOnly);
			QTextStream stream(&file);
			QString string = stream.read();

			comboBox->insertStringList(QStringList::split('\n', string));
		}
		else
		{
			QFile file;
			file.open(IO_ReadOnly, 0);
			QTextStream stream(&file);
			QString string = stream.read();

			comboBox->insertStringList(QStringList::split('\n', string));
		}
		break;
	case 2:
		listBox = new QListBox(this, "list box");
		listBox->setSelectionMode(QListBox::Multi);
		layout->addWidget(listBox);
		if(!filename.isNull())
		{
			QFile file(filename);
			file.open(IO_ReadOnly);
			QTextStream stream(&file);
			QString string = stream.read();

			listBox->insertStringList(QStringList::split('\n', string));
		}
		else
		{
			QFile file;
			file.open(IO_ReadOnly, 0);
			QTextStream stream(&file);
			QString string = stream.read();

			listBox->insertStringList(QStringList::split('\n', string));
		}
		break;
	case 3:
		lineEdit = new QLineEdit(this, "line edit");
		lineEdit->setEchoMode(QLineEdit::Password);
		layout->addWidget(lineEdit);
		break;
	}
	layout->addSpacing(5);

	x=(w/2)-(width()/2);
	y=(h/2)-(height()/2);

	move(x,y);
}

QString InputDialog::getString()
{
	switch (type)
	{
	case 0:
	case 3:
		return ((QLineEdit *)child("line edit"))->text();
		break;
	case 1:
		return ((QComboBox *)child("combo box"))->currentText();
		break;
	case 2:
		QString string;
		int i;
		for(i = 0; i < listBox->count(); i++)
		{
			if(listBox->isSelected(i))
			{
				string+=listBox->text(i)+'\n';
			}
		}
		if(string[string.length()-1] == '\n')
		{
			string.truncate(string.length()-1);
		}
		return string;
	}
	return QString::null;
}
