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

/* OPIE */
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

/* QT */
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpaintdevice.h>
#include <qobject.h>

/* STD */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>


int main( int argc, char **argv )
{
	QPEApplication a( argc, argv );
	
	LircHandler lh;
	
	lh.disableIrDA();
	lh.setupModules();
	lh.startLircd();
	if(!lh.isLircdRunning()) {
		QMessageBox::critical(NULL, QObject::tr("Error"),
				QObject::tr("Unable to start lircd"),
				QMessageBox::Ok, QMessageBox::NoButton);
	}
	
	MainView w;
	a.setMainWidget( &w );
	QPEApplication::showWidget( &w );
	int result = a.exec();
	
	lh.stopLircd();
	lh.cleanupModules();
	
	return result;
}
