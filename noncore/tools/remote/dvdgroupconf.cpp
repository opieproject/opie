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

#include "dvdgroupconf.h"

DVDGroupConf::DVDGroupConf(QWidget *parent, const char *name):QWidget(parent,name)
{
	QGridLayout *layout = new QGridLayout(this);

//put rows between the buttons of size 1
	layout->addRowSpacing(1,1);
	layout->addRowSpacing(3,1);

//put collumns between the buttons of size 5
	layout->addColSpacing(1,5);
	layout->addColSpacing(3,5);
	layout->addColSpacing(5,5);
	layout->addColSpacing(7,5);

//make sure that the collumns with the buttons in them stretch before the filler collumns do
//since there is so little room, there is no need to do this for the rows
	layout->setColStretch(4,1);
	layout->setColStretch(2,1);
	layout->setColStretch(6,1);
	layout->setColStretch(8,1);
	layout->setColStretch(0,1);

//add the menu navigation buttons, and connect them to the RemoteTab slots...
	QPushButton *up = new QPushButton("Up",this,"up");
	layout->addWidget(up, 0, 4, 0);
	connect(up, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *down = new QPushButton("Down",this,"down");
	layout->addWidget(down, 4, 4, 0);
	connect(down, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *left = new QPushButton("Left",this,"left");
	layout->addWidget(left, 2, 2, 0);
	connect(left, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *right = new QPushButton("Right", this, "right");
	layout->addWidget(right, 2, 6, 0);
	connect(right, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *enter = new QPushButton("Enter",this,"enter");
	layout->addWidget(enter, 2, 4, 0);
	connect(enter, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

//add some other buttons

	QPushButton *eject = new QPushButton("Eject", this, "eject");
	layout->addWidget(eject, 0, 0, 0);
	connect(eject, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *menu = new QPushButton("Menu", this, "menu");
	layout->addWidget(menu, 2, 0, 0);
	connect(menu, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *topMenu = new QPushButton("Top", this, "topMenu");
	layout->addWidget(topMenu, 4, 0, 0);
	connect(topMenu, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *audio = new QPushButton("Audio", this, "audio");
	layout->addWidget(audio, 0, 8, 0);
	connect(audio, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *sub = new QPushButton("Sub", this, "sub");
	layout->addWidget(sub, 2, 8, 0);
	connect(sub, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *angle = new QPushButton("Angle", this, "angle");
	layout->addWidget(angle, 4, 8, 0);
	connect(angle, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );
}
