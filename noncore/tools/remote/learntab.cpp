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

#include "learntab.h"

LearnTab::LearnTab(QWidget *parent, const char *name):QWidget(parent,name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *bottomLayout = new QHBoxLayout(this);

	layout->insertSpacing(0,5);
	remotesBox = new QListBox(this, "remotesBox");
	layout->insertWidget(0, remotesBox, 1);
	remotesBox->insertStringList(getRemotes());
	
	layout->insertSpacing(-1,5);
	layout->insertLayout(-1, bottomLayout);
	layout->insertSpacing(-1,5);
	
	QPushButton *add = new QPushButton("Add", this, "add");
	bottomLayout->insertSpacing(-1, 5);
	bottomLayout->insertWidget(-1, add);
	bottomLayout->insertSpacing(-1, 5);
	QPushButton *edit = new QPushButton("Edit", this, "edit");
	bottomLayout->insertWidget(-1, edit);
	bottomLayout->insertSpacing(-1, 5);
	QPushButton *del = new QPushButton("Delete", this, "delete");
	bottomLayout->insertWidget(-1, del);
	bottomLayout->insertSpacing(-1, 5);
	
	connect(add, SIGNAL(clicked()), this, SLOT(add()) );
	connect(edit, SIGNAL(clicked()), this, SLOT(edit()) );
	connect(del, SIGNAL(clicked()), this, SLOT(del()) );
}

void LearnTab::add()
{
	printf("LearnTab::add: add pressed\n");
	RecordDialog *dialog = new RecordDialog(this);
	dialog->showMaximized();
}

void LearnTab::edit()
{
}

void LearnTab::del()
{
}

QStringList LearnTab::getRemotes()
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

	if(::connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
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

	::close(fd);
	return list;
}

//this function was ripped for rc.c in xrc, it is available here: http://www.lirc.org/software.html
const char *LearnTab::readPacket()
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

