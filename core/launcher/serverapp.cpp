/**********************************************************************
** Copyright (C) 2000-2003 Trolltech AS.  All rights reserved.
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

#include "serverapp.h"

#include <qtopia/password.h>
#include <qtopia/config.h>
#include <qtopia/power.h>
#include <qtopia/devicebuttonmanager.h>
#include <qtopia/pluginloader.h>

#ifdef Q_WS_QWS
#include <qtopia/qcopenvelope_qws.h>
#endif
#include <qtopia/global.h>
#include <qtopia/custom.h>

#ifdef Q_WS_QWS
#include <qgfx_qws.h>
#endif
#ifdef Q_OS_WIN32
#include <io.h>
#include <process.h>
#else
#include <unistd.h>
#endif
#include <qmessagebox.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qfile.h>
#include <qpixmapcache.h>

#include <stdlib.h>

static ServerApplication *serverApp = 0;
static int loggedin=0;



/*  Apply light/power settings for current power source */
static void applyLightSettings(PowerStatus *p)
{
    int initbright, intervalDim, intervalLightOff, intervalSuspend;
    bool dim, lightoff, suspend;

    {
	Config config("qpe");
	bool defsus;
	if ( p->acStatus() == PowerStatus::Online ) {
	    config.setGroup("ExternalPower");
	    defsus = FALSE;
	} else {
	    config.setGroup("BatteryPower");
	    defsus = TRUE;
	}

	intervalDim = config.readNumEntry( "Interval_Dim", 20 );
	intervalLightOff = config.readNumEntry("Interval_LightOff", 30);
	intervalSuspend = config.readNumEntry("Interval", 240);
	initbright = config.readNumEntry("Brightness", 255);
	dim = config.readBoolEntry("Dim", TRUE);
	lightoff = config.readBoolEntry("LightOff", FALSE );
	suspend = config.readBoolEntry("Suspend", defsus );

	/*	For compability	*/
	config.setGroup("Screensaver");
	config.writeEntry( "Dim", dim );
	config.writeEntry( "LightOff", lightoff );
	config.writeEntry( "Suspend", suspend );
	config.writeEntry( "Interval_Dim", intervalDim );
	config.writeEntry( "Interval_LightOff", intervalLightOff );
	config.writeEntry( "Interval", intervalSuspend );
	config.writeEntry( "Brightness", initbright );
    }

    int i_dim =      (dim ? intervalDim : 0);
    int i_lightoff = (lightoff ? intervalLightOff : 0);
    int i_suspend =  (suspend ? intervalSuspend : 0);

#ifndef QT_NO_COP
    QCopEnvelope eB("QPE/System", "setBacklight(int)" );
    eB << initbright;

    QCopEnvelope e("QPE/System", "setScreenSaverIntervals(int,int,int)" );
    e << i_dim << i_lightoff << i_suspend;
#endif
}

//---------------------------------------------------------------------------

/*
  Priority is number of alerts that are needed to pop up
  alert.
 */
class DesktopPowerAlerter : public QMessageBox
{
    Q_OBJECT
public:
    DesktopPowerAlerter( QWidget *parent, const char *name = 0 )
	: QMessageBox( tr("Battery Status"), tr("Low Battery"),
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
	return;
    if ( priority > currentPriority )
	return;
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

//---------------------------------------------------------------------------

KeyFilter::KeyFilter(QObject* parent) : QObject(parent), held_tid(0), heldButton(0)
{
    qwsServer->setKeyboardFilter(this);
}

void KeyFilter::timerEvent(QTimerEvent* e)
{
    if ( e->timerId() == held_tid ) {
	killTimer(held_tid);
	// button held
	if ( heldButton ) {
	    emit activate(heldButton, TRUE);
	    heldButton = 0;
	}
	held_tid = 0;
    }
}

bool KeyFilter::filter(int /*unicode*/, int keycode, int modifiers, bool press,
		  bool autoRepeat)
{
    if ( !loggedin
	    // Permitted keys
	    && keycode != Key_F34 // power
	    && keycode != Key_F30 // select
	    && keycode != Key_Enter
	    && keycode != Key_Return
	    && keycode != Key_Space
	    && keycode != Key_Left
	    && keycode != Key_Right
	    && keycode != Key_Up
	    && keycode != Key_Down )
	return TRUE;
    if ( !modifiers ) {
	if ( !((ServerApplication*)qApp)->keyboardGrabbed() ) {
	    // First check to see if DeviceButtonManager knows something about this button:
	    const DeviceButton* button = DeviceButtonManager::instance().buttonForKeycode(keycode);
	    if (button && !autoRepeat) {
		if ( held_tid ) {
		    killTimer(held_tid);
		    held_tid = 0;
		}
		if ( button->heldAction().isNull() ) {
		    if ( press )
			emit activate(button, FALSE);
		} else if ( press ) {
		    heldButton = button;
		    held_tid = startTimer(500);
		} else if ( heldButton ) {
		    heldButton = 0;
		    emit activate(button, FALSE);
		}
		QWSServer::screenSaverActivate(FALSE);
		return TRUE;
	    }
	}
	if ( keycode == Key_F34 ) {
	    if ( press ) emit power();
	    return TRUE;
	}
	if ( keycode == Key_F35 ) {
	    if ( press ) emit backlight();
	    return TRUE;
	}
	if ( keycode == Key_F32 ) {
#ifndef QT_NO_COP
	    if ( press ) QCopEnvelope e( "QPE/Desktop", "startSync()" );
#endif
	    return TRUE;
	}
	if ( keycode == Key_F31 ) {
	    if ( press ) emit symbol();
	    QWSServer::screenSaverActivate(FALSE);
	    return TRUE;
	}
    }
    if ( keycode == Key_NumLock ) {
	if ( press ) emit numLockStateToggle();
    }
    if ( keycode == Key_CapsLock ) {
	if ( press ) emit capsLockStateToggle();
    }
    if ( serverApp )
	serverApp->keyClick(keycode,press,autoRepeat);
    return FALSE;
}

enum MemState { MemUnknown, MemVeryLow, MemLow, MemNormal } memstate=MemUnknown;

#if defined(QPE_HAVE_MEMALERTER)
QPE_MEMALERTER_IMPL
#endif

#if defined(CUSTOM_SOUND_IMPL)
CUSTOM_SOUND_IMPL
#endif

//---------------------------------------------------------------------------

bool ServerApplication::doRestart = FALSE;
bool ServerApplication::allowRestart = TRUE;

ServerApplication::ServerApplication( int& argc, char **argv, Type t )
    : QPEApplication( argc, argv, t )
{
#ifdef CUSTOM_SOUND_INIT
    CUSTOM_SOUND_INIT;
#endif

#if defined(QPE_HAVE_MEMALERTER)
    initMemalerter();
#endif

    // We know we'll have lots of cached pixmaps due to App/DocLnks
    QPixmapCache::setCacheLimit(512);

    QTimer *timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(psTimeout()) );
    timer->start( 10000 );
    ps = new PowerStatus;
    pa = new DesktopPowerAlerter( 0 );

    applyLightSettings(ps);

    if ( PluginLoader::inSafeMode() )
	QTimer::singleShot(500, this, SLOT(showSafeMode()) );
    QTimer::singleShot(20*1000, this, SLOT(clearSafeMode()) );

    KeyFilter* kf = new KeyFilter(this);

    connect( kf, SIGNAL(launch()), this, SIGNAL(launch()) );
    connect( kf, SIGNAL(power()), this, SIGNAL(power()) );
    connect( kf, SIGNAL(backlight()), this, SIGNAL(backlight()) );
    connect( kf, SIGNAL(symbol()), this, SIGNAL(symbol()));
    connect( kf, SIGNAL(numLockStateToggle()), this,SIGNAL(numLockStateToggle()));
    connect( kf, SIGNAL(capsLockStateToggle()), this,SIGNAL(capsLockStateToggle()));
    connect( kf, SIGNAL(activate(const DeviceButton*,bool)), this,SIGNAL(activate(const DeviceButton*,bool)));

    connect( kf, SIGNAL(power()), this, SLOT(togglePower()) );
    connect( kf, SIGNAL(backlight()), this, SLOT(toggleLight()) );

    connect( this, SIGNAL(volumeChanged(bool)), this, SLOT(rereadVolumes()) );
    rereadVolumes();

    serverApp = this;
}


ServerApplication::~ServerApplication()
{
    delete ps;
    delete pa;
}

bool ServerApplication::screenLocked()
{
    return loggedin == 0;
}

void ServerApplication::login(bool at_poweron)
{
    if ( !loggedin ) {
	Global::terminateBuiltin("calibrate"); // No tr
	Password::authenticate(at_poweron);
	loggedin=1;
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "unlocked()" );
#endif
    }
}

#if defined(QPE_HAVE_TOGGLELIGHT)
#include <qtopia/config.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <time.h>
#endif

static bool blanked=FALSE;

static void blankScreen()
{
#ifdef QWS
    QWidget w(0, 0, Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WStyle_StaysOnTop | Qt::WPaintUnclipped);
    w.resize( qt_screen->width(), qt_screen->height() );
    w.move(0, 0);
    
    QPainter p(&w);
    p.fillRect(w.rect(), QBrush(QColor(255,255,255)) );
    p.end();
    w.repaint();

    blanked = TRUE;
#endif
}

static void darkScreen()
{
    qpe_setBacklight(0); // force off
}


void ServerApplication::togglePower()
{
    static int haveAPM = -1;
    if ( haveAPM < 0 ) {
	if ( QFile::exists( "/proc/apm" ) ) {
	    haveAPM = 1;
	} else {
	    haveAPM = 0;
	    qWarning( "Cannot suspend - no APM support in kernel" );
	}
    }
    
    if ( haveAPM ) {
	bool wasloggedin = loggedin;
	loggedin=0;
	if ( wasloggedin ) {
	    Config cfg( QPEApplication::qpeDir()+"/etc/Security.conf", Config::File);
	    cfg.setGroup("Passcode");
	    QString passcode = cfg.readEntry("passcode");
	    if ( !passcode.isEmpty() && cfg.readNumEntry("passcode_poweron",0) )
		blankScreen();
	}
	
	system("apm --suspend");
    
#ifndef QT_NO_COP
	QWSServer::screenSaverActivate( FALSE );
	{
	    QCopEnvelope("QPE/Card", "mtabChanged()" ); // might have changed while asleep
	    QCopEnvelope e("QPE/System", "setBacklight(int)");
	    e << -3; // Force on
	}
#endif
	if ( wasloggedin )
	    login(TRUE);
    } 
    
    //qcopBridge->closeOpenConnections();
    //qDebug("called togglePower()!!!!!!");
}

void ServerApplication::toggleLight()
{
#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "setBacklight(int)");
    e << -2; // toggle
#endif
}


#ifdef Q_WS_QWS
bool ServerApplication::qwsEventFilter( QWSEvent *e )
{
    checkMemory();

    if ( e->type == QWSEvent::Mouse ) {
	QWSMouseEvent *me = (QWSMouseEvent *)e;
	static bool up = TRUE;
	if ( me->simpleData.state&LeftButton ) {
	    if ( up ) {
		up = FALSE;
		screenClick(TRUE);
	    }
	} else if ( !up ) {
	    up = TRUE;
	    screenClick(FALSE);
	}
    }

    return QPEApplication::qwsEventFilter( e );
}
#endif

void ServerApplication::psTimeout()
{
    checkMemory(); // in case no events are being generated

    PowerStatus::ACStatus oldStatus = ps->acStatus();

    *ps = PowerStatusManager::readStatus();

    if ( oldStatus != ps->acStatus() ) {
	// power source changed, read settings applying to current powersource
	applyLightSettings(ps);
    }


    if ( (ps->batteryStatus() == PowerStatus::VeryLow ) ) {
	pa->alert( tr( "Battery is running very low." ), 6 );
    }

    if (  ps->batteryStatus() == PowerStatus::Critical ) {
	pa->alert(  tr( "Battery level is critical!\n"
			"Please recharge the main battery!" ), 1 );
    }

    if (  ps->backupBatteryStatus() == PowerStatus::VeryLow ) {
	pa->alert( tr( "The Back-up battery is very low.\nPlease charge the back-up battery." ), 3 );
    }
}

void ServerApplication::showSafeMode()
{
    if ( QMessageBox::warning(0, tr("Safe Mode"), tr("<P>A system startup error occurred, "
		"and the system is now in Safe Mode. "
		"Plugins are not loaded in Safe Mode. "
		"You can use the Plugin Manager to "
		"disable plugins that cause system error."), tr("OK"), tr("Plugin Manager..."), 0) == 1 ) {
	Global::execute( "pluginmanager" );
    }
}

void ServerApplication::clearSafeMode()
{
    // If we've been running OK for a while then we won't bother going into
    // safe mode immediately on the next crash.
    Config cfg( "PluginLoader" );
    cfg.setGroup( "Global" );
    QString mode = cfg.readEntry( "Mode", "Normal" );
    if ( mode == "MaybeSafe" ) {
	cfg.writeEntry( "Mode", "Normal" );
    }
}

void ServerApplication::shutdown()
{
    if ( type() != GuiServer )
	return;
    ShutdownImpl *sd = new ShutdownImpl( 0, 0, WDestructiveClose );
    connect( sd, SIGNAL(shutdown(ShutdownImpl::Type)),
	     this, SLOT(shutdown(ShutdownImpl::Type)) );
    sd->showMaximized();
}

void ServerApplication::shutdown( ShutdownImpl::Type t )
{
    switch ( t ) {
	case ShutdownImpl::ShutdownSystem:
#ifndef Q_OS_WIN32
	    execlp("shutdown", "shutdown", "-h", "now", (void*)0); // No tr
#else
	    qDebug("ServerApplication::ShutdownSystem");      
	    prepareForTermination(FALSE);
	    quit();
#endif
	    break;

	case ShutdownImpl::RebootSystem:
#ifndef Q_OS_WIN32
	    execlp("shutdown", "shutdown", "-r", "now", (void*)0); // No tr
#else
	    qDebug("ServerApplication::RebootSystem");      
	    restart();
#endif
	    break;

	case ShutdownImpl::RestartDesktop:
	    restart();
	    break;

	case ShutdownImpl::TerminateDesktop:
	    prepareForTermination(FALSE);
	    quit();
	    break;
    }
}

void ServerApplication::restart()
{
    if ( allowRestart ) {
	prepareForTermination(TRUE);
	doRestart = TRUE;
	quit();
    }
}

void ServerApplication::rereadVolumes()
{
    Config cfg("Sound");
    cfg.setGroup("System");
    touchclick = cfg.readBoolEntry("Touch");
    keyclick = cfg.readBoolEntry("Key");
}


void ServerApplication::checkMemory()
{
#if defined(QPE_HAVE_MEMALERTER)
    static bool ignoreNormal=TRUE;
    static bool existingMessage=FALSE;

    if(existingMessage)
	return; // don't show a second message while still on first

    existingMessage = TRUE;
    switch ( memstate ) {
	case MemUnknown:
	    break;
	case MemLow:
	    memstate = MemUnknown;
	    if ( !recoverMemory() ) {
		QMessageBox::warning( 0 , tr("Memory Status"),
		    tr("Memory Low\nPlease save data.") );
		ignoreNormal = FALSE;
	    }
	    break;
	case MemNormal:
	    memstate = MemUnknown;
	    if ( !ignoreNormal ) {
		ignoreNormal = TRUE;
		QMessageBox::information ( 0 , tr("Memory Status"),
		    "Memory OK" );
	    }
	    break;
	case MemVeryLow:
	    memstate = MemUnknown;
	    QMessageBox::critical( 0 , tr("Memory Status"),
		tr("Critical Memory Shortage\n"
		"Please end this application\n"
		"immediately.") );
	    recoverMemory();
    }
    existingMessage = FALSE;
#endif
}

bool ServerApplication::recoverMemory()
{
    return FALSE;
}

void ServerApplication::keyClick(int keycode, bool press, bool repeat)
{
#ifdef CUSTOM_SOUND_KEYCLICK
    if ( keyclick )
	CUSTOM_SOUND_KEYCLICK(keycode,press,repeat);
#else
    Q_UNUSED( keycode );
    Q_UNUSED( press );
    Q_UNUSED( repeat );
#endif
}

void ServerApplication::screenClick(bool press)
{
#ifdef CUSTOM_SOUND_TOUCH
    if ( touchclick )
	CUSTOM_SOUND_TOUCH(press);
#else
    Q_UNUSED( press );
#endif
}


#include "serverapp.moc"
