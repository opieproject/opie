/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "desktop.h"
#include "taskbar.h"
#include "stabmon.h"

#include <qpe/qpeapplication.h>
#include <qpe/network.h>
#include <qpe/config.h>
#if defined( QT_QWS_SHARP ) || defined( QT_QWS_IPAQ )
#include <qpe/custom.h>
#endif

#include <opie/odevice.h>

#include <qmessagebox.h>
#include <qfile.h>
#include <qimage.h>
#include <qwindowsystem_qws.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/alarmserver.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)
#include "../calibrate/calibrate.h"
#endif

using namespace Opie;

void initEnvironment()
{
  Config config("locale");
  config.setGroup( "Location" );
  QString tz = config.readEntry( "Timezone", getenv("TZ") );

  // if not timezone set, pick New York
  if (tz.isNull())
      tz = "America/New_York";

  setenv( "TZ", tz, 1 );
  config.writeEntry( "Timezone", tz);

  config.setGroup( "Language" );
  QString lang = config.readEntry( "Language", getenv("LANG") );
  if ( !lang.isNull() )
    setenv( "LANG", lang, 1 );
}


int initApplication( int argc, char ** argv )
{
    initEnvironment();

#if !defined(QT_QWS_CASSIOPEIA) && !defined(QT_QWS_IPAQ) && !defined(QT_QWS_EBX)
    setenv( "QWS_SIZE", "240x320", 0 );
#endif

    //Don't flicker at startup:
    QWSServer::setDesktopBackground( QImage() );
    DesktopApplication a( argc, argv, QApplication::GuiServer );

	ODevice::inst ( )-> setSoftSuspend ( true );

	{ // init backlight
	    QCopEnvelope e("QPE/System", "setBacklight(int)" );
   	 e << -3; // Forced on
	}

    AlarmServer::initialize();

    Desktop *d = new Desktop();

    QObject::connect( &a, SIGNAL(menu()), d, SLOT(raiseMenu()) );
    QObject::connect( &a, SIGNAL(datebook()), d, SLOT(raiseDatebook()) );
    QObject::connect( &a, SIGNAL(contacts()), d, SLOT(raiseContacts()) );
    QObject::connect( &a, SIGNAL(launch()),   d, SLOT(raiseLauncher()) );
    QObject::connect( &a, SIGNAL(email()),   d, SLOT(raiseEmail()) );
    QObject::connect( &a, SIGNAL(power()),   d, SLOT(togglePower()) );
    QObject::connect( &a, SIGNAL(backlight()),   d, SLOT(toggleLight()) );
    QObject::connect( &a, SIGNAL(symbol()),   d, SLOT(toggleSymbolInput()) );
    QObject::connect( &a, SIGNAL(numLockStateToggle()),   d, SLOT(toggleNumLockState()) );
    QObject::connect( &a, SIGNAL(capsLockStateToggle()),   d, SLOT(toggleCapsLockState()) );
    QObject::connect( &a, SIGNAL(prepareForRestart()),   d, SLOT(terminateServers()) );

    (void)new SysFileMonitor(d);
    Network::createServer(d);

#if defined(QT_QWS_CASSIOPEIA) || defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)
    if ( !QFile::exists( "/etc/pointercal" ) ) {
	// Make sure calibration widget starts on top.
	Calibrate *cal = new Calibrate;
	cal->exec();
	delete cal;
    }
#endif

    d->show();

	if ( QDate::currentDate ( ). year ( ) < 2000 ) {
		if ( QMessageBox::information ( 0, DesktopApplication::tr( "Information" ), DesktopApplication::tr( "<p>The system date doesn't seem to be valid.\n(%1)</p><p>Do you want to correct the clock ?</p>" ). arg( TimeString::dateString ( QDate::currentDate ( ))), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes ) {
			QCopEnvelope e ( "QPE/Application/systemtime", "setDocument(QString)" );
			e << QString ( );		                              
		}
	}

    int rv =  a.exec();

    delete d;

	ODevice::inst ( )-> setSoftSuspend ( false );

    return rv;
}

static const char *pidfile_path = "/var/run/opie.pid";

void create_pidfile ( )
{
	FILE *f;

	if (( f = ::fopen ( pidfile_path, "w" ))) {
		::fprintf ( f, "%d", getpid ( ));
		::fclose ( f );
	}
}

void remove_pidfile ( )
{
	::unlink ( pidfile_path );
}

void handle_sigterm ( int /* sig */ )
{
	if ( qApp )
		qApp-> quit ( );
}

int main( int argc, char ** argv )
{
    ::signal ( SIGCHLD, SIG_IGN );

	::signal ( SIGTERM, handle_sigterm );
	::signal ( SIGINT, handle_sigterm );

	::setsid ( );
	::setpgid ( 0, 0 );

	::atexit ( remove_pidfile );
	create_pidfile ( );

    int retVal = initApplication ( argc, argv );

    // Kill them. Kill them all.
    ::kill ( 0, SIGTERM );
    ::sleep ( 1 );
    ::kill ( 0, SIGKILL );

    return retVal;
}

