/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#ifndef QTOPIA_INTERNAL_FILEOPERATIONS
#define QTOPIA_INTERNAL_FILEOPERATIONS
#endif
#include "server.h"
#include "serverapp.h"
#include "taskbar.h"
#include "stabmon.h"
#include "launcher.h"
#include "firstuse.h"

#include <qtopia/qpeapplication.h>
#include <qtopia/network.h>
#include <qtopia/config.h>
#include <qtopia/timezone.h>
#include <qtopia/custom.h>
#include <qtopia/global.h>

#include <qfile.h>
#include <qdir.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#include <qtopia/qcopenvelope_qws.h>
#endif
#include <qtopia/alarmserver.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#ifndef Q_OS_WIN32
#include <unistd.h>
#else
#include <process.h>
#endif

#if defined(QPE_NEED_CALIBRATION)
#include "../calibrate/calibrate.h"
#endif

#ifdef QT_QWS_LOGIN
#include "../login/qdmdialogimpl.h"
#endif

#ifdef Q_WS_QWS
#include <qkeyboard_qws.h>
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
	fprintf( stderr, "mounted /tmp\n" ); // No tr
    }
    if ( mount("none", "/home", "ramfs", 0, 0 ) ) {
	perror("mounting ramfs /home");
    } else {
	fprintf( stderr, "mounted /home\n" ); // No tr
    }
    if ( mount("none","/proc","proc",0,0) ) {
	perror("Mounting - /proc");
    } else {
	fprintf( stderr, "mounted /proc\n" ); // No tr
    }
    if ( mount("none","/mnt","shm",0,0) ) {
	perror("Mounting - shm");
    }
    setenv( "QTDIR", "/", 1 );
    setenv( "QPEDIR", "/", 1 );
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
#include <qtopia/global.h>

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

static void cleanup()
{
    QDir dir( Global::tempDir(), "qcop-msg-*" );

    QStringList stale = dir.entryList();
    QStringList::Iterator it;
    for ( it = stale.begin(); it != stale.end(); ++it ) {
	dir.remove( *it );
    }
}

static void refreshTimeZoneConfig()
{
   // We need to help WorldTime in setting up its configuration for
   //   the current translation
    // BEGIN no tr
    const char *defaultTz[] = {
	"America/New_York",
	"America/Los_Angeles",
	"Europe/Oslo",
	"Asia/Tokyo",
	"Asia/Hong_Kong",
	"Australia/Brisbane",
	0
    };
    // END no tr

    TimeZone curZone;
    QString zoneID;
    int zoneIndex;
    Config cfg = Config( "WorldTime" );
    cfg.setGroup( "TimeZones" );
    if (!cfg.hasKey( "Zone0" )){
	// We have no existing timezones use the defaults which are untranslated strings
	QString currTz = TimeZone::current().id();
	QStringList zoneDefaults;
	zoneDefaults.append( currTz );
	for ( int i = 0; defaultTz[i] && zoneDefaults.count() < 6; i++ ) {
	    if ( defaultTz[i] != currTz )
		zoneDefaults.append( defaultTz[i] );
	}
	zoneIndex = 0;
	for (QStringList::Iterator it = zoneDefaults.begin(); it != zoneDefaults.end() ; ++it){
	    cfg.writeEntry( "Zone" + QString::number( zoneIndex ) , *it);
	    zoneIndex++;
	}
    }
    // We have an existing list of timezones refresh the
    //  translations of TimeZone name
    zoneIndex = 0;
    while (cfg.hasKey( "Zone"+ QString::number( zoneIndex ))){
	zoneID = cfg.readEntry( "Zone" + QString::number( zoneIndex ));
	curZone = TimeZone( zoneID );
	if ( !curZone.isValid() ){
	    qDebug( "initEnvironment() Invalid TimeZone %s", zoneID.latin1() );
	    break;
	}
	cfg.writeEntry( "ZoneName" + QString::number( zoneIndex ), curZone.city() );
	zoneIndex++;
    }

}

void initEnvironment()
{
#ifdef Q_OS_WIN32
    // Config file requires HOME dir which uses QDir which needs the winver
    qt_init_winver();
#endif
    Config config("locale");
    config.setGroup( "Location" );
    QString tz = config.readEntry( "Timezone", getenv("TZ") ).stripWhiteSpace();

    // if not timezone set, pick New York
    if (tz.isNull() || tz.isEmpty())
	tz = "America/New_York";

    setenv( "TZ", tz, 1 );
    config.writeEntry( "Timezone", tz);

    config.setGroup( "Language" );
    QString lang = config.readEntry( "Language", getenv("LANG") ).stripWhiteSpace();
    if( lang.isNull() || lang.isEmpty())
	lang = "en_US";

    setenv( "LANG", lang, 1 );
    config.writeEntry("Language", lang);
    config.write();

    config = Config("qpe");
    config.setGroup( "Rotation" );
    QString dispRep = config.readEntry( "Screen", getenv("QWS_DISPLAY") ).stripWhiteSpace();

/*
    if (!dispRep.isNull() && !dispRep.isEmpty()) {
	setenv( "QWS_DISPLAY", dispRep, 1 );
	config.writeEntry( "Screen", dispRep);
    }
*/
    QString keyOffset = config.readEntry( "Cursor", getenv("QWS_CURSOR_ROTATION") );

    if (keyOffset.isNull())
	keyOffset = "0";

    setenv( "QWS_CURSOR_ROTATION", keyOffset, 1 );
    config.writeEntry( "Cursor", keyOffset);
    config.write();
}

static void initBacklight()
{
#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "setBacklight(int)" );
    e << -3; // Forced on
#endif
}

static void initKeyboard()
{
    Config config("qpe");

    config.setGroup( "Keyboard" );

    int ard = config.readNumEntry( "RepeatDelay" );
    int arp = config.readNumEntry( "RepeatPeriod" );
    if ( ard > 0 && arp > 0 )
	qwsSetKeyboardAutoRepeat( ard, arp );

    QString layout = config.readEntry( "Layout", "us101" );
    Server::setKeyboardLayout( layout );
}

static bool firstUse()
{
    bool needFirstUse = FALSE;
#if defined(QPE_NEED_CALIBRATION)
    if ( !QFile::exists( "/etc/pointercal" ) )
	needFirstUse = TRUE;
#endif

    {
	Config config( "qpe" );
	config.setGroup( "Startup" );
	needFirstUse |= config.readBoolEntry( "FirstUse", TRUE );
    }

    if ( !needFirstUse )
	return FALSE;

    FirstUse *fu = new FirstUse();
    fu->exec();
    bool rs = fu->restartNeeded();
    delete fu;
    return rs;
}

int initApplication( int argc, char ** argv )
{
    cleanup();

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

    //Don't flicker at startup:
#ifdef QWS
    QWSServer::setDesktopBackground( QImage() );
#endif
    ServerApplication a( argc, argv, QApplication::GuiServer );

    refreshTimeZoneConfig();

    initBacklight();

    initKeyboard();

    // Don't use first use under Windows
#ifdef Q_OS_UNIX
    if ( firstUse() ) {
	a.restart();
	return 0;
    }
#endif

    AlarmServer::initialize();

#if defined(QT_QWS_LOGIN)
    for( int i=0; i<a.argc(); i++ )
      if( strcmp( a.argv()[i], "-login" ) == 0 ) { // No tr
	QDMDialogImpl::login( );
	return 0;
      }
#endif

    Server *s = new Server();

    (void)new SysFileMonitor(s);
#ifdef QWS
    Network::createServer(s);
#endif

    s->show();

    int rv =  a.exec();

    qDebug("exiting...");
    delete s;

    return rv;
}

#ifndef Q_OS_WIN32
int main( int argc, char ** argv )
{
#ifndef SINGLE_APP
    signal( SIGCHLD, SIG_IGN );
#endif

    int retVal = initApplication( argc, argv );

#ifdef Q_WS_QWS
    // Have we been asked to restart?
    if ( ServerApplication::doRestart ) {
	for ( int fd = 3; fd < 100; fd++ )
	    close( fd );
# if defined(QT_DEMO_SINGLE_FLOPPY)
	execl( "/sbin/init", "qpe", 0 );
# elif defined(QT_QWS_CASSIOPEIA)
	execl( "/bin/sh", "sh", 0 );
# else
	execl( (QPEApplication::qpeDir()+"bin/qpe").latin1(), "qpe", 0 );
# endif
    }
#endif

#ifndef SINGLE_APP
    // Kill them. Kill them all.
    setpgid( getpid(), getppid() );
    killpg( getpid(), SIGTERM );
    sleep( 1 );
    killpg( getpid(), SIGKILL );
#endif

    return retVal;
}
#else

int main( int argc, char ** argv )
{
    int retVal = initApplication( argc, argv );

    if ( DesktopApplication::doRestart ) {
	qDebug("Trying to restart");
	execl( (QPEApplication::qpeDir()+"bin\\qpe").latin1(), "qpe", 0 );
    }

    return retVal;
}

#endif

