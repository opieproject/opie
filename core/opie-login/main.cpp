/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#define _GNU_SOURCE

#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpestyle.h>
#include <qpe/power.h>

#include <opie/odevice.h>

#include <qwindowsystem_qws.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qfile.h>

#include "loginapplication.h"
#include "loginwindowimpl.h"
#include "calibrate.h"

using namespace Opie;

int login_main ( int argc, char **argv );
void sigterm ( int sig );
void exit_closelog ( );

static struct option long_options [] = {
	{ "autologin", 1, 0, 'a' },
	{ 0, 0, 0, 0 }
};


int main ( int argc, char **argv )
{
	if ( ::geteuid ( ) != 0 ) {
		::fprintf ( stderr, "%s can only be executed by root. (or chmod +s)", argv [0] );
		return 1;
	}
	if ( ::getuid ( ) != 0 ) // qt doesn't really like SUID and
		::setuid ( 0 );      // messes up things like config files

	char *autolog = 0;
	int c;
	while (( c = ::getopt_long ( argc, argv, "a:", long_options, 0 )) != -1 ) {
		switch ( c ) {
			case 'a':
				autolog = optarg;
				break;
			default:
				::fprintf ( stderr, "Usage: %s [-a|--autologin=<user>]\n", argv [0] );
				return 2;
		}
	}

//	struct rlimit rl;
//	::getrlimit ( RLIMIT_NOFILE, &rl );

//	for ( unsigned int i = 0; i < rl. rlim_cur; i++ )
//		::close ( i );

	::setpgid ( 0, 0 );
	::setsid ( );

	::signal ( SIGTERM, sigterm );

	::openlog ( "opie-login", LOG_CONS, LOG_AUTHPRIV );
	::atexit ( exit_closelog );

	while ( true ) {
		pid_t child = ::fork ( );

		if ( child < 0 ) {
			::syslog ( LOG_ERR, "Could not fork GUI process\n" );
			break;
		}
		else if ( child > 0 ) {
			int status = 0;
			time_t started = ::time ( 0 );

			while ( ::waitpid ( child, &status, 0 ) < 0 ) { }

			if (( ::time ( 0 ) - started ) < 3 ) {
				if ( autolog ) {
					::syslog ( LOG_ERR, "Respawning too fast -- disabling auto-login\n" );
					autolog = 0;
				}
				else {
					::syslog ( LOG_ERR, "Respawning too fast -- going down\n" );
					break;
				}
			}
			int killedbysig = 0;

			if ( WIFSIGNALED( status )) {
				switch ( WTERMSIG( status )) {
					case SIGINT :
					case SIGTERM:
					case SIGKILL:
						break;

					default     :
						killedbysig = WTERMSIG( status );
						break;
				}
			}
			if ( killedbysig ) {  // qpe was killed by an uncaught signal
				qApp = 0;

				QWSServer::setDesktopBackground ( QImage ( ));
				QApplication *app = new QApplication ( argc, argv, QApplication::GuiServer );
				app-> setFont ( QFont ( "Helvetica", 10 ));
				app-> setStyle ( new QPEStyle ( ));

				const char *sig = ::strsignal ( killedbysig );
				QLabel *l = new QLabel ( 0, "sig", Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool );
				l-> setText ( LoginWindowImpl::tr( "OPIE was terminated\nby an uncaught signal\n(%1)\n" ). arg ( sig ));
				l-> setAlignment ( Qt::AlignCenter );
				l-> move ( 0, 0 );
				l-> resize ( app-> desktop ( )-> width ( ), app-> desktop ( )-> height ( ));
				l-> show ( );
				QTimer::singleShot ( 3000, app, SLOT( quit ( )));
				app-> exec ( );
				delete app;
				qApp = 0;
			}
		}
		else {
			if ( autolog ) {
				LoginApplication::setLoginAs ( autolog );

				if ( LoginApplication::changeIdentity ( ))
					::exit ( LoginApplication::login ( ));
				else
					::exit ( 0 );
			}
			else
				::exit ( login_main ( argc, argv ));
		}
	}
	return 0;
}

void sigterm ( int /*sig*/ )
{
	::exit ( 0 );
}


void exit_closelog ( )
{
	::closelog ( );
}


class LoginScreenSaver : public QWSScreenSaver
{
public:
  LoginScreenSaver ( )
  {
    m_lcd_status = true;

    m_backlight_bright = -1;
    m_backlight_forcedoff = false;

    // Make sure the LCD is in fact on, (if opie was killed while the LCD is off it would still be off)
    ODevice::inst ( )-> setDisplayStatus ( true );
  }
  void restore()
  {
    if ( !m_lcd_status )     // We must have turned it off
      ODevice::inst ( ) -> setDisplayStatus ( true );

    setBacklight ( -3 );
  }
  bool save( int level )
  {
    switch ( level ) {
      case 0:
        if ( backlight() > 1 )
          setBacklight( 1 ); // lowest non-off
        return true;
        break;
      case 1:
        setBacklight( 0 ); // off
        return true;
        break;
      case 2:
        // We're going to suspend the whole machine
        if ( PowerStatusManager::readStatus().acStatus() != PowerStatus::Online ) {
          QWSServer::sendKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );
          return true;
        }
        break;
    }
    return false;
  }

private:
public:
  void setIntervals( int i1 = 30, int i2 = 20, int i3 = 60 )
  {
    int v [4];

    v [ 0 ] = QMAX( 1000 * i1, 100 );
    v [ 1 ] = QMAX( 1000 * i2, 100 );
    v [ 2 ] = QMAX( 1000 * i3, 100 );
    v [ 3 ] = 0;

    if ( !i1 && !i2 && !i3 )
      QWSServer::setScreenSaverInterval ( 0 );
    else
      QWSServer::setScreenSaverIntervals ( v );
  }

  int backlight ( )
  {
    if ( m_backlight_bright == -1 )
      m_backlight_bright = 255;

    return m_backlight_bright;
  }

  void setBacklight ( int bright )
  {
    if ( bright == -3 ) {
      // Forced on
      m_backlight_forcedoff = false;
      bright = -1;
    }
    if ( m_backlight_forcedoff && bright != -2 )
      return ;
    if ( bright == -2 ) {
      // Toggle between off and on
      bright = m_backlight_bright ? 0 : -1;
      m_backlight_forcedoff = !bright;
    }

    m_backlight_bright = bright;

    bright = backlight ( );
    ODevice::inst ( ) -> setDisplayBrightness ( bright );

    m_backlight_bright = bright;
  }

private:
  bool m_lcd_status;

  int m_backlight_bright;
  bool m_backlight_forcedoff;
};




int login_main ( int argc, char **argv )
{
	QWSServer::setDesktopBackground( QImage() );
	LoginApplication *app = new LoginApplication ( argc, argv );

	app-> setFont ( QFont ( "Helvetica", 10 ));
	app-> setStyle ( new QPEStyle ( ));

	ODevice::inst ( )-> setSoftSuspend ( true );

#if defined(QT_QWS_CASSIOPEIA) || defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)
	if ( !QFile::exists ( "/etc/pointercal" )) {
		// Make sure calibration widget starts on top.
		Calibrate *cal = new Calibrate;
		cal-> exec ( );
		delete cal;
	}
#endif

	LoginScreenSaver *saver = new LoginScreenSaver;

	saver-> setIntervals ( );
	QWSServer::setScreenSaver ( saver );
	saver-> restore ( );


	LoginWindowImpl *lw = new LoginWindowImpl ( );
	app-> setMainWidget ( lw );
	lw-> setGeometry ( 0, 0, app-> desktop ( )-> width ( ), app-> desktop ( )-> height ( ));
	lw-> show ( );

	int rc = app-> exec ( );

	ODevice::inst ( )-> setSoftSuspend ( false );

	if ( app-> loginAs ( )) {
		if ( app-> changeIdentity ( )) {
			app-> login ( );

			// if login succeeds, it never comes back

			QMessageBox::critical ( 0, LoginWindowImpl::tr( "Failure" ), LoginWindowImpl::tr( "Could not start OPIE." ));
			rc = 1;
		}
		else {
			QMessageBox::critical ( 0, LoginWindowImpl::tr( "Failure" ), LoginWindowImpl::tr( "Could not switch to new user identity" ));
			rc = 2;
		}

	}
	return rc;
}

