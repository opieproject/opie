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
#include "mrulist.h"
#include "qcopbridge.h"
#include "shutdownimpl.h"
#include "startmenu.h"
#include "taskbar.h"
#include "transferserver.h"
#include "irserver.h"
#include "packageslave.h"

#include <qpe/applnk.h>
#include <qpe/mimetype.h>
#include <qpe/password.h>
#include <qpe/config.h>
#include <qpe/power.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>
#ifdef QT_QWS_CUSTOM
#include "qpe/custom.h"
#endif

#include <qgfx_qws.h>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qwindowsystem_qws.h>

#include <qvaluelist.h>

#include <stdlib.h>
#include <unistd.h>

class QCopKeyRegister
{
public:
  QCopKeyRegister() : keyCode(0) { }
  QCopKeyRegister(int k, const QString &c, const QString &m) 
	: keyCode(k), channel(c), message(m) { }

  int getKeyCode() const { return keyCode; }
  QString getChannel() const { return channel; }
  QString getMessage() const { return message; }

private:
  int keyCode;
  QString channel, message;
};

typedef QValueList<QCopKeyRegister> KeyRegisterList;
KeyRegisterList keyRegisterList;

static Desktop* qpedesktop = 0;
static int loggedin=0;
static void login(bool at_poweron)
{
    if ( !loggedin ) {
	Global::terminateBuiltin("calibrate");
	Password::authenticate(at_poweron);
	loggedin=1;
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
	: QMessageBox( tr("Battery Status"), "Low Battery",
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



DesktopApplication::DesktopApplication( int& argc, char **argv, Type t )
    : QPEApplication( argc, argv, t )
{

    QTimer *t = new QTimer( this );
    connect( t, SIGNAL(timeout()), this, SLOT(psTimeout()) );
    t->start( 10000 );
    ps = new PowerStatus;
    pa = new DesktopPowerAlerter( 0 );

  channel = new QCopChannel( "QPE/Desktop", this );
  connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
		   this, SLOT(receive(const QCString&, const QByteArray&)) );
}


DesktopApplication::~DesktopApplication()
{
    delete ps;
    delete pa;
}

void DesktopApplication::receive( const QCString &msg, const QByteArray &data )
{
  QDataStream stream( data, IO_ReadOnly );
  if (msg == "keyRegister(int key, QString channel, QString message)")
	{
	  int k;
	  QString c, m;

	  stream >> k;
	  stream >> c;
	  stream >> m;
	  
	  qWarning("KeyRegisterRecieved: %i, %s, %s", k, (const char*)c, (const char *)m);
	  keyRegisterList.append(QCopKeyRegister(k,c,m));
	} 
  else if (msg == "suspend()"){
    emit power();
  }
  
}

enum MemState { Unknown, VeryLow, Low, Normal } memstate=Unknown;

#ifdef Q_WS_QWS
bool DesktopApplication::qwsEventFilter( QWSEvent *e )
{
    qpedesktop->checkMemory();

    if ( e->type == QWSEvent::Key ) {
	  QWSKeyEvent *ke = (QWSKeyEvent *)e;
	  if ( !loggedin && ke->simpleData.keycode != Key_F34 )
	    return TRUE;
	  bool press = ke->simpleData.is_press;

	  if (!keyRegisterList.isEmpty())
		{
		  KeyRegisterList::Iterator it;
		  for( it = keyRegisterList.begin(); it != keyRegisterList.end(); ++it )
			{
			  if ((*it).getKeyCode() == ke->simpleData.keycode)
				QCopEnvelope((*it).getChannel().utf8(), (*it).getMessage().utf8());
			}
		}

	  if ( !keyboardGrabbed() ) {
	    if ( ke->simpleData.keycode == Key_F9 ) {
		  if ( press ) emit datebook();
		  return TRUE;
	    }
	    if ( ke->simpleData.keycode == Key_F10 ) {
	      if ( !press && cardSendTimer ) {
		    emit contacts();
		    delete cardSendTimer;
		  } else if ( press ) {
		    cardSendTimer = new QTimer();
		    cardSendTimer->start( 2000, TRUE );
		    connect( cardSendTimer, SIGNAL( timeout() ), this, SLOT( sendCard() ) );
		  }
		  return TRUE;
	    }
	    /* menu key now opens application menu/toolbar
		   if ( ke->simpleData.keycode == Key_F11 ) {
		   if ( press ) emit menu();
		   return TRUE;
		   }
	    */
	    if ( ke->simpleData.keycode == Key_F12 ) {
		  while( activePopupWidget() )
		    activePopupWidget()->close();
		  if ( press ) emit launch();
		  return TRUE;
	    }
	    if ( ke->simpleData.keycode == Key_F13 ) {
		  if ( press ) emit email();
		  return TRUE;
	    }
	  }

	  if ( ke->simpleData.keycode == Key_F34 ) {
	    if ( press ) emit power();
	    return TRUE;
	  }
	  if ( ke->simpleData.keycode == Key_SysReq ) {
	    if ( press ) emit power();
	    return TRUE;
	  }
	  if ( ke->simpleData.keycode == Key_F35 ) {
	    if ( press ) emit backlight();
	    return TRUE;
	  }
	  if ( ke->simpleData.keycode == Key_F32 ) {
	    if ( press ) QCopEnvelope e( "QPE/Desktop", "startSync()" );
	    return TRUE;
	  }
	  if ( ke->simpleData.keycode == Key_F31 && !ke->simpleData.modifiers ) {
	    if ( press ) emit symbol();
	    return TRUE;
	  }
	  if ( ke->simpleData.keycode == Key_NumLock ) {
	    if ( press ) emit numLockStateToggle();
	  }
	  if ( ke->simpleData.keycode == Key_CapsLock ) {
	    if ( press ) emit capsLockStateToggle();
	  }
	  if ( press )
	    qpedesktop->keyClick();
    } else {
	  if ( e->type == QWSEvent::Mouse ) {
	    QWSMouseEvent *me = (QWSMouseEvent *)e;
	    static bool up = TRUE;
	    if ( me->simpleData.state&LeftButton ) {
		  if ( up ) {
		    up = FALSE;
		    qpedesktop->screenClick();
		  }
	    } else {
		  up = TRUE;
	    }
	  }
    }
	
    return QPEApplication::qwsEventFilter( e );
}
#endif

void DesktopApplication::psTimeout()
{
    qpedesktop->checkMemory(); // in case no events are being generated

    *ps = PowerStatusManager::readStatus();

    if ( (ps->batteryStatus() == PowerStatus::VeryLow ) ) {
	pa->alert( tr( "Battery is running very low." ), 6 );
    }

    if (  ps->batteryStatus() == PowerStatus::Critical ) {
	pa->alert(  tr( "Battery level is critical!\n"
			"Keep power off until power restored!" ), 1 );
    }

    if (  ps->backupBatteryStatus() == PowerStatus::VeryLow ) {
	pa->alert( tr( "The Back-up battery is very low.\nPlease charge the back-up battery." ), 3 );
    }
}


void DesktopApplication::sendCard()
{
    delete cardSendTimer;
    cardSendTimer = 0;
    QString card = getenv("HOME");
    card += "/Applications/addressbook/businesscard.vcf";

    if ( QFile::exists( card ) ) {
	QCopEnvelope e("QPE/Obex", "send(QString,QString,QString)");
	QString mimetype = "text/x-vCard";
	e << tr("business card") << card << mimetype;
    }
}

#if defined(QPE_HAVE_MEMALERTER)
QPE_MEMALERTER_IMPL
#endif

#if defined(CUSTOM_SOUND_IMPL)
CUSTOM_SOUND_IMPL
#endif

//===========================================================================

Desktop::Desktop() :
    QWidget( 0, 0, WStyle_Tool | WStyle_Customize ),
    qcopBridge( 0 ),
    transferServer( 0 ),
    packageSlave( 0 )
{
#ifdef CUSTOM_SOUND_INIT
    CUSTOM_SOUND_INIT;
#endif

    qpedesktop = this;

//    bg = new Info( this );
    tb = new TaskBar;

    launcher = new Launcher( 0, 0, WStyle_Customize | QWidget::WGroupLeader );

    connect(launcher, SIGNAL(busy()), tb, SLOT(startWait()));
    connect(launcher, SIGNAL(notBusy(const QString&)), tb, SLOT(stopWait(const QString&)));

    int displayw = qApp->desktop()->width();
    int displayh = qApp->desktop()->height();


    QSize sz = tb->sizeHint();

    setGeometry( 0, displayh-sz.height(), displayw, sz.height() );
    tb->setGeometry( 0, displayh-sz.height(), displayw, sz.height() );

    tb->show();
    launcher->showMaximized();
    launcher->show();
    launcher->raise();
#if defined(QPE_HAVE_MEMALERTER)
    initMemalerter();
#endif
    // start services
    startTransferServer();
    (void) new IrServer( this );
    rereadVolumes();

    packageSlave = new PackageSlave( this );
    connect(qApp, SIGNAL(volumeChanged(bool)), this, SLOT(rereadVolumes()));

    qApp->installEventFilter( this );
}

void Desktop::show()
{
    login(TRUE);
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
    static bool ignoreNormal=FALSE;
    static bool existingMessage=FALSE;

    if(existingMessage)
	return; // don't show a second message while still on first

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
	    else
		QMessageBox::information ( 0 , "Memory Status",
		    "There is enough memory again." );
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

static bool isVisibleWindow(int wid)
{
    const QList<QWSWindow> &list = qwsServer->clientWindows();
    QWSWindow* w;
    for (QListIterator<QWSWindow> it(list); (w=it.current()); ++it) {
	if ( w->winId() == wid )
	    return !w->isFullyObscured();
    }
    return FALSE;
}

static bool hasVisibleWindow(const QString& clientname)
{
    const QList<QWSWindow> &list = qwsServer->clientWindows();
    QWSWindow* w;
    for (QListIterator<QWSWindow> it(list); (w=it.current()); ++it) {
	if ( w->client()->identity() == clientname && !w->isFullyObscured() )
	    return TRUE;
    }
    return FALSE;
}

void Desktop::raiseLauncher()
{
    if ( isVisibleWindow(launcher->winId()) )
	launcher->nextView();
    else
	launcher->raise();
}

void Desktop::executeOrModify(const QString& appLnkFile)
{
    AppLnk lnk(MimeType::appsFolderName() + "/" + appLnkFile);
    if ( lnk.isValid() ) {
	QCString app = lnk.exec().utf8();
	Global::terminateBuiltin("calibrate");
	if ( QCopChannel::isRegistered("QPE/Application/" + app) ) {
	    MRUList::addTask(&lnk);
	    if ( hasVisibleWindow(app) )
		QCopChannel::send("QPE/Application/" + app, "nextView()");
	    else
		QCopChannel::send("QPE/Application/" + app, "raise()");
	} else {
	    lnk.execute();
	}
    }
}

void Desktop::raiseDatebook()
{
    executeOrModify("Applications/datebook.desktop");
}

void Desktop::raiseContacts()
{
    executeOrModify("Applications/addressbook.desktop");
}

void Desktop::raiseMenu()
{
    Global::terminateBuiltin("calibrate");
    tb->startMenu()->launch();
}

void Desktop::raiseEmail()
{
    executeOrModify("Applications/qtmail.desktop");
}

// autoStarts apps on resume and start
void Desktop::execAutoStart()
{
  QString appName;
  Config cfg( "autostart" );
  cfg.setGroup( "AutoStart" );
  appName = cfg.readEntry("Apps", "");
  QCopEnvelope e("QPE/System", "execute(QString)");
  e << QString(appName);
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

static bool blanked=FALSE;

static void blankScreen()
{
    if ( !qt_screen ) return;
    /* Should use a big black window instead.
    QGfx* g = qt_screen->screenGfx();
    g->fillRect(0,0,qt_screen->width(),qt_screen->height());
    delete g;
    */
    blanked = TRUE;
}

static void darkScreen()
{
    extern void qpe_setBacklight(int);
    qpe_setBacklight(0); // force off
}


void Desktop::togglePower()
{
  bool wasloggedin = loggedin;
  loggedin=0;
  darkScreen();
  if ( wasloggedin )
    blankScreen();
  
  system("apm --suspend");
  
  

  QWSServer::screenSaverActivate( FALSE );
  {
    QCopEnvelope("QPE/Card", "mtabChanged()" ); // might have changed while asleep
    QCopEnvelope e("QPE/System", "setBacklight(int)");
    e << -3; // Force on
  }
  if ( wasloggedin ) {
    login(TRUE);
  }
  sleep(1);
  execAutoStart();
  //qcopBridge->closeOpenConnections();
  //qDebug("called togglePower()!!!!!!");
}

void Desktop::toggleLight()
{
    QCopEnvelope e("QPE/System", "setBacklight(int)");
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
    int displayw = qApp->desktop()->width();
    int displayh = qApp->desktop()->height();

    QSize sz = tb->sizeHint();

    tb->setGeometry( 0, displayh-sz.height(), displayw, sz.height() );
}

void DesktopApplication::shutdown()
{
    if ( type() != GuiServer )
	return;
    ShutdownImpl *sd = new ShutdownImpl( 0, 0, WDestructiveClose );
    connect( sd, SIGNAL(shutdown(ShutdownImpl::Type)),
	     this, SLOT(shutdown(ShutdownImpl::Type)) );
    sd->showMaximized();
}

void DesktopApplication::shutdown( ShutdownImpl::Type t )
{
    switch ( t ) {
	case ShutdownImpl::ShutdownSystem:
	    execlp("shutdown", "shutdown", "-h", "now", (void*)0);
	    break;
	case ShutdownImpl::RebootSystem:
	    execlp("shutdown", "shutdown", "-r", "now", (void*)0);
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

void DesktopApplication::restart()
{
    prepareForTermination(TRUE);

#ifdef Q_WS_QWS
    for ( int fd = 3; fd < 100; fd++ )
	close( fd );
#if defined(QT_DEMO_SINGLE_FLOPPY)
    execl( "/sbin/init", "qpe", 0 );
#elif defined(QT_QWS_CASSIOPEIA)
    execl( "/bin/sh", "sh", 0 );
#else
    execl( (qpeDir()+"/bin/qpe").latin1(), "qpe", 0 );
#endif
    exit(1);
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

void Desktop::rereadVolumes()
{
    Config cfg("Sound");
    cfg.setGroup("System");
    touchclick = cfg.readBoolEntry("Touch");
    keyclick = cfg.readBoolEntry("Key");
}

void Desktop::keyClick()
{
#ifdef CUSTOM_SOUND_KEYCLICK
    if ( keyclick )
	CUSTOM_SOUND_KEYCLICK;
#endif
}

void Desktop::screenClick()
{
#ifdef CUSTOM_SOUND_TOUCH
    if ( touchclick )
	CUSTOM_SOUND_TOUCH;
#endif
}

void Desktop::soundAlarm()
{
#ifdef CUSTOM_SOUND_ALARM
    CUSTOM_SOUND_ALARM;
#endif
}

bool Desktop::eventFilter( QObject *w, QEvent *ev )
{
    if ( ev->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent *)ev;
	if ( ke->key() == Qt::Key_F11 ) { // menu key
	    QWidget *active = qApp->activeWindow();
	    if ( active && active->isPopup() ) {
		active->close();
	    }
	    raiseMenu();
	    return TRUE;
	}
    }
    return FALSE;
}

