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
#if defined( QT_QWS_CUSTOM ) || defined( QT_QWS_IPAQ )
#include <qpe/custom.h>
#endif

#include <opie/odevice.h>

#include <qfile.h>
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

static void initBacklight()
{
    QCopEnvelope e("QPE/System", "setBacklight(int)" );
    e << -3; // Forced on
}


class ModelKeyFilter : public QObject, public QWSServer::KeyboardFilter
{
public:
	ModelKeyFilter ( ) : QObject ( 0, "MODEL_KEY_FILTER" )
	{
		bool doinst = false;
	
		m_model = ODevice::inst ( )-> model ( );
		m_power_timer = 0;
		
		switch ( m_model ) {
			case OMODEL_iPAQ_H31xx:
			case OMODEL_iPAQ_H36xx: 
			case OMODEL_iPAQ_H37xx: 
			case OMODEL_iPAQ_H38xx: doinst = true; 
			                        break;
			default               : break;
		}
		if ( doinst )
			QWSServer::setKeyboardFilter ( this );
	}

	virtual bool filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
	{
		bool kill = false;
	
		// Rotate cursor keys 180°
		switch ( m_model ) {
			case OMODEL_iPAQ_H31xx:
			case OMODEL_iPAQ_H38xx: {
				int newkeycode = keycode;
			
				switch ( keycode ) {
					case Key_Left : newkeycode = Key_Right; break;
					case Key_Right: newkeycode = Key_Left; break;
					case Key_Up   : newkeycode = Key_Down; break;
					case Key_Down : newkeycode = Key_Up; break;
				}
				if ( newkeycode != keycode ) {
					QWSServer::sendKeyEvent ( -1, newkeycode, modifiers, isPress, autoRepeat );
					kill = true;
				}
				break;
			}
			default: break;
		}	
		
		// map Power Button short/long press to F34/F35
		switch ( m_model ) {
			case OMODEL_iPAQ_H31xx:
			case OMODEL_iPAQ_H36xx: 
			case OMODEL_iPAQ_H37xx: 
			case OMODEL_iPAQ_H38xx: {
				if ( keycode == Key_SysReq ) {
					if ( isPress ) {
						if ( m_power_timer )
							killTimer ( m_power_timer );
						m_power_timer = startTimer ( 500 );
					}
					else if ( m_power_timer ) {
						killTimer ( m_power_timer );
						m_power_timer = 0;
						QWSServer::sendKeyEvent ( -1, Key_F34, 0, true, false );
						QWSServer::sendKeyEvent ( -1, Key_F34, 0, false, false );
					}
					kill = true;			
				}
				break;
			}
			default: break;
		}				
		return kill;
	}

	virtual void timerEvent ( QTimerEvent * )
	{
		killTimer ( m_power_timer );
		m_power_timer = 0;
		QWSServer::sendKeyEvent ( -1, Key_F35, 0, true, false );
		QWSServer::sendKeyEvent ( -1, Key_F35, 0, false, false );
	}

private:
	OModel  m_model;
	int     m_power_timer;
};



int initApplication( int argc, char ** argv )
{
	ODevice::inst ( )-> setPowerButtonHandler ( ODevice::OPIE );

    initEnvironment();

#if !defined(QT_QWS_CASSIOPEIA) && !defined(QT_QWS_IPAQ) && !defined(QT_QWS_EBX)
    setenv( "QWS_SIZE", "240x320", 0 );
#endif

    //Don't flicker at startup:
    QWSServer::setDesktopBackground( QImage() );
    DesktopApplication a( argc, argv, QApplication::GuiServer );

	(void) new ModelKeyFilter ( );

    initBacklight();

    AlarmServer::initialize();

    Desktop *d = new Desktop();

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

    int rv =  a.exec();

    delete d;

	ODevice::inst ( )-> setPowerButtonHandler ( ODevice::KERNEL );

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

