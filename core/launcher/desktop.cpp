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
#include "info.h"
#include "launcher.h"
#include "qcopbridge.h"
#include "shutdownimpl.h"
#include "startmenu.h"
#include "taskbar.h"
#include "transferserver.h"
#include "irserver.h"
#include "packageslave.h"
#include "screensaver.h"

#include <qpe/applnk.h>
#include <qpe/mimetype.h>
#include <qpe/password.h>
#include <qpe/config.h>
#include <qpe/power.h>
#include <qpe/timeconversion.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/network.h>
#include <qpe/global.h>

#if defined( QT_QWS_SHARP ) || defined( QT_QWS_IPAQ )
#include <qpe/custom.h>
#endif

#include <opie/odevice.h>

#include <qgfx_qws.h>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qwindowsystem_qws.h>

#include <qvaluelist.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


using namespace Opie;

class QCopKeyRegister
{
public:
	QCopKeyRegister() : keyCode( 0 )
	{ }
	QCopKeyRegister( int k, const QCString &c, const QCString &m )
			: keyCode( k ), channel( c ), message( m )
	{ }

	int getKeyCode() const
	{
		return keyCode;
	}
	QCString getChannel() const
	{
		return channel;
	}
	QCString getMessage() const
	{
		return message;
	}

private:
	int keyCode;
	QCString channel, message;
};

typedef QValueList<QCopKeyRegister> KeyRegisterList;
KeyRegisterList keyRegisterList;

static Desktop* qpedesktop = 0;
static int loggedin = 0;
static void login( bool at_poweron )
{
	if ( !loggedin ) {
		Global::terminateBuiltin( "calibrate" );
		Password::authenticate( at_poweron );
		loggedin = 1;
		QCopEnvelope e( "QPE/Desktop", "unlocked()" );
	}
}

bool Desktop::screenLocked()
{
	return loggedin == 0;
}

/*
  Priority is number of alerts that are needed to pop up
  alert.
 */
class DesktopPowerAlerter : public QMessageBox
{
public:
	DesktopPowerAlerter( QWidget *parent, const char *name = 0 )
			: QMessageBox( tr( "Battery Status" ), "Low Battery",
			               QMessageBox::Critical,
			               QMessageBox::Ok | QMessageBox::Default,
			               QMessageBox::NoButton, QMessageBox::NoButton,
			               parent, name, FALSE )
	{
		currentPriority = INT_MAX;
		alertCount = 0;
	}

	void alert( const QString &text, int priority );
	void hideEvent( QHideEvent * );
private:
	int currentPriority;
	int alertCount;
};

void DesktopPowerAlerter::alert( const QString &text, int priority )
{
	alertCount++;
	if ( alertCount < priority )
		return ;
	if ( priority > currentPriority )
		return ;
	currentPriority = priority;
	setText( text );
	show();
}


void DesktopPowerAlerter::hideEvent( QHideEvent *e )
{
	QMessageBox::hideEvent( e );
	alertCount = 0;
	currentPriority = INT_MAX;
}


void DesktopApplication::switchLCD ( bool on )
{
	if ( qApp ) {
		DesktopApplication *dapp = (DesktopApplication *) qApp;

		if ( dapp-> m_screensaver ) {
			if ( on ) {
				dapp-> m_screensaver-> setDisplayState ( true );
				dapp-> m_screensaver-> setBacklight ( -3 );
			}
			else {
				dapp-> m_screensaver-> setDisplayState ( false );
			}
		}
	}
}


DesktopApplication::DesktopApplication( int& argc, char **argv, Type appType )
		: QPEApplication( argc, argv, appType )
{

	Config cfg( "apm" );
	cfg.setGroup( "Warnings" );
	//cfg.readNumEntry( "checkinterval", 10000 )
	m_powerVeryLow = cfg.readNumEntry( "powerverylow", 10 );
	m_powerCritical = cfg.readNumEntry( "powercritical", 5 );

	m_ps = new PowerStatus;
	m_ps_last = new PowerStatus;
	pa = new DesktopPowerAlerter( 0 );

	m_timer = new QTimer( this );
	connect( m_timer, SIGNAL( timeout() ), this, SLOT( apmTimeout() ) );
	m_timer->start( 5000 );

	m_last_button = 0;
	m_button_timer = new QTimer ( );
	connect ( m_button_timer, SIGNAL( timeout ( )), this, SLOT( sendHeldAction ( )));
	
	channel = new QCopChannel( "QPE/System", this );
	connect( channel, SIGNAL( received( const QCString&, const QByteArray& ) ),
	         this, SLOT( systemMessage( const QCString&, const QByteArray& ) ) );

	channel = new QCopChannel( "QPE/Launcher", this );
	connect( channel, SIGNAL( received( const QCString&, const QByteArray& ) ),
	         this, SLOT( launcherMessage( const QCString&, const QByteArray& ) ) );

	m_screensaver = new OpieScreenSaver ( );
	m_screensaver-> setInterval ( -1 );
	QWSServer::setScreenSaver( m_screensaver );

	rereadVolumes();
	connect( qApp, SIGNAL( volumeChanged( bool ) ), this, SLOT( rereadVolumes() ) );
	
	apmTimeout ( );
	
	grabKeyboard ( );
}


DesktopApplication::~DesktopApplication()
{
	ungrabKeyboard ( );

	delete m_ps;
	delete m_ps_last;
	delete pa;
}

void DesktopApplication::apmTimeout()
{
	qpedesktop->checkMemory(); // in case no events are being generated

	*m_ps_last = *m_ps;
	*m_ps = PowerStatusManager::readStatus();

	if ( m_ps-> acStatus ( ) != m_ps_last-> acStatus ( ))
		m_screensaver-> powerStatusChanged ( *m_ps );

	if ( m_ps-> acStatus ( ) != PowerStatus::Online ) {
		int bat = m_ps-> batteryPercentRemaining ( );

		if ( bat < m_ps_last-> batteryPercentRemaining ( )) {
			if ( bat <= m_powerCritical )
				pa->alert( tr( "Battery level is critical!\nKeep power off until power restored!" ), 1 );
			else if ( bat <= m_powerVeryLow )
				pa->alert( tr( "Battery is running very low." ), 2 );
		}
		if ( m_ps-> backupBatteryStatus ( ) == PowerStatus::VeryLow )
			pa->alert( tr( "The Back-up battery is very low.\nPlease charge the back-up battery." ), 2 );
	}
}


void DesktopApplication::systemMessage( const QCString & msg, const QByteArray & data )
{
	QDataStream stream ( data, IO_ReadOnly );

	if ( msg == "setScreenSaverInterval(int)" ) {
		int time;
		stream >> time;
		m_screensaver-> setInterval( time );
	}
	else if ( msg == "setScreenSaverIntervals(int,int,int)" ) {
		int t1, t2, t3;
		stream >> t1 >> t2 >> t3;
		m_screensaver-> setIntervals( t1, t2, t3 );
	}
	else if ( msg == "setBacklight(int)" ) {
		int bright;
		stream >> bright;
		m_screensaver-> setBacklight( bright );
	}
	else if ( msg == "setScreenSaverMode(int)" ) {
		int mode;
		stream >> mode;
		m_screensaver-> setMode ( mode );
	}
	else if ( msg == "reloadPowerWarnSettings()" ) {
		reloadPowerWarnSettings();
	}
	else if ( msg == "setDisplayState(int)" ) {
		int state;
		stream >> state;
		m_screensaver-> setDisplayState ( state != 0 );
	}
	else if ( msg == "suspend()" ) {
		emit power();
	}
	else if ( msg == "sendBusinessCard()" ) {
		QString card = ::getenv ( "HOME" );
		card += "/Applications/addressbook/businesscard.vcf";

		if ( QFile::exists( card ) ) {
			QCopEnvelope e ( "QPE/Obex", "send(QString,QString,QString)" );
			QString mimetype = "text/x-vCard";
			e << tr( "business card" ) << card << mimetype;
		}
	}
}

void DesktopApplication::reloadPowerWarnSettings()
{
	Config cfg( "apm" );
	cfg.setGroup( "Warnings" );

	//  m_timer->changeInterval( cfg.readNumEntry( "checkinterval", 10000 ) );
	m_powerVeryLow = cfg.readNumEntry( "powerverylow", 10 );
	m_powerCritical = cfg.readNumEntry( "powervcritical", 5 );
}


enum MemState { Unknown, VeryLow, Low, Normal } memstate = Unknown;


void DesktopApplication::launcherMessage( const QCString & msg, const QByteArray & data )
{
	QDataStream stream ( data, IO_ReadOnly );

	if ( msg == "deviceButton(int,int,int)" ) {
		int keycode, press, autoRepeat;
		stream >> keycode >> press >> autoRepeat;
		
		const ODeviceButton *db = ODevice::inst ( )-> buttonForKeycode ( keycode );
		
		if ( db )
	    	checkButtonAction ( db, keycode, press, autoRepeat );
	}
	else if ( msg == "keyRegister(int,QCString,QCString)" ) {
		int k;
		QCString c, m;
		stream >> k >> c >> m;

		keyRegisterList.append ( QCopKeyRegister ( k, c, m ));
	}
}

void DesktopApplication::sendHeldAction ( )
{
	if ( m_last_button ) {
		m_last_button-> heldAction ( ). send ( );
		m_last_button = 0;
	}
}



void DesktopApplication::checkButtonAction ( const ODeviceButton *db, int keycode, bool press, bool autoRepeat )
{		
	if ( db ) {
		if ( !press && !autoRepeat && m_button_timer-> isActive ( )) {
			m_button_timer-> stop ( );
			if ( !db-> pressedAction ( ). channel ( ). isEmpty ( )) {
				db-> pressedAction ( ). send ( );
			}
		}
		else if ( press && !autoRepeat ) {
			m_button_timer-> stop ( );
			
			if ( !db-> heldAction ( ). channel ( ). isEmpty ( )) {
				m_last_button = db;
				m_button_timer-> start ( ODevice::inst ( )-> buttonHoldTime ( ), true );
			}
		}
	}
}

bool DesktopApplication::eventFilter ( QObject *o, QEvent *e )
{
	if ( e-> type ( ) == QEvent::KeyPress || e-> type ( ) == QEvent::KeyRelease ) {
		QKeyEvent *ke = (QKeyEvent *) e;
	
		const ODeviceButton *db = ODevice::inst ( )-> buttonForKeycode ( ke-> key ( ));
        
		if ( db ) {
			checkButtonAction ( db, ke-> key ( ), e-> type ( ) == QEvent::KeyPress, ke-> isAutoRepeat ( ));
			return true;
		}
	}
	return QPEApplication::eventFilter ( o, e );
}

#ifdef Q_WS_QWS

bool DesktopApplication::qwsEventFilter( QWSEvent *e )
{
	qpedesktop->checkMemory();

	if ( e->type == QWSEvent::Key ) {
		QWSKeyEvent * ke = (QWSKeyEvent *) e;
		ushort keycode = ke-> simpleData. keycode;

		if ( !loggedin && keycode != Key_F34 )
			return true;
			
		bool press = ke-> simpleData. is_press;
		bool autoRepeat = ke-> simpleData. is_auto_repeat;

		if ( !keyboardGrabbed ( )) {
			// app that registers key/message to be sent back to the app, when it doesn't have focus,
			// when user presses key, unless keyboard has been requested from app.
			// will not send multiple repeats if user holds key
			// i.e. one shot
		 
			if ( keycode != 0 && press && !autoRepeat ) {
				for ( KeyRegisterList::Iterator it = keyRegisterList.begin(); it != keyRegisterList.end(); ++it ) {
					if (( *it ). getKeyCode ( ) == keycode ) {
						QCopEnvelope (( *it ). getChannel ( ), ( *it ). getMessage ( ));
						return true;
					}
				}
			}
		}

		if ( keycode == HardKey_Suspend ) {
			if ( press )
				emit power ( );
			return true;
		}
		else if ( keycode == HardKey_Backlight ) {
			if ( press )
				emit backlight ( );
			return true;
		}
		else if ( keycode == Key_F32 ) {
			if ( press )
				QCopEnvelope e( "QPE/Desktop", "startSync()" );
			return true;
		}
		else if ( keycode == Key_F31 && !ke-> simpleData. modifiers ) { // Symbol Key -> show Unicode IM
			if ( press )
				emit symbol ( );
			return true;
		}
		else if ( keycode == Key_NumLock ) {
			if ( press )
				emit numLockStateToggle ( );
		}
		else if ( keycode == Key_CapsLock ) {
			if ( press )
				emit capsLockStateToggle();
		}
		if (( press && !autoRepeat ) || ( !press && autoRepeat )) {
			if ( m_keyclick_sound )
				ODevice::inst ( )-> keySound ( );
		}
	}
	else if ( e-> type == QWSEvent::Mouse ) {
		QWSMouseEvent * me = ( QWSMouseEvent * ) e;
		static bool up = true;

		if ( me-> simpleData. state & LeftButton ) {
			if ( up ) {
				up = false;
				if ( m_screentap_sound )
					ODevice::inst ( ) -> touchSound ( );
			}
		}
		else {
			up = true;
		}
	}

	return QPEApplication::qwsEventFilter ( e );
}
#endif



#if defined(QPE_HAVE_MEMALERTER)
QPE_MEMALERTER_IMPL
#endif

//===========================================================================

Desktop::Desktop() :
		QWidget( 0, 0, WStyle_Tool | WStyle_Customize ),
		qcopBridge( 0 ),
		transferServer( 0 ),
		packageSlave( 0 )
{
	qpedesktop = this;

	//    bg = new Info( this );
	tb = new TaskBar;

	launcher = new Launcher( 0, 0, WStyle_Customize | QWidget::WGroupLeader );

	connect( launcher, SIGNAL( busy() ), tb, SLOT( startWait() ) );
	connect( launcher, SIGNAL( notBusy( const QString& ) ), tb, SLOT( stopWait( const QString& ) ) );

	int displayw = qApp->desktop() ->width();
	int displayh = qApp->desktop() ->height();


	QSize sz = tb->sizeHint();

	setGeometry( 0, displayh - sz.height(), displayw, sz.height() );
	tb->setGeometry( 0, displayh - sz.height(), displayw, sz.height() );

	tb->show();
	launcher->showMaximized();
	launcher->show();
	launcher->raise();
#if defined(QPE_HAVE_MEMALERTER)

	initMemalerter();
#endif
	// start services
	startTransferServer();
	( void ) new IrServer( this );

	packageSlave = new PackageSlave( this );

	qApp->installEventFilter( this );

	qApp-> setMainWidget ( launcher );
}

void Desktop::show()
{
	login( TRUE );
	QWidget::show();
}

Desktop::~Desktop()
{
	delete launcher;
	delete tb;
	delete qcopBridge;
	delete transferServer;
}

bool Desktop::recoverMemory()
{
	return tb->recoverMemory();
}

void Desktop::checkMemory()
{
#if defined(QPE_HAVE_MEMALERTER)
	static bool ignoreNormal = FALSE;
	static bool existingMessage = FALSE;

	if ( existingMessage )
		return ; // don't show a second message while still on first

	existingMessage = TRUE;
	switch ( memstate ) {
		case Unknown:
			break;
		case Low:
			memstate = Unknown;
			if ( recoverMemory() )
				ignoreNormal = TRUE;
			else
				QMessageBox::warning( 0 , "Memory Status",
				                      "The memory smacks of shortage. \n"
				                      "Please save data. " );
			break;
		case Normal:
			memstate = Unknown;
			if ( ignoreNormal )
				ignoreNormal = FALSE;
			//       else
			//         QMessageBox::information ( 0 , "Memory Status",
			//                                    "There is enough memory again." );
			break;
		case VeryLow:
			memstate = Unknown;
			QMessageBox::critical( 0 , "Memory Status",
			                       "The memory is very low. \n"
			                       "Please end this application \n"
			                       "immediately." );
			recoverMemory();
	}
	existingMessage = FALSE;
#endif
}

static bool isVisibleWindow( int wid )
{
#ifdef QWS
	const QList<QWSWindow> &list = qwsServer->clientWindows();
	QWSWindow* w;
	for ( QListIterator<QWSWindow> it( list ); ( w = it.current() ); ++it ) {
		if ( w->winId() == wid )
			return !w->isFullyObscured();
	}
#endif
	return FALSE;
}

static bool hasVisibleWindow( const QString& clientname )
{
#ifdef QWS
	const QList<QWSWindow> &list = qwsServer->clientWindows();
	QWSWindow* w;
	for ( QListIterator<QWSWindow> it( list ); ( w = it.current() ); ++it ) {
		if ( w->client() ->identity() == clientname && !w->isFullyObscured() )
			return TRUE;
	}
#endif
	return FALSE;
}


void Desktop::executeOrModify( const QString& appLnkFile )
{
	AppLnk lnk( MimeType::appsFolderName() + "/" + appLnkFile );
	if ( lnk.isValid() ) {
		QCString app = lnk.exec().utf8();
		Global::terminateBuiltin( "calibrate" );
		if ( QCopChannel::isRegistered( "QPE/Application/" + app ) ) {
			//  MRUList::addTask( &lnk );
			if ( hasVisibleWindow( app ) )
				QCopChannel::send( "QPE/Application/" + app, "nextView()" );
			else
				QCopChannel::send( "QPE/Application/" + app, "raise()" );
		}
		else {
			lnk.execute();
		}
	}
}

// autoStarts apps on resume and start
void Desktop::execAutoStart()
{
	QString appName;
	int delay;
	QDateTime now = QDateTime::currentDateTime();
	Config cfg( "autostart" );
	cfg.setGroup( "AutoStart" );
	appName = cfg.readEntry( "Apps", "" );
	delay = ( cfg.readEntry( "Delay", "0" ) ).toInt();
	// If the time between suspend and resume was longer then the
	// value saved as delay, start the app
	if ( suspendTime.secsTo( now ) >= ( delay * 60 ) && !appName.isEmpty() ) {
		QCopEnvelope e( "QPE/System", "execute(QString)" );
		e << QString( appName );
	}
}

#if defined(QPE_HAVE_TOGGLELIGHT)
#include <qpe/config.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <time.h>
#endif


void Desktop::togglePower()
{
	static bool excllock = false;

	qDebug ( "togglePower (locked == %d)", excllock ? 1 : 0 );

	if ( excllock )
		return ;

	excllock = true;

	bool wasloggedin = loggedin;
	loggedin = 0;
	suspendTime = QDateTime::currentDateTime();

#ifdef QWS

	if ( Password::needToAuthenticate ( true ) && qt_screen ) {
		// Should use a big black window instead.
		// But this would not show up fast enough
		QGfx *g = qt_screen-> screenGfx ( );
		g-> fillRect ( 0, 0, qt_screen-> width ( ), qt_screen-> height ( ));
		delete g;
	}
#endif

	ODevice::inst ( )-> suspend ( );

	DesktopApplication::switchLCD ( true ); // force LCD on without slow qcop call
	QWSServer::screenSaverActivate ( false );

	{
		QCopEnvelope( "QPE/Card", "mtabChanged()" ); // might have changed while asleep
	}

	if ( wasloggedin )
		login ( true );

	execAutoStart();
	//qcopBridge->closeOpenConnections();

	excllock = false;
}

void Desktop::toggleLight()
{
	QCopEnvelope e( "QPE/System", "setBacklight(int)" );
	e << -2; // toggle
}

void Desktop::toggleSymbolInput()
{
	tb->toggleSymbolInput();
}

void Desktop::toggleNumLockState()
{
	tb->toggleNumLockState();
}

void Desktop::toggleCapsLockState()
{
	tb->toggleCapsLockState();
}

void Desktop::styleChange( QStyle &s )
{
	QWidget::styleChange( s );
	int displayw = qApp->desktop() ->width();
	int displayh = qApp->desktop() ->height();

	QSize sz = tb->sizeHint();

	tb->setGeometry( 0, displayh - sz.height(), displayw, sz.height() );
}

void DesktopApplication::shutdown()
{
	if ( type() != GuiServer )
		return ;
	ShutdownImpl *sd = new ShutdownImpl( 0, 0, WDestructiveClose );
	connect( sd, SIGNAL( shutdown( ShutdownImpl::Type ) ),
	         this, SLOT( shutdown( ShutdownImpl::Type ) ) );
	sd->showMaximized();
}

void DesktopApplication::shutdown( ShutdownImpl::Type t )
{
	switch ( t ) {
		case ShutdownImpl::ShutdownSystem:
			execlp( "shutdown", "shutdown", "-h", "now", ( void* ) 0 );
			break;
		case ShutdownImpl::RebootSystem:
			execlp( "shutdown", "shutdown", "-r", "now", ( void* ) 0 );
			break;
		case ShutdownImpl::RestartDesktop:
			restart();
			break;
		case ShutdownImpl::TerminateDesktop:
			prepareForTermination( FALSE );

			// This is a workaround for a Qt bug
			// clipboard applet has to stop its poll timer, or Qt/E
			// will hang on quit() right before it emits aboutToQuit()
			emit aboutToQuit ( );

			quit();
			break;
	}
}

void DesktopApplication::restart()
{
	prepareForTermination( TRUE );

#ifdef Q_WS_QWS

	for ( int fd = 3; fd < 100; fd++ )
		close( fd );
#if defined(QT_DEMO_SINGLE_FLOPPY)

	execl( "/sbin/init", "qpe", 0 );
#elif defined(QT_QWS_CASSIOPEIA)

	execl( "/bin/sh", "sh", 0 );
#else

	execl( ( qpeDir() + "/bin/qpe" ).latin1(), "qpe", 0 );
#endif

	exit( 1 );
#endif
}

void Desktop::startTransferServer()
{
	// start qcop bridge server
	qcopBridge = new QCopBridge( 4243 );
	if ( !qcopBridge->ok() ) {
		delete qcopBridge;
		qcopBridge = 0;
	}
	// start transfer server
	transferServer = new TransferServer( 4242 );
	if ( !transferServer->ok() ) {
		delete transferServer;
		transferServer = 0;
	}
	if ( !transferServer || !qcopBridge )
		startTimer( 2000 );
}

void Desktop::timerEvent( QTimerEvent *e )
{
	killTimer( e->timerId() );
	startTransferServer();
}

void Desktop::terminateServers()
{
	delete transferServer;
	delete qcopBridge;
	transferServer = 0;
	qcopBridge = 0;
}

void DesktopApplication::rereadVolumes()
{
	Config cfg( "qpe" );
	cfg. setGroup ( "Volume" );
	
	m_screentap_sound = cfg. readBoolEntry ( "TouchSound" );
	m_keyclick_sound  = cfg. readBoolEntry ( "KeySound" );
	m_alarm_sound     = cfg. readBoolEntry ( "AlarmSound" );
}

void DesktopApplication::soundAlarm()
{
	if ( me ( )-> m_alarm_sound )
		ODevice::inst ( )-> alarmSound ( );
}

DesktopApplication *DesktopApplication::me ( )
{
	return (DesktopApplication *) qApp;
}

