/*
Opie-Remote.  emulates remote controlls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the 
License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 

GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with 
this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "vcrgroup.h"

VCRGroup::VCRGroup(QWidget *parent=0, const char *name=0) : QWidget(parent, 
name)
{
	QVBoxLayout *vlayout = new QVBoxLayout(this);

	QHBoxLayout *hlayout1 = new QHBoxLayout(this);
	QHBoxLayout *hlayout2 = new QHBoxLayout(this);

	vlayout->addLayout(hlayout1);
	vlayout->addSpacing(1);
	vlayout->addLayout(hlayout2);

	QPushButton *play = new 
QPushButton("Play",this, "play");
	hlayout1->addWidget(play);
	connect(play, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );

	hlayout1->addSpacing(5);

	QPushButton *pause = new 
QPushButton("Pause",this, "pause");
	hlayout1->addWidget(pause);
	connect(pause, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );

	hlayout1->addSpacing(5);

	QPushButton *stop = new 
QPushButton("Stop",this, "stop");
	hlayout1->addWidget(stop);
	connect(stop, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );

	hlayout1->addSpacing(5);

	QPushButton *record = new 
QPushButton("Record",this, "record");
	hlayout1->addWidget(record);
	connect(record, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );

	QPushButton *back = new 
QPushButton("Back",this, "back");
	hlayout2->addWidget(back);
	connect(back, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );

	hlayout2->addSpacing(5);

	QPushButton *rewind = new 
QPushButton("Rewind",this, "rewind");
	hlayout2->addWidget(rewind);
	connect(rewind, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );

	hlayout2->addSpacing(5);

	QPushButton *ff = new 
QPushButton("FF",this, "ff");
	hlayout2->addWidget(ff);
	connect(ff, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );

	hlayout2->addSpacing(5);

	QPushButton *next = new 
QPushButton("Next",this, "next");
	hlayout2->addWidget(next);
	connect(next, SIGNAL(pressed()), this->parentWidget(), SLOT(sendIR()) );
}
