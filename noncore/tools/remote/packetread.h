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

file not used
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#define PACKET_SIZE 256
#define TIMEOUT 3

//this function was ripped for rc.c in xrc, it is available here: http://www.lirc.org/software.html
const char *readPacket(int fd)
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
QStringList getRemotes(int fd)
{
	const char write_buffer[] = "LIST\n";
	const char *readbuffer;
	int i, numlines;
	QStringList list;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);

	if(std::connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("ConfigTab::GetRemotes");
		return NULL;
	}

	write(fd, write_buffer, strlen(write_buffer) );

	for(i=0; i<5; i++)
	{
		readbuffer = readPacket(int fd);
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
		perror("ConfigTab::GetRemotes");
		return NULL;
	}

	std::close(fd);
	return list;
}

QStringList getButtons(int fd, const char *remoteName)
{
	QString write_buffer = "LIST ";
	const char *readbuffer;
	int i, j, numlines;
	QStringList list;
	QString string;

	write_buffer += remoteName;
	write_buffer += '\n';

	fd = socket(AF_UNIX, SOCK_STREAM, 0);

	if(std::connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox *mb = new QMessageBox("Error!",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
		perror("ConfigTab::GetRemotes");
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
		perror("ConfigTab::GetRemotes");
		return NULL;
	}

	std::close(fd);
	return list;
}
