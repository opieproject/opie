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

#include "mbox.h"

MBox::MBox(int w, int h, int type, QString title, QString message, QString *btext0 = 0, QString *btext1= 0, QString *btext2 = 0, QWidget *parent=0, const char*name=0, bool modal=true, WFlags f=0):QDialog(parent, name, modal, f)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QHBoxLayout *hlayout1 = new QHBoxLayout(this);
	QHBoxLayout *hlayout2 = new QHBoxLayout(this);

	int x, y;
	
	layout->addLayout(hlayout1);
	layout->addLayout(hlayout2);
	
	setCaption(title);

	QLabel *image = new QLabel(this, "image");
	QLabel *text = new QLabel(message, this, "text");
	
	switch (type)
	{
	case 0:
		image->setPixmap(Resource::loadPixmap("opie-sh/info"));
		break;
	case 1:
		image->setPixmap(Resource::loadPixmap("opie-sh/warning"));
		break;
	case 2:
		image->setPixmap(Resource::loadPixmap("opie-sh/error"));
		break;
	}

	hlayout1->addWidget(image);
	hlayout1->addSpacing(5);
	hlayout1->addWidget(text);
	
	if(!btext0->isNull())
	{
		QPushButton *button0 = new QPushButton((const QString)*btext0, this, "button0");
		hlayout2->addSpacing(5);
		hlayout2->addWidget(button0);
		hlayout2->addSpacing(5);
		connect(button0, SIGNAL(clicked()), this, SLOT(b0pressed()) );
	}
	else
	{
		QPushButton *button0 = new QPushButton("Ok", this, "button0");
		hlayout2->addSpacing(5);
		hlayout2->addWidget(button0);
		hlayout2->addSpacing(5);
		connect(button0, SIGNAL(clicked()), this, SLOT(b0pressed()) );
	}
	
	if(!btext1->isNull())
	{
		QPushButton *button1 = new QPushButton((const QString)*btext1, this, "button1");
		hlayout2->addWidget(button1);
		hlayout2->addSpacing(5);
		connect(button1, SIGNAL(clicked()), this, SLOT(b1pressed()) );
	}
	
	if(!btext2->isNull())
	{
		QPushButton *button2 = new QPushButton((const QString)*btext2, this, "button2");
		hlayout2->addWidget(button2);
		hlayout2->addSpacing(5);
		connect(button2, SIGNAL(clicked()), this, SLOT(b2pressed()) );
	}
	
	x=(w/2)-(width()/2);
	y=(h/2)-(height()/2);

	move(x, y);
}
