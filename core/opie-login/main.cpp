#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpestyle.h>
#include <qpe/power.h>

#include <opie/odevice.h>

#include <qwindowsystem_qws.h>
#include <qfile.h>

#include "loginwindowimpl.h"
#include "calibrate.h"

using namespace Opie;

int login_main ( int argc, char **argv );
void sigusr1 ( int sig );
void exit_closelog ( );



int main ( int argc, char **argv )
{
	if ( ::geteuid ( ) != 0 ) {
		::fprintf ( stderr, "%s can only be executed by root. (or chmod +s)", argv [0] );
		return 1;
	}

//	struct rlimit rl;
//	::getrlimit ( RLIMIT_NOFILE, &rl );

//	for ( unsigned int i = 0; i < rl. rlim_cur; i++ )
//		::close ( i );

	::setpgid ( 0, 0 );
	::setsid ( );

	::signal ( SIGUSR1, sigusr1 );

	::openlog ( "opie-login", LOG_CONS, LOG_AUTHPRIV );
	::atexit ( exit_closelog );

	while ( true ) {
		pid_t child = ::fork ( );

		if ( child < 0 ) {
			::syslog ( LOG_ERR, "Could not fork process" );
			break;

		}
		else if ( child > 0 ) {
			int status = 0;

			while ( ::waitpid ( child, &status, 0 ) < 0 ) { }
		}
		else {
			::exit ( login_main ( argc, argv ));
		}
	}
	return 0;
}

void sigusr1 ( int /*sig*/ )
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
	QPEApplication app ( argc, argv, QApplication::GuiServer );

	app. setFont ( QFont ( "Helvetica", 10 ));
	app. setStyle ( new QPEStyle ( ));

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
	app. setMainWidget ( lw );
	lw-> setGeometry ( 0, 0, app. desktop ( )-> width ( ), app. desktop ( )-> height ( ));
	lw-> show ( );

	int rc = app. exec ( );

	ODevice::inst ( )-> setSoftSuspend ( false );

	return rc;
}

