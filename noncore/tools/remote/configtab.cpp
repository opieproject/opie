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

#include "configtab.h"

ConfigTab::ConfigTab(QWidget *parent, const char *name):QWidget(parent,name)
{

	QVBoxLayout *layout  =  new QVBoxLayout(this);

	topGroupConf = new TopGroupConf(this, "topGroupConf");
	layout->addWidget(topGroupConf, 1);
	layout->addSpacing(1);

	dvdGroupConf = new DVDGroupConf(this, "dvdGroupConf");
	layout->addWidget(dvdGroupConf, 1);
	layout->addSpacing(1);

	vcrGroupConf = new VCRGroupConf(this, "vcrGroupConf");
	layout->addWidget(vcrGroupConf, 1);
	layout->addSpacing(1);

	channelGroupConf = new ChannelGroupConf(this, "channelGroupConf");
	layout->addWidget(channelGroupConf, 1);
	

}

void ConfigTab::setConfig(Config *newCfg)
{
	cfg = newCfg;
	cfg->setGroup("Remotes");
	topGroupConf->updateRemotes(cfg->readListEntry("remoteList", ',') );
	cfg->setGroup("Default");
	const QObject *obj;

	const QObjectList *objList = topGroupConf->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(!cfg->hasKey((QString)obj->name()+"Label"))
			{
				cfg->writeEntry((QString)obj->name()+"Label", ((QPushButton *)obj)->text());
			}
		}
	}

	objList = dvdGroupConf->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(!cfg->hasKey((QString)obj->name()+"Label"))
			{
				cfg->writeEntry((QString)obj->name()+"Label", ((QPushButton *)obj)->text());
			}
		}
	}

	objList = vcrGroupConf->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(!cfg->hasKey((QString)obj->name()+"Label"))
			{
				cfg->writeEntry((QString)obj->name()+"Label", ((QPushButton *)obj)->text());
			}
		}
	}

	objList = channelGroupConf->children();
	for(obj = ((QObjectList *)objList)->first(); obj != 0; obj=((QObjectList *)objList)->next())
	{
		if(obj->inherits("QPushButton"))
		{
			if(!cfg->hasKey((QString)obj->name()+"Label"))
			{
				cfg->writeEntry((QString)obj->name()+"Label", ((QPushButton *)obj)->text());
			}
		}
	}
	cfg->write();

	cfg->setGroup("Remotes");
}
/*
void ConfigTab::savePressed()
{
	cfg->setGroup(remotes->currentText());
	ButtonDialog *bd = new ButtonDialog(this, "BD", true, 0);
	if( bd->exec() == 1)
	{
		printf("%s\n", bd->getList().join(" ").latin1());
	}
}
*/

void ConfigTab::newPressed()
{
	QStringList list;
	cfg->setGroup("Remotes");
	list=cfg->readListEntry("remoteList", ',');
	list+=topGroupConf->getRemotesText();
	cfg->writeEntry("remoteList", list, ',');
	cfg->setGroup(topGroupConf->getRemotesText());
	topGroupConf->updateRemotes(QStringList(topGroupConf->getRemotesText()) );
	cfg->write();
	emit remotesChanged();
	remoteSelected(topGroupConf->getRemotesText());
}

void ConfigTab::remoteSelected(const QString &string)
{
	cfg->setGroup(string);
	const QObject *obj;

	const QObjectList *objList = topGroupConf->children();
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

	objList = dvdGroupConf->children();
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

	objList = vcrGroupConf->children();
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

	objList = channelGroupConf->children();
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

void ConfigTab::buttonPressed()
{
	const QObject *button = sender();
	QString string = button->name();

	ButtonDialog *bd = new ButtonDialog(((QPushButton *)button)->text(), this, "BD", true, 0);
	if( bd->exec() == 1)
	{
		cfg->writeEntry(string, bd->getList().join(" ").latin1());
		cfg->writeEntry(string+"Label", bd->getLabel().latin1());
	}
	cfg->write();
	((QPushButton *)button)->setText(bd->getLabel());
}
