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

#include "remotetab.h"

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

	timeout = 0;

	addr.sun_family=AF_UNIX;
	strcpy(addr.sun_path,"/dev/lircd");
}

int RemoteTab::sendIR()
{
	const QObject *button = sender();
	QString string = cfg->readEntry(button->name());
	string+='\n';
	const char *write_buffer = string.latin1();
	const char *read_buffer;
	bool done=false;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd == -1)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("RemoteTab::SendIR");
		return 0;
	}


	if(::connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("RemoteTab::SendIR");
	}

	printf("fd2: %d\n", fd);
	printf("%s", write_buffer);

	printf("1\n");
	printf("%d\n", write(fd, write_buffer, strlen(write_buffer) ) );
	printf("2\n");
	while(!done)
	{
		read_buffer=readPacket();
		printf("%s\n", read_buffer);
		if(strcasecmp(read_buffer, "END") == 0)
		{
			printf("done reading packet\n");
			done=true;
		}
	}
	::close(fd);
}

//		printf("%s\n", readPacket());
//		printf("%d\n", read(fd, read_buffer,sizeof(read_buffer)) );
//		printf("%s", read_buffer);

//this function was ripped for rc.c in xrc, it is available here: http://www.lirc.org/software.html
const char *RemoteTab::readPacket()
{
	static char buffer[PACKET_SIZE+1]="";
	char *end;
	static int ptr=0,end_len=0;
	ssize_t ret;

	if(ptr>0)
	{
		memmove(buffer,buffer+ptr,strlen(buffer+ptr)+1);
		ptr=strlen(buffer);
		end=strchr(buffer,'\n');
	}
	else
	{
		end=NULL;
	}
	alarm(TIMEOUT);
	while(end==NULL)
	{
		if(PACKET_SIZE<=ptr)
		{
//			fprintf(stderr,"%s: bad packet\n",progname);
			ptr=0;
			return(NULL);
		}
		ret=read(fd,buffer+ptr,PACKET_SIZE-ptr);

		if(ret<=0 || timeout)
		{
			if(timeout)
			{
//				fprintf(stderr,"%s: timeout\n",progname);
			}
			else
			{
				alarm(0);
			}
			ptr=0;
			return(NULL);
		}
		buffer[ptr+ret]=0;
		ptr=strlen(buffer);
		end=strchr(buffer,'\n');
	}
	alarm(0);timeout=0;

	end[0]=0;
	ptr=strlen(buffer)+1;
//#       ifdef DEBUG
//	printf("buffer: -%s-\n",buffer);
//#       endif
	return(buffer);
}

void RemoteTab::setIRSocket(int newfd)
{
	fd = newfd;
}

void RemoteTab::setConfig(Config *newCfg)
{
	cfg = newCfg;
	cfg->setGroup("Remotes");
	topGroup->updateRemotes(cfg);
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
