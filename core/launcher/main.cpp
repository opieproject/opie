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

#if defined(QT_QWS_CASSIOPEIA) || defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)
#include "../calibrate/calibrate.h"
#endif

#ifdef QT_QWS_LOGIN
#include "../login/qdmdialogimpl.h"
#endif

#ifdef QT_QWS_CASSIOPEIA
static void ignoreMessage( QtMsgType, const char * )
{
}
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <qdatetime.h>

void initCassiopeia()
{
    // MIPSEL-specific init - make sure /proc exists for shm
/*
    if ( mount("/dev/ram0", "/", "ext2", MS_REMOUNT | MS_MGC_VAL, 0 ) ) {
	perror("Remounting - / read/write");
    }
*/
    if ( mount("none", "/tmp", "ramfs", 0, 0 ) ) {
	perror("mounting ramfs /tmp");
    } else {
	fprintf( stderr, "mounted /tmp\n" );
    }
    if ( mount("none", "/home", "ramfs", 0, 0 ) ) {
	perror("mounting ramfs /home");
    } else {
	fprintf( stderr, "mounted /home\n" );
    }
    if ( mount("none","/proc","proc",0,0) ) {
	perror("Mounting - /proc");
    } else {
	fprintf( stderr, "mounted /proc\n" );
    }
    if ( mount("none","/mnt","shm",0,0) ) {
	perror("Mounting - shm");
    }
    setenv( "QTDIR", "/", 1 );
    setenv( "OPIEDIR", "/", 1 );
    setenv( "HOME", "/home", 1 );
    mkdir( "/home/Documents", 0755 );

    // set a reasonable starting date
    QDateTime dt( QDate( 2001, 3, 15 ) );
    QDateTime now = QDateTime::currentDateTime();
    int change = now.secsTo( dt );

    time_t t = ::time(0);
    t += change;
    stime(&t);

    qInstallMsgHandler(ignoreMessage);
}
#endif

#ifdef QPE_OWNAPM
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <qpe/global.h>

static void disableAPM() 
{

    int fd, cur_val, ret;
    char *device = "/dev/apm_bios";

    fd = open (device, O_WRONLY);

    if (fd ==  -1) {
      perror(device);
      return;
    }

    cur_val = ioctl(fd, APM_IOCGEVTSRC, 0);
    if (cur_val == -1) {
      perror("ioctl");
      exit(errno);
    }

    ret = ioctl(fd, APM_IOCSEVTSRC, cur_val & ~APM_EVT_POWER_BUTTON);
    if (ret == -1) {
        perror("ioctl");
        return;
    }
    close(fd);
}

static void initAPM()
{
    // So that we have to do it ourself, but better.
    disableAPM();
}
#endif

#ifdef QT_DEMO_SINGLE_FLOPPY
#include <sys/mount.h>

void initFloppy()
{
    mount("none","/proc","proc",0,0);
    setenv( "QTDIR", "/", 0 );
    setenv( "HOME", "/root", 0 );
    setenv( "QWS_SIZE", "240x320", 0 );
}
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
#ifdef QT_QWS_CASSIOPEIA
    initCassiopeia();
#endif

#ifdef QPE_OWNAPM
    initAPM();
#endif

#ifdef QT_DEMO_SINGLE_FLOPPY
    initFloppy();
#endif

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

#if defined(QT_QWS_LOGIN)
    for( int i=0; i<a.argc(); i++ )
      if( strcmp( a.argv()[i], "-login" ) == 0 ) {
	QDMDialogImpl::login( );
	return 0;
      }
#endif

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

    return rv;
}

int main( int argc, char ** argv )
{
#ifndef SINGLE_APP
    signal( SIGCHLD, SIG_IGN );
#endif

    int retVal = initApplication( argc, argv );

#ifndef SINGLE_APP
    // Kill them. Kill them all.
    setpgid( getpid(), getppid() );
    killpg( getpid(), SIGTERM );
    sleep( 1 );
    killpg( getpid(), SIGKILL );
#endif

    return retVal;
}

