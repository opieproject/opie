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

#include "channelgroupconf.h"

ChannelGroupConf::ChannelGroupConf(QWidget *parent=0, const char *name=0):QWidget(parent,name)
{
	QGridLayout *layout = new QGridLayout(this);
	QVBoxLayout *volLayout = new QVBoxLayout(this);
	QVBoxLayout *chanLayout = new QVBoxLayout(this);

	layout->addRowSpacing(1,5);
	layout->addRowSpacing(3,5);
	layout->addRowSpacing(5,5);

	layout->addColSpacing(1,1);
	layout->addColSpacing(3,1);
	layout->addColSpacing(5,1);
	layout->addColSpacing(7,1);

	QPushButton *one = new QPushButton("1", this, "one");
	layout->addWidget(one, 0, 2, 0);
	connect(one, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *two = new QPushButton("2", this, "two");
	layout->addWidget(two, 0, 4, 0);
	connect(two, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *three = new QPushButton("3", this, "three");
	layout->addWidget(three, 0, 6, 0);
	connect(three, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *four = new QPushButton("4", this, "four");
	layout->addWidget(four, 2, 2, 0);
	connect(four, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *five = new QPushButton("5", this, "five");
	layout->addWidget(five, 2, 4, 0);
	connect(five, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *six = new QPushButton("6", this, "six");
	layout->addWidget(six, 2, 6, 0);
	connect(six, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *seven = new QPushButton("7", this, "seven");
	layout->addWidget(seven, 4, 2, 0);
	connect(seven, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *eight = new QPushButton("8", this, "eight");
	layout->addWidget(eight, 4, 4, 0);
	connect(eight, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *nine = new QPushButton("9", this, "nine");
	layout->addWidget(nine, 4, 6, 0);
	connect(nine, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QPushButton *zero = new QPushButton("0", this, "zero");
	layout->addWidget(zero, 6, 4, 0);
	connect(zero, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	layout->addMultiCellLayout(volLayout, 0, 6, 0, 0, 0);
	layout->addMultiCellLayout(chanLayout, 0, 6, 8, 8, 0);

	QPushButton *volUp = new QPushButton("+", this, "volUp");
	volLayout->addWidget(volUp, 1, 0);
	connect(volUp, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QLabel *volLabel = new QLabel("Volume ", this, "volLabel");
	volLayout->addWidget(volLabel, 0, 0);

	QPushButton *volDown = new QPushButton("-", this, "volDown");
	volLayout->addWidget(volDown, 1, 0);
	connect(volDown, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );


	QPushButton *chanUp = new QPushButton("+", this, "chanUp");
	chanLayout->addWidget(chanUp, 1, 0);
	connect(chanUp, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

	QLabel *chanLabel = new QLabel("Channel", this, "chanLabel");
	chanLayout->addWidget(chanLabel, 0, 0);

	QPushButton *chanDown = new QPushButton("-", this, "chanDown");
	chanLayout->addWidget(chanDown, 1, 0);
	connect(chanDown, SIGNAL(pressed()), this->parentWidget(), SLOT(buttonPressed()) );

}
