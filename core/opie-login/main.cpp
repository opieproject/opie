#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie/odevice.h>

#include <qwindowsystem_qws.h>
#include <qfile.h>

#include "loginwindowimpl.h"
#include "calibrate.h"
                                                              
int login_main ( int argc, char **argv );


int main ( int argc, char **argv )
{
	if ( geteuid ( ) != 0 ) {
		fprintf ( stderr, "%s can only be executed by root. (or chmod +s)", argv [0] );
		return 1;
	}

//	struct rlimit rl;
//	getrlimit ( RLIMIT_NOFILE, &rl );
	
//	for ( unsigned int i = 0; i < rl. rlim_cur; i++ )
//		close ( i );

	setpgid ( 0, 0 );
	setsid ( );
	
	openlog ( "opie-login", LOG_CONS, LOG_AUTHPRIV );
	
	while ( true ) {
		pid_t child = fork ( );
	
		if ( child < 0 ) {
			syslog ( LOG_ERR, "Could not fork process" );
			break;
			
		}
		else if ( child > 0 ) {
			int status = 0;
			
			while ( waitpid ( child, &status, 0 ) < 0 ) { }	
		}
		else {
			exit ( login_main ( argc, argv ));
		}
	}
	closelog ( );
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




int login_main ( int argc, char **argv )
{
	QWSServer::setDesktopBackground( QImage() );
	QPEApplication app ( argc, argv, QApplication::GuiServer );

	(void) new ModelKeyFilter ( );
		
	{	
	    QCopEnvelope e("QPE/System", "setBacklight(int)" );
    	e << -3; // Forced on
	}

#if defined(QT_QWS_CASSIOPEIA) || defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)
	if ( !QFile::exists ( "/etc/pointercal" )) {
		// Make sure calibration widget starts on top.
		Calibrate *cal = new Calibrate;
		cal-> exec ( );
		delete cal;
	}
#endif
	
	LoginWindowImpl *lw = new LoginWindowImpl ( );
	app. setMainWidget ( lw );
	lw-> setGeometry ( 0, 0, app. desktop ( )-> width ( ), app. desktop ( )-> height ( ));
	lw-> show ( );
	
	return app. exec ( );
}

