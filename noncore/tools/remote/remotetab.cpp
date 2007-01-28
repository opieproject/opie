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

#include <qmessagebox.h>

#include "remotetab.h"
#include "lirchandler.h"

RemoteTab::RemoteTab(QWidget *parent, const char *name):QWidget(parent,name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	topGroup = new TopGroup(this);
//	topGroup->setMaximumHeight(22);
	layout->addWidget(topGroup, 0, 0);
	printf("%d %d", topGroup->width(), topGroup->height());

	layout->addSpacing(1);

	dvdGroup = new DVDGroup(this);
//	dvdGroup->setMaximumHeight(68);
	layout->addWidget(dvdGroup, 0, 0);

	layout->addSpacing(1);

	vcrGroup = new VCRGroup(this);
	layout->addWidget(vcrGroup, 0, 0);
//	vcrGroup->setMaximumHeight(45);

	layout->addSpacing(1);

	channelGroup = new ChannelGroup(this);
//	channelGroup->setMaximumHeight(91);
	layout->addWidget(channelGroup, 0, 0);

	this->setMaximumWidth(240);
}

int RemoteTab::sendIR()
{
	const QObject *button = sender();
	QString string = cfg->readEntry(button->name());
	if(string != "") {
		string+='\n';
		LircHandler lh;
		return lh.sendIR(string.latin1());
	}
	else {
		QMessageBox::warning(this, tr("Error"), tr("This button has not\nbeen configured"), QMessageBox::Ok, QMessageBox::NoButton);
		return 0;
	}
}

void RemoteTab::setConfig(Config *newCfg)
{
	cfg = newCfg;
	cfg->setGroup("Remotes");
	topGroup->updateRemotes(cfg);
	
	QString curr_remote = topGroup->getRemotesText();
	if(curr_remote != "")
		remoteSelected(curr_remote);
}

void RemoteTab::remoteSelected(const QString &string)
{
	printf("1%s\n", string.latin1() );
	cfg->setGroup(string);
	const QObject *obj;

	const QObjectList *objList = topGroup->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(cfg->hasKey((QString)obj->name()+"Label"))
			{
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
			}
			else
			{
				cfg->setGroup("Default");
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
				cfg->setGroup(string);
			}
		}
	}
	
	objList = dvdGroup->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(cfg->hasKey((QString)obj->name()+"Label"))
			{
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
			}
			else
			{
				cfg->setGroup("Default");
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
				cfg->setGroup(string);
			}
		}
	}

	objList = vcrGroup->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(cfg->hasKey((QString)obj->name()+"Label"))
			{
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
			}
			else
			{
				cfg->setGroup("Default");
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
				cfg->setGroup(string);
			}
		}
	}

	objList = channelGroup->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(cfg->hasKey((QString)obj->name()+"Label"))
			{
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
			}
			else
			{
				cfg->setGroup("Default");
				((QPushButton *)obj)->setText(cfg->readEntry((QString)obj->name()+"Label") );
				cfg->setGroup(string);
			}
		}
	}
}

void RemoteTab::updateRemotesList()
{
	topGroup->updateRemotes(cfg);
}
