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
//#include "mrulist.h"
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
#include <qpe/timeconversion.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/network.h>
#include <qpe/global.h>

#if defined( QT_QWS_CUSTOM ) || defined( QT_QWS_IPAQ )
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
  QCopKeyRegister( int k, const QString &c, const QString &m )
      : keyCode( k ), channel( c ), message( m )
  { }

  int getKeyCode() const
  {
    return keyCode;
  }
  QString getChannel() const
  {
    return channel;
  }
  QString getMessage() const
  {
    return message;
  }

private:
  int keyCode;
  QString channel, message;
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


class QPEScreenSaver : public QWSScreenSaver
{
private:
  int LcdOn;

public:
  QPEScreenSaver()
  {
    m_disable_suspend = 100;
    m_enable_dim = false;
    m_enable_lightoff = false;
    m_enable_onlylcdoff = false;

    m_lcd_status = true;

    m_backlight_bright = -1;
    m_backlight_forcedoff = false;

    // Make sure the LCD is in fact on, (if opie was killed while the LCD is off it would still be off)
    ODevice::inst ( ) -> setDisplayStatus ( true );
  }
  void restore()
  {
    if ( !m_lcd_status ) {    // We must have turned it off
      ODevice::inst ( ) -> setDisplayStatus ( true );
      m_lcd_status = true;
    }

    setBacklight ( -3 );
  }
  bool save( int level )
  {
    switch ( level ) {
      case 0:
        if ( m_disable_suspend > 0 && m_enable_dim ) {
          if ( backlight() > 1 )
            setBacklight( 1 ); // lowest non-off
        }
        return true;
        break;
      case 1:
        if ( m_disable_suspend > 1 && m_enable_lightoff ) {
          setBacklight( 0 ); // off
        }
        return true;
        break;
      case 2:
        if ( m_enable_onlylcdoff ) {
          ODevice::inst ( ) -> setDisplayStatus ( false );
          m_lcd_status = false;
          return true;
        }
        else // We're going to suspend the whole machine
        {
          if ( ( m_disable_suspend > 2 ) &&
                  ( PowerStatusManager::readStatus().acStatus() != PowerStatus::Online ) &&
                  ( !Network::networkOnline ( ) ) ) {
            QWSServer::sendKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );
            return true;
          }
        }
        break;
    }
    return false;
  }

private:
  static int ssi( int interval, Config & config, const QString & enable, const QString & value, int def )
  {
    if ( !enable.isEmpty() && config.readNumEntry( enable, 0 ) == 0 )
      return 0;

    if ( interval < 0 ) {
      // Restore screen blanking and power saving state
      interval = config.readNumEntry( value, def );
    }
    return interval;
  }

public:
  void setIntervals( int i1, int i2, int i3 )
  {
    Config config( "qpe" );
    config.setGroup( "Screensaver" );

    int v[ 4 ];
    i1 = ssi( i1, config, "Dim", "Interval_Dim", 30 );
    i2 = ssi( i2, config, "LightOff", "Interval_LightOff", 20 );
    i3 = ssi( i3, config, "", "Interval", 60 );

    //qDebug("screen saver intervals: %d %d %d", i1, i2, i3);

    v [ 0 ] = QMAX( 1000 * i1, 100 );
    v [ 1 ] = QMAX( 1000 * i2, 100 );
    v [ 2 ] = QMAX( 1000 * i3, 100 );
    v [ 3 ] = 0;
    m_enable_dim = ( ( i1 != 0 ) ? config. readNumEntry ( "Dim", 1 ) : false );
    m_enable_lightoff = ( ( i2 != 0 ) ? config. readNumEntry ( "LightOff", 1 ) : false );
    m_enable_onlylcdoff = config. readNumEntry ( "LcdOffOnly", 0 );

    if ( !i1 && !i2 && !i3 )
      QWSServer::setScreenSaverInterval( 0 );
    else
      QWSServer::setScreenSaverIntervals( v );
  }

  void setInterval ( int interval )
  {
    setIntervals ( -1, -1, interval );
  }

  void setMode ( int mode )
  {
    if ( mode > m_disable_suspend )
      setInterval( -1 );
    m_disable_suspend = mode;
  }

  int backlight ( )
  {
    if ( m_backlight_bright == -1 ) {
      // Read from config
      Config config ( "qpe" );
      config. setGroup ( "Screensaver" );
      m_backlight_bright = config. readNumEntry ( "Brightness", 255 );
    }
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

  void setDisplayState ( bool on )
  {
    if ( m_lcd_status != on ) {
      ODevice::inst ( ) -> setDisplayStatus ( on );
      m_lcd_status = on;
    }
  }

private:
  int m_disable_suspend;
  bool m_enable_dim;
  bool m_enable_lightoff;
  bool m_enable_onlylcdoff;

  bool m_lcd_status;

  int m_backlight_bright;
  bool m_backlight_forcedoff;
};


void DesktopApplication::switchLCD ( bool on )
{
  if ( qApp ) {
    DesktopApplication *dapp = (DesktopApplication *) qApp;

    if ( dapp-> m_screensaver ) {
      if ( on )
      dapp-> m_screensaver-> restore ( ); //setBacklight ( on ? -3 : -1 );
    else
      dapp-> m_screensaver-> save ( 1 );
    
  }
  }
}


DesktopApplication::DesktopApplication( int& argc, char **argv, Type appType )
    : QPEApplication( argc, argv, appType )
{

  QTimer * t = new QTimer( this );
  connect( t, SIGNAL( timeout() ), this, SLOT( psTimeout() ) );
  t->start( 10000 );
  ps = new PowerStatus;
  pa = new DesktopPowerAlerter( 0 );

  channel = new QCopChannel( "QPE/Desktop", this );
  connect( channel, SIGNAL( received( const QCString&, const QByteArray& ) ),
           this, SLOT( desktopMessage( const QCString&, const QByteArray& ) ) );

  channel = new QCopChannel( "QPE/System", this );
  connect( channel, SIGNAL( received( const QCString&, const QByteArray& ) ),
           this, SLOT( systemMessage( const QCString&, const QByteArray& ) ) );

  m_screensaver = new QPEScreenSaver;

  m_screensaver-> setInterval ( -1 );
  QWSServer::setScreenSaver( m_screensaver );
}


DesktopApplication::~DesktopApplication()
{
  delete ps;
  delete pa;
}

void DesktopApplication::desktopMessage( const QCString &msg, const QByteArray &data )
{
#ifdef Q_WS_QWS
  QDataStream stream( data, IO_ReadOnly );
  if ( msg == "keyRegister(int key, QString channel, QString message)" ) {
    int k;
    QString c, m;
    stream >> k;
    stream >> c;
    stream >> m;

    qWarning( "KeyRegisterReceived: %i, %s, %s", k, ( const char* ) c, ( const char * ) m );
    keyRegisterList.append( QCopKeyRegister( k, c, m ) );
  }
#endif
}


void DesktopApplication::systemMessage( const QCString & msg, const QByteArray & data )
{
#ifdef Q_WS_QWS
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
  else if ( msg == "setDisplayState(int)" ) {
  	int state;
  	stream >> state;
  	m_screensaver-> setDisplayState ( state != 0 );
  }
  else if ( msg == "suspend()" ) {
    emit power();
  }
#endif
}

enum MemState { Unknown, VeryLow, Low, Normal } memstate = Unknown;

#ifdef Q_WS_QWS
bool DesktopApplication::qwsEventFilter( QWSEvent *e )
{
  qpedesktop->checkMemory();

  if ( e->type == QWSEvent::Key ) {
    QWSKeyEvent * ke = ( QWSKeyEvent * ) e;
    if ( !loggedin && ke->simpleData.keycode != Key_F34 )
      return TRUE;
    bool press = ke->simpleData.is_press;
    bool autoRepeat = ke->simpleData.is_auto_repeat;

    /*
      app that registers key/message to be sent back to the app, when it doesn't have focus,
      when user presses key, unless keyboard has been requested from app.
      will not send multiple repeats if user holds key
      i.e. one shot
     */
    if ( !keyRegisterList.isEmpty() &&  ke->simpleData.keycode !=0 && press) {
//        qDebug("<<<<<<<<<<<<<keycode %d", ke->simpleData.keycode);
      KeyRegisterList::Iterator it;
      for ( it = keyRegisterList.begin(); it != keyRegisterList.end(); ++it ) {
        if ( ( *it ).getKeyCode() == ke->simpleData.keycode && !autoRepeat && !keyboardGrabbed() ) {
          if ( press )
            qDebug( "press" );
          else
            qDebug( "release" );
          QCopEnvelope( ( *it ).getChannel().utf8(), ( *it ).getMessage().utf8() );
        }
      }
    }

    if ( !keyboardGrabbed() ) {
      if ( ke->simpleData.keycode == Key_F9 ) {
        if ( press )
          emit datebook();
        return TRUE;
      }
      if ( ke->simpleData.keycode == Key_F10 ) {
        if ( !press && cardSendTimer ) {
          emit contacts();
          delete cardSendTimer;
        }
        else if ( press ) {
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
        while ( activePopupWidget() )
          activePopupWidget() ->close();
        if ( press )
          emit launch();
        return TRUE;
      }
      if ( ke->simpleData.keycode == Key_F13 ) {
        if ( press )
          emit email();
        return TRUE;
      }
    }

    if ( ke->simpleData.keycode == Key_F34 ) {
      if ( press )
        emit power();
      return TRUE;
    }
    // This was used for the iPAQ PowerButton
    // See main.cpp for new KeyboardFilter
    //
    //    if ( ke->simpleData.keycode == Key_SysReq ) {
    //      if ( press ) emit power();
    //      return TRUE;
    //    }
    if ( ke->simpleData.keycode == Key_F35 ) {
      if ( press )
        emit backlight();
      return TRUE;
    }
    if ( ke->simpleData.keycode == Key_F32 ) {
      if ( press )
        QCopEnvelope e( "QPE/Desktop", "startSync()" );
      return TRUE;
    }
    if ( ke->simpleData.keycode == Key_F31 && !ke->simpleData.modifiers ) {
      if ( press )
        emit symbol();
      return TRUE;
    }
    if ( ke->simpleData.keycode == Key_NumLock ) {
      if ( press )
        emit numLockStateToggle();
    }
    if ( ke->simpleData.keycode == Key_CapsLock ) {
      if ( press )
        emit capsLockStateToggle();
    }
    if ( ( press && !autoRepeat ) || ( !press && autoRepeat ) )
      qpedesktop->keyClick();
  }
  else {
    if ( e->type == QWSEvent::Mouse ) {
      QWSMouseEvent * me = ( QWSMouseEvent * ) e;
      static bool up = TRUE;
      if ( me->simpleData.state & LeftButton ) {
        if ( up ) {
          up = FALSE;
          qpedesktop->screenClick();
        }
      }
      else {
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

  if ( ( ps->batteryStatus() == PowerStatus::VeryLow ) ) {
    pa->alert( tr( "Battery is running very low." ), 6 );
  }

  if ( ps->batteryStatus() == PowerStatus::Critical ) {
    pa->alert( tr( "Battery level is critical!\n"
                   "Keep power off until power restored!" ), 1 );
  }

  if ( ps->backupBatteryStatus() == PowerStatus::VeryLow ) {
    pa->alert( tr( "The Back-up battery is very low.\nPlease charge the back-up battery." ), 3 );
  }
}


void DesktopApplication::sendCard()
{
  delete cardSendTimer;
  cardSendTimer = 0;
  QString card = getenv( "HOME" );
  card += "/Applications/addressbook/businesscard.vcf";

  if ( QFile::exists( card ) ) {
    QCopEnvelope e( "QPE/Obex", "send(QString,QString,QString)" );
    QString mimetype = "text/x-vCard";
    e << tr( "business card" ) << card << mimetype;
  }
}

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
  rereadVolumes();

  packageSlave = new PackageSlave( this );
  connect( qApp, SIGNAL( volumeChanged( bool ) ), this, SLOT( rereadVolumes() ) );

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
  const QList<QWSWindow> &list = qwsServer->clientWindows();
  QWSWindow* w;
  for ( QListIterator<QWSWindow> it( list ); ( w = it.current() ); ++it ) {
    if ( w->winId() == wid )
      return !w->isFullyObscured();
  }
  return FALSE;
}

static bool hasVisibleWindow( const QString& clientname )
{
  const QList<QWSWindow> &list = qwsServer->clientWindows();
  QWSWindow* w;
  for ( QListIterator<QWSWindow> it( list ); ( w = it.current() ); ++it ) {
    if ( w->client() ->identity() == clientname && !w->isFullyObscured() )
      return TRUE;
  }
  return FALSE;
}

void Desktop::raiseLauncher()
{
  Config cfg( "qpe" ); //F12 'Home'
  cfg.setGroup( "AppsKey" );
  QString tempItem;
  tempItem = cfg.readEntry( "Middle", "Home" );
  if ( tempItem == "Home" || tempItem.isEmpty() ) {
    home ( );
  }
  else {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << tempItem;
  }
}

void Desktop::home ( )
{ 
    if ( isVisibleWindow( launcher->winId() ) )
      launcher->nextView();
    else
      launcher->raise();
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

void Desktop::raiseDatebook()
{
  Config cfg( "qpe" ); //F9 'Activity'
  cfg.setGroup( "AppsKey" );
  QString tempItem;
  tempItem = cfg.readEntry( "LeftEnd" , "Calendar" );
  if ( tempItem == "Calendar" || tempItem.isEmpty() ) {
    tempItem = "datebook";
  }
  QCopEnvelope e( "QPE/System", "execute(QString)" );
  e << tempItem;
}

void Desktop::raiseContacts()
{
  Config cfg( "qpe" ); //F10, 'Contacts'
  cfg.setGroup( "AppsKey" );
  QString tempItem;
  tempItem = cfg.readEntry( "Left2nd", "Address Book" );
  if ( tempItem == "Address Book" || tempItem.isEmpty() ) {
    tempItem = "addressbook";
  }
  QCopEnvelope e( "QPE/System", "execute(QString)" );
  e << tempItem;
}

void Desktop::raiseMenu()
{
  Config cfg( "qpe" ); //F11, 'Menu
  cfg.setGroup( "AppsKey" );
  QString tempItem;
  tempItem = cfg.readEntry( "Right2nd" , "Popup Menu" );
  if ( tempItem == "Popup Menu" || tempItem.isEmpty() ) {
    Global::terminateBuiltin( "calibrate" );
    tb->startMenu() ->launch();
  }
  else {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << tempItem;
  }
}

void Desktop::raiseEmail()
{
  Config cfg( "qpe" ); //F13, 'Mail' // only in zaurus, on ipaq mail key is F11
  cfg.setGroup( "AppsKey" );
  QString tempItem;
  tempItem = cfg.readEntry( "RightEnd", "Mail" );
  if ( tempItem == "Mail" || tempItem == "qtmail" || tempItem.isEmpty() ) {
    tempItem = "mail";
  }
  QCopEnvelope e( "QPE/System", "execute(QString)" );
  e << tempItem;
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

  ODevice::inst ( ) -> suspend ( );

  QWSServer::screenSaverActivate ( false );
  DesktopApplication::switchLCD ( true ); // force LCD on without slow qcop call

  {
    QCopEnvelope( "QPE/Card", "mtabChanged()" ); // might have changed while asleep
  }

  if ( wasloggedin )
    login( TRUE );

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

void Desktop::rereadVolumes()
{
  Config cfg( "qpe" );
  cfg.setGroup( "Volume" );
  touchclick = cfg.readBoolEntry( "TouchSound" );
  keyclick = cfg.readBoolEntry( "KeySound" );
  alarmsound = cfg.readBoolEntry( "AlarmSound" );
}

void Desktop::keyClick()
{
  if ( keyclick )
    ODevice::inst ( ) -> keySound ( );
}

void Desktop::screenClick()
{
  if ( touchclick )
    ODevice::inst ( ) -> touchSound ( );
}

void Desktop::soundAlarm()
{
  if ( qpedesktop-> alarmsound )
    ODevice::inst ( ) -> alarmSound ( );
}

