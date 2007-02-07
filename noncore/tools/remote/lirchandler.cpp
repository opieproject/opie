/*
Opie-Remote.  emulates remote controls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2007 Paul Eggleton & Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <qstring.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qtextstream.h>
#include <opie2/oprocess.h>
#include <qpe/qcopenvelope_qws.h>

#include "lirchandler.h"

#define PACKET_SIZE 256
#define TIMEOUT 3
#define LIRCD_SOCKET "/dev/lircd"
#define LIRCD_SERVICECMD "/etc/init.d/lircd"
#define LIRCD_CONF "/etc/lircd.conf"

using namespace Opie::Core;

LircHandler::LircHandler(void)
{
	fd = 0;
	addr.sun_family=AF_UNIX;
	strcpy(addr.sun_path, LIRCD_SOCKET);
}

bool LircHandler::connectLirc(void)
{
	if(!checkLircdConfValid(false))
		return false;
	
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd == -1)
	{
		QMessageBox mb(QObject::tr("Error"),
											QObject::tr("Unable to create socket"),
											QMessageBox::Critical,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb.exec();
		perror("LircHandler::connectLirc");
		return false;
	}
	
	if(::connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox mb(QObject::tr("Error"),
											QObject::tr("Could not connect to lircd"),
											QMessageBox::Critical,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb.exec();
		perror("LircHandler::connectLirc");
		return false;
	}
	
	return true;
}

//this function was ripped for rc.c in xrc, it is available here: http://www.lirc.org/software.html
const char *LircHandler::readPacket()
{
	static char buffer[PACKET_SIZE+1]="";
	char *end;
	static int ptr=0;
	ssize_t ret;
	int timeout = 0;

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
			fprintf(stderr,"readPacket: bad packet\n");
			ptr=0;
			return(NULL);
		}
		ret=read(fd,buffer+ptr,PACKET_SIZE-ptr);

		if(ret<=0 || timeout)
		{
			if(timeout)
			{
				fprintf(stderr,"readPacket: timeout\n");
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
	alarm(0);

	end[0]=0;
	ptr=strlen(buffer)+1;
//#       ifdef DEBUG
//	printf("buffer: -%s-\n",buffer);
//#       endif
	return(buffer);
}

QStringList LircHandler::getRemotes(void)
{
	const char write_buffer[] = "LIST\n";
	const char *readbuffer;
	int i, numlines;
	QStringList list;
	
	if(connectLirc()) {
		write(fd, write_buffer, strlen(write_buffer) );
	
		for(i=0; i<5; i++)
		{
			readbuffer = readPacket();
		}
	
		numlines = atoi(readbuffer);
	
		for(i=0; i<numlines; i++)
		{
			list+=readPacket();
		}
	
		if(strcasecmp(readPacket(), "END") != 0)
		{
			QMessageBox mb(QObject::tr("Error"),
												QObject::tr("Bad packet while communicating with lircd"),
												QMessageBox::Critical,
												QMessageBox::Ok,
												QMessageBox::NoButton,
												QMessageBox::NoButton);
			mb.exec();
			perror("LircHandler::getRemotes");
			return NULL;
		}
	
		::close(fd);
	}
	
	return list;
}

QStringList LircHandler::getButtons(const char *remoteName)
{
	QString write_buffer = "LIST ";
	const char *readbuffer;
	int i, j, numlines;
	QStringList list;
	QString string;

	write_buffer += remoteName;
	write_buffer += '\n';

	if(connectLirc()) {
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
			QMessageBox mb(QObject::tr("Error"),
												QObject::tr("Bad packet while communicating with lircd"),
												QMessageBox::Critical,
												QMessageBox::Ok,
												QMessageBox::NoButton,
												QMessageBox::NoButton);
			mb.exec();
			perror("LircHandler::getRemotes");
			return NULL;
		}
		
		::close(fd);
	}
		
	return list;
}

int LircHandler::sendIR(const char *lircaction)
{
	const char *read_buffer;
	bool done=false;

	if(connectLirc()) {
		printf("fd2: %d\n", fd);
		printf("%s", lircaction);
	
		printf("1\n");
		printf("%d\n", write(fd, lircaction, strlen(lircaction) ) );
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
		return 1;
	}
	else
		return 0;
}

bool LircHandler::startLircd(void)
{
	return (system(LIRCD_SERVICECMD " start") == 0);
}

bool LircHandler::stopLircd(void)
{
	return (system(LIRCD_SERVICECMD " stop") == 0);
}

bool LircHandler::isLircdRunning(void)
{
	return (OProcess::processPID("lircd") != 0);
}

void LircHandler::reloadLircdConf(void)
{
	int pid = OProcess::processPID("lircd");
	if(pid > -1)
		kill(pid, SIGHUP);
	else
		startLircd();
}

bool LircHandler::setupModules(void)
{
	// Remove IrDA modules which get in the way
	system("rmmod ircomm-tty ircomm");
	// Load LIRC device driver
	system("modprobe lirc_sir");
	return true;
}

bool LircHandler::cleanupModules(void)
{
	// Unload LIRC device driver
	system("rmmod lirc_sir");
	// Load IrDA drivers back in
	system("modprobe ircomm-tty");
	return true;
}

void LircHandler::disableIrDA(void)
{
	QCopEnvelope e("QPE/IrDaApplet", "disableIrda()");
}

void LircHandler::mergeRemoteConfig(const QString &newconfig)
{
	QStringList contents;
	QStringList newcontents;
	QFile conf(LIRCD_CONF);
	QFile newconf(newconfig);
	
	readFromFile(conf, contents);
	readFromFile(newconf, newcontents);
	contents += newcontents;
	
	writeToFile(conf, contents);
}

void LircHandler::removeRemote(const QString &remotetodelete)
{
	QStringList contents;
	QFile conf(LIRCD_CONF);
	bool found = false;
	bool inremote = false;
	QString remotename("");
	int lineidx = 0;
	int startidx = 0;
	int lastendidx = 0;
	
	readFromFile(conf, contents);
	
	for (QStringList::Iterator it = contents.begin(); it != contents.end(); ++it ) {
		QString line = (*it).stripWhiteSpace();
		if(line == "begin remote") {
			startidx = lastendidx;
			inremote = true;
		}
		else if(line == "end remote") {
			lastendidx = lineidx + 1;
			if(remotename == remotetodelete) {
				found = true;
				break;
			}
			inremote = false;
		}
		else if(inremote && line.left(4) == "name") {
			remotename = line.mid(4).stripWhiteSpace();
		}
		lineidx++;
	}
	
	if(found) {
		// Remove the remote and any preceding lines (most likely associated comments)
		int linecount = lastendidx - startidx; 
		QStringList::Iterator it = contents.at(startidx);
		for (int i = 0; i < linecount; i++ ) {
			it = contents.remove(it);
		}
		
		// Check if there is at least one remote still defined
		found = false;
		for (it = contents.begin(); it != contents.end(); ++it ) {
			QString line = (*it).stripWhiteSpace();
			if(line == "begin remote") {
				found = true;
				break;
			}
		}
		
		if(found)
			writeToFile(conf, contents);
		else
			conf.remove();
	}
}

bool LircHandler::checkRemoteExists(const QString &remote)
{
	QStringList contents;
	QFile conf(LIRCD_CONF);
	bool inremote = false;
	
	readFromFile(conf, contents);
	
	for (QStringList::Iterator it = contents.begin(); it != contents.end(); ++it ) {
		QString line = (*it).stripWhiteSpace();
		if(line == "begin remote") {
			inremote = true;
		}
		else if(line == "end remote") {
			inremote = false;
		}
		else if(inremote && line.left(4) == "name") {
			QString rname = line.mid(4).stripWhiteSpace();
			if(rname == remote)
				return true;
		}
	}
	return false;
}

bool LircHandler::checkLircdConfValid(bool silent)
{
	QStringList contents;
	QFile conf(LIRCD_CONF);
	bool inremote = false;
	
	if(conf.exists()) {
		readFromFile(conf, contents);
		
		for (QStringList::Iterator it = contents.begin(); it != contents.end(); ++it ) {
			QString line = (*it).stripWhiteSpace();
			if(line == "begin remote") {
				inremote = true;
			}
			else if(line == "end remote") {
				if(inremote)
					return true;
			}
		}
	}
	
	if(!silent) {
		QMessageBox::information(NULL, QObject::tr("No remote"),
				QObject::tr("No remotes have been learned.\nPlease go to the Learn tab\nand learn a remote."),
				QMessageBox::Ok, QMessageBox::NoButton);
	}
	
	return false;
}

bool LircHandler::readFromFile(QFile &file, QStringList &strlist) 
{
	if(file.open(IO_ReadOnly)) {
		QTextStream in(&file);

		strlist = QStringList::split('\n', in.read(), true);
		file.close();
		if(strlist.count() > 0)
			strlist.remove(strlist.at(strlist.count() - 1));  // remove extra blank line
	}
	return true;
}

bool LircHandler::writeToFile(QFile &file, QStringList &strlist) 
{
	if(file.open(IO_WriteOnly | IO_Truncate)) {
		QTextStream out(&file);
		for (QStringList::Iterator it = strlist.begin(); it != strlist.end(); ++it ) {
			out << (*it) << "\n";
		}
		file.close();
	}
	return true;
}
