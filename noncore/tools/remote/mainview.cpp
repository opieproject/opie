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

#include "mainview.h"

MainView::MainView(QWidget *parent, const char *name) : QWidget(parent, name)
{
	setIcon( Resource::loadPixmap( "remote" ) );
	setCaption(tr("Remote") );

	QVBoxLayout *layout = new QVBoxLayout(this);
	QTabWidget *tabs = new QTabWidget(this);

	layout->addWidget(tabs);

	printf("MainView: 1\n");
	remote = new RemoteTab(tabs);
	printf("MainView: 2\n");
	learn = new LearnTab(tabs);
	printf("MainView: 3\n");
	config = new ConfigTab(tabs);
	printf("MainView: 4\n");
	connect(config, SIGNAL(remotesChanged()), this, SLOT(updateRemotesList()) );
	printf("MainView: 5\n");

	remote->setIRSocket(fd);

	tabs->addTab(remote, tr("Remote") );
	tabs->addTab(learn,tr("Learn") );
	tabs->addTab(config,tr("Config") );

	printf("1\n");
	cfg = new Config("Remote");
	printf("2\n");

	remote->setConfig(cfg);
	config->setConfig(cfg);
}

int MainView::getIRSocket()
{
	return fd;
}

void MainView::setIRSocket(int newfd)
{
	fd = newfd;
	remote->setIRSocket(fd);
}

void MainView::updateRemotesList()
{
	remote->updateRemotesList();
	printf("MainView: got remotes changed signal\n");
}
