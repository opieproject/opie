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

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpaintdevice.h>
#include <qobject.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "mainview.h"

/*void reconnect(int &fd)
{
//	struct sockaddr_un addr;

	addr.sun_family=AF_UNIX;
	strcpy(addr.sun_path,"/dev/lircd");
	fd = socket(AF_UNIX, SOCK_STREAM, 0);

	if(connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox *mb = new QMessageBox("error",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
	}
}
*/

int main( int argc, char **argv )
{
	QPEApplication a( argc, argv );
	MainView w;

	int fd;
//	struct sockaddr_un addr;
//	char read_buffer[BUFFERSIZE+1];
//	char write_buffer[] = "LIST\n";

//	addr.sun_family=AF_UNIX;
//	strcpy(addr.sun_path,"/dev/lircd");
//	fd = socket(AF_UNIX, SOCK_STREAM, 0);

//	printf("fd1: %d\n", fd);
/*
	if(connect(fd,(struct sockaddr *) &addr, sizeof(addr) ) == -1)
	{
		QMessageBox *mb = new QMessageBox("error",
											"couldnt connect to socket",
											QMessageBox::NoIcon,
											QMessageBox::Ok,
											QMessageBox::NoButton,
											QMessageBox::NoButton);
		mb->exec();
	}
*/

//	printf("%d\n", write(fd, write_buffer, sizeof(write_buffer) ) );

//	printf("%d\n", read(fd, read_buffer, BUFFERSIZE ) );

//	QMessageBox *mbtest = new QMessageBox("lirc test",
//											*new QString((const char *) read_buffer),
//											QMessageBox::NoIcon,
//											QMessageBox::Ok,
//											QMessageBox::NoButton,
//											QMessageBox::NoButton);
//	mbtest->exec();

	a.setMainWidget( &w );
	w.setIRSocket(fd);
	w.showMaximized();
	 return a.exec();
}
