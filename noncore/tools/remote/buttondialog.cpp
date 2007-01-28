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

#include "buttondialog.h"

ButtonDialog::ButtonDialog(QString buttonName, QWidget *parent, const char*name, bool modal, WFlags f):QDialog(parent, name, modal, f)
{
	setCaption(tr(buttonName));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QHBoxLayout *hlayout1 = new QHBoxLayout(this);
	QHBoxLayout *hlayout2 = new QHBoxLayout(this);
	QHBoxLayout *hlayout3 = new QHBoxLayout(this);

	layout->addSpacing(5);
	layout->addLayout(hlayout1);
	layout->addSpacing(5);
	layout->addLayout(hlayout2);
	layout->addSpacing(5);
	layout->addLayout(hlayout3);
	layout->addSpacing(5);
	
	lh = new LircHandler();

	remote = new QComboBox(false, this, "remote");
	QLabel *remoteLabel = new QLabel(remote, "Remote: ", this, "remoteLabel");
	hlayout1->addSpacing(5);
	hlayout1->addWidget(remoteLabel);
	hlayout1->addSpacing(5);
	hlayout1->addWidget(remote);
	hlayout1->addSpacing(5);
	remote->insertItem("Remote  ");
	remote->insertStringList(lh->getRemotes());
	connect(remote, SIGNAL(activated(const QString&)), this, SLOT(remoteSelected(const QString&)) );

	button = new QComboBox(false, this, "button");
	QLabel *buttonLabel = new QLabel(remote, "Button: ", this, "buttonLabel");
	hlayout2->addSpacing(5);
	hlayout2->addWidget(buttonLabel);
	hlayout2->addSpacing(5);
	hlayout2->addWidget(button);
	hlayout2->addSpacing(5);
	button->insertItem("Button       ");
	connect(button, SIGNAL(activated(const QString&)), this, SLOT(buttonSelected(const QString&)) );
	
	label = new QLineEdit(this, "label");
	label->setText(buttonName);
	QLabel *labelLabel = new QLabel(label, "Label: ", this, "labelLabel");
	hlayout3->addSpacing(5);
	hlayout3->addWidget(labelLabel);
	hlayout3->addSpacing(5);
	hlayout3->addWidget(label);
	hlayout3->addSpacing(5);
}

ButtonDialog::~ButtonDialog()
{
	delete lh;
}

void ButtonDialog::remoteSelected(const QString &string)
{
	button->insertStringList(lh->getButtons(string.latin1()) );
	list="SEND_ONCE";
	list+=string;
}

void ButtonDialog::buttonSelected(const QString &string)
{
	list+=string;
}

QStringList ButtonDialog::getList()
{
	return list;
}

QString ButtonDialog::getLabel()
{
	return label->text();
}
