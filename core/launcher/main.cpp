/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
** Copyright (C) 2003-2005 The Opie Team <opie-devel@handhelds.org>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**********************************************************************/

#ifndef QTOPIA_INTERNAL_FILEOPERATIONS
#define QTOPIA_INTERNAL_FILEOPERATIONS
#endif
#ifdef QT_QWS_LOGIN
#include "../login/qdmdialogimpl.h"
#endif
#include "calibrate.h"
#include "server.h"
#include "serverapp.h"
#include "stabmon.h"
#include "firstuse.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/oglobal.h>
#include <qtopia/network.h>
#include <qtopia/alarmserver.h>
using namespace Opie::Core;

/* QT */
#include <qdir.h>
#include <qmessagebox.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#include <qtopia/qcopenvelope_qws.h>
#endif
#ifdef Q_WS_QWS
#include <qkeyboard_qws.h>
#endif

/* STD */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void create_pidfile();
void remove_pidfile();

static void cleanup()
{
    QDir dir( "/tmp", "qcop-msg-*" );

    QStringList stale = dir.entryList();
    QStringList::Iterator it;
    for ( it = stale.begin(); it != stale.end(); ++it ) {
    dir.remove( *it );
    }
}

void initEnvironment()
{
    Config config("locale");
    config.setGroup( "Location" );
    QString tz = config.readEntry( "Timezone", getenv("TZ") ).stripWhiteSpace();

    // timezone
    if (tz.isNull() || tz.isEmpty()) tz = "America/New_York";
    setenv( "TZ", tz, 1 );
    config.writeEntry( "Timezone", tz);

    // language
    config.setGroup( "Language" );
    QString lang = config.readEntry( "Language", getenv("LANG") ).stripWhiteSpace();
    if( lang.isNull() || lang.isEmpty()) lang = "en_US";
    setenv( "LANG", lang, 1 );
    config.writeEntry("Language", lang);
    config.write();

    // rotation
    int t = ODevice::inst()->rotation();
    odebug << "ODevice reports transformation to be " << t << oendl;

    QString env( getenv("QWS_DISPLAY") );
    if ( env.isEmpty() )
    {
            int rot = ODevice::inst()->rotation() * 90;
            QString qws_display = QString( "%1:Rot%2:0").arg(ODevice::inst()->qteDriver()).arg(rot);
            odebug << "setting QWS_DISPLAY to '" << qws_display << "'" << oendl;
            setenv("QWS_DISPLAY", (const char*) qws_display, 1);
    }
    else
        odebug << "QWS_DISPLAY already set as '" << env << "' - overriding ODevice transformation" << oendl;

    QPEApplication::defaultRotation(); /* to ensure deforient matches reality */
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
    if ( QWSServer::mouseHandler() &&
         QWSServer::mouseHandler() ->inherits("QCalibratedMouseHandler") ) {
        if ( !QFile::exists( "/etc/pointercal" ) )
            needFirstUse = TRUE;
    }

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
    initEnvironment();

#ifdef QWS
    QWSServer::setDesktopBackground( QImage() );
#endif
    ServerApplication a( argc, argv, QApplication::GuiServer );
    initKeyboard();

    bool firstUseShown = firstUse();
    if ( firstUseShown )
    {
        a.restart();
        return 0;
    }

    {
        QCopEnvelope e("QPE/System", "setBacklight(int)" );
        e << -3; // Forced on
    }

    AlarmServer::initialize();
    Server *s = new Server();
    new SysFileMonitor(s);
#ifdef QWS
    Network::createServer(s);
#endif
    s->show();

    if ( !firstUseShown ) {
        Config config( "qpe" );
        config.setGroup( "Startup" );
        bool showTimeSettings = config.readBoolEntry( "ShowTimeSettings", FALSE );

        if ( !showTimeSettings && QDate::currentDate().year() < 2007 )
        {
            if ( QMessageBox::information ( 0, ServerApplication::tr( "Information" ),
                ServerApplication::tr( "<p>The system date doesn't seem to be valid.\n(%1)</p><p>Do you want to correct the clock ?</p>" )
                                    .arg( TimeString::dateString( QDate::currentDate())),
                    QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
                    {
                        showTimeSettings = TRUE;
                    }
            }

            if ( showTimeSettings )
            {
                QCopEnvelope e ( "QPE/Application/systemtime", "setDocument(QString)" );
                e << QString ( );
            }
    }

    create_pidfile();
    odebug << "--> mainloop in" << oendl;
    int rv = a.exec();
    odebug << "<-- mainloop out" << oendl;
    remove_pidfile();
    odebug << "removing server object..." << oendl;
    delete s;

    odebug << "returning from qpe/initapplication..." << oendl;
    return rv;
}

static const char *pidfile_path = "/var/run/opie.pid";

void create_pidfile()
{
    FILE *f;

    if (( f = ::fopen( pidfile_path, "w" ))) {
        ::fprintf( f, "%d", getpid ( ));
        ::fclose( f );
    }
    else
    {
        odebug << "couldn't create pidfile: " << strerror( errno ) << oendl;
    }
}

void remove_pidfile()
{
    ::unlink( pidfile_path );
}

void handle_sigterm( int sig )
{
    qDebug( "D'oh! QPE Server process got SIGNAL %d. Trying to exit gracefully...", sig );
    ::signal( SIGCHLD, SIG_IGN );
    ::signal( SIGSEGV, SIG_IGN );
    ::signal( SIGBUS, SIG_IGN );
    ::signal( SIGILL, SIG_IGN );
    ::signal( SIGTERM, SIG_IGN );
    ::signal ( SIGINT, SIG_IGN );
    if ( qApp ) qApp->quit();
}

int main( int argc, char ** argv )
{
    ::signal( SIGCHLD, SIG_IGN );
    ::signal( SIGSEGV, handle_sigterm );
    ::signal( SIGBUS, handle_sigterm );
    ::signal( SIGILL, handle_sigterm );
    ::signal( SIGTERM, handle_sigterm );
    ::signal ( SIGINT, handle_sigterm );
    ::setsid();
    ::setpgid( 0, 0 );

    ::atexit( remove_pidfile );

    int retVal = initApplication( argc, argv );

    // Have we been asked to restart?
    if ( ServerApplication::doRestart )
    {
        for ( int fd = 3; fd < 100; fd++ ) close( fd );
        execl( (QPEApplication::qpeDir()+"bin/qpe").latin1(), "qpe", 0 );
    }

    return retVal;
}
