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

ButtonDialog::ButtonDialog(QString buttonName, QWidget *parent=0, const char*name=0, bool modal=FALSE, WFlags f=0):QDialog(parent, name, modal, f)
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

	remote = new QComboBox(false, this, "remote");
	QLabel *remoteLabel = new QLabel(remote, "Remote: ", this, "remoteLabel");
	hlayout1->addSpacing(5);
	hlayout1->addWidget(remoteLabel);
	hlayout1->addSpacing(5);
	hlayout1->addWidget(remote);
	hlayout1->addSpacing(5);
	remote->insertItem("Remote  ");
	remote->insertStringList(getRemotes());
	connect(remote, SIGNAL(activated(const QString &)), this, SLOT(remoteSelected(const QString&)) );

	button = new QComboBox(false, this, "button");
	QLabel *buttonLabel = new QLabel(remote, "Button: ", this, "buttonLabel");
	hlayout2->addSpacing(5);
	hlayout2->addWidget(buttonLabel);
	hlayout2->addSpacing(5);
	hlayout2->addWidget(button);
	hlayout2->addSpacing(5);
	button->insertItem("Button       ");
	connect(button, SIGNAL(activated(const QString &)), this, SLOT(buttonSelected(const QString&)) );
	
	label = new QLineEdit(this, "label");
	label->setText(buttonName);
	QLabel *labelLabel = new QLabel(label, "Label: ", this, "labelLabel");
	hlayout3->addSpacing(5);
	hlayout3->addWidget(labelLabel);
	hlayout3->addSpacing(5);
	hlayout3->addWidget(label);
	hlayout3->addSpacing(5);
}

void ButtonDialog::remoteSelected(const QString &string)
{
	button->insertStringList(getButtons(string.latin1()) );
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

QStringList ButtonDialog::getRemotes()
{
	const char write_buffer[] = "LIST\n";
	const char *readbuffer;
	int i, numlines;
	QStringList list;

	addr.sun_family=AF_UNIX;
	strcpy(addr.sun_path,"/dev/lircd");

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
		perror("ButtonDialog::GetRemotes");
		return NULL;
	}

	if(std::connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("ButtonDialog::GetRemotes");
		return NULL;
	}

	write(fd, write_buffer, strlen(write_buffer));

	for(i=0; i<5; i++)
	{
		printf("%d\n", i);
		readbuffer = readPacket();
		printf("%s", readbuffer);
		printf("%d\n", i);
	}

	numlines = atoi(readbuffer);

	for(i=0; i<numlines; i++)
	{
		list+=readPacket();
	}

	if(strcasecmp(readPacket(), "END") != 0)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"bad packet",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("ButtonDialog::GetRemotes");
		return NULL;
	}

	std::close(fd);
	return list;
}

QStringList ButtonDialog::getButtons(const char *remoteName)
{
	QString write_buffer = "LIST ";
	const char *readbuffer;
	int i, j, numlines;
	QStringList list;
	QString string;

	write_buffer += remoteName;
	write_buffer += '\n';

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
		perror("ButtonDialog::GetButtons");
		return NULL;
	}


	if(std::connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("ButtonDialog::GetButtons");
		return NULL;
	}

	write(fd, write_buffer.latin1(), strlen(write_buffer) );

	for(i=0; i<5; i++)
	{
		readbuffer = readPacket();
	}

	numlines = atoi(readbuffer);

	for(i=0; i<numlines; i++)
	{
		list+=readPacket();
		for(j=0; j<list[i].length(); j++)
		{
			if(list[i][j] == ' ')
				break;
		}
		list[i].remove(0, j+1);
	}

	if(strcasecmp(readPacket(), "END") != 0)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"bad packet",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("ButtonDialog::GetButtons");
		return NULL;
	}

	std::close(fd);
	return list;
}


//this function was ripped for rc.c in xrc, it is available here: http://www.lirc.org/software.html
const char *ButtonDialog::readPacket()
{
	static char buffer[PACKET_SIZE+1]="";
	char *end;
	static int ptr=0,end_len=0;
	ssize_t ret;
	timeout = 0;

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
			fprintf(stderr,"bad packet\n");
			ptr=0;
			return(NULL);
		}
		ret=read(fd,buffer+ptr,PACKET_SIZE-ptr);

		if(ret<=0 || timeout)
		{
			if(timeout)
			{
				fprintf(stderr,"timeout\n");
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
