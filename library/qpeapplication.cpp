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
** $Id: qpeapplication.cpp,v 1.11 2002-06-24 22:38:00 sandman Exp $
**
**********************************************************************/
#define QTOPIA_INTERNAL_LANGLIST
#include <stdlib.h>
#include <unistd.h>
#include <qfile.h>
#ifdef Q_WS_QWS
#ifndef QT_NO_COP
#if QT_VERSION <= 231
#define private public
#define sendLocally processEvent
#include "qcopenvelope_qws.h"
#undef private
#else
#include "qcopenvelope_qws.h"
#endif
#endif
#include <qwindowsystem_qws.h>
#endif
#include <qtextstream.h>
#include <qpalette.h>
#include <qbuffer.h>
#include <qptrdict.h>
#include <qregexp.h>
#include <qdir.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qdragobject.h>
#include <qevent.h>
#include <qtooltip.h>
#include <qsignal.h>
#include <linux/fb.h>

#include <qsignal.h>
#include "qpeapplication.h"
#include "qpestyle.h"
#if QT_VERSION >= 300
#include <qstylefactory.h>
#else
#include <qplatinumstyle.h>
#include <qwindowsstyle.h>
#include <qmotifstyle.h>
#include <qmotifplusstyle.h>
#include "lightstyle.h"
#endif
#include "global.h"
#include "resource.h"
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
#include "qutfcodec.h"
#endif
#include "config.h"
#include "network.h"
#include "fontmanager.h"
#include "fontdatabase.h"

#include "power.h"
#include "alarmserver.h"
#include "applnk.h"
#include "qpemenubar.h"

#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

// for setBacklight()
#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <stdlib.h>

#include <dlfcn.h> // for Liquid HACK

class QPEApplicationData {
public:
    QPEApplicationData() : presstimer(0), presswidget(0), rightpressed(FALSE),
  kbgrabber(0), kbregrab(FALSE), notbusysent(FALSE), preloaded(FALSE),
  forceshow(FALSE), nomaximize(FALSE), qpe_main_widget(0),
  keep_running(TRUE)
    {
  qcopq.setAutoDelete(TRUE);
    }

    int presstimer;
    QWidget* presswidget;
    QPoint presspos;
    bool rightpressed;
    int kbgrabber;
    bool kbregrab;
    bool notbusysent;
    QString appName;
    struct QCopRec {
  QCopRec(const QCString &ch, const QCString &msg,
                               const QByteArray &d) :
      channel(ch), message(msg), data(d) { }

  QCString channel;
  QCString message;
  QByteArray data;
    };
    bool preloaded;
    bool forceshow;
    bool nomaximize;
    QWidget* qpe_main_widget;
    bool keep_running;
    QList<QCopRec> qcopq;

    void enqueueQCop(const QCString &ch, const QCString &msg,
                               const QByteArray &data)
    {
  qcopq.append(new QCopRec(ch,msg,data));
    }
    void sendQCopQ()
    {
  QCopRec* r;
  for (QListIterator<QCopRec> it(qcopq); (r=it.current()); ++it)
      QCopChannel::sendLocally(r->channel,r->message,r->data);
  qcopq.clear();
    }
};

class ResourceMimeFactory : public QMimeSourceFactory {
public:
    ResourceMimeFactory()
    {
  setFilePath( Global::helpPath() );
  setExtensionType("html","text/html;charset=UTF-8");
    }

    const QMimeSource* data(const QString& abs_name) const
    {
  const QMimeSource* r = QMimeSourceFactory::data(abs_name);
  if ( !r ) {
      int sl = abs_name.length();
      do {
    sl = abs_name.findRev('/',sl-1);
    QString name = sl>=0 ? abs_name.mid(sl+1) : abs_name;
    int dot = name.findRev('.');
    if ( dot >= 0 )
        name = name.left(dot);
    QImage img = Resource::loadImage(name);
    if ( !img.isNull() )
        r = new QImageDrag(img);
      } while (!r && sl>0);
  }
  return r;
    }
};

static int muted=0;
static int micMuted=0;

static void setVolume(int t=0, int percent=-1)
{
  switch (t) {
  case 0: {
  Config cfg("qpe");
  cfg.setGroup("Volume");
  if ( percent < 0 )
    percent = cfg.readNumEntry("VolumePercent",50);
  int fd = 0;
  if ((fd = open("/dev/mixer", O_RDWR))>=0) {
    int vol = muted ? 0 : percent;
    // set both channels to same volume
    vol |= vol << 8;
    ioctl(fd, MIXER_WRITE(0), &vol);
    ::close(fd);
  }
  } break;
  }
}

static void setMic(int t=0, int percent=-1)
{
  switch (t) {
  case 0: {
  Config cfg("qpe");
  cfg.setGroup("Volume");
  if ( percent < 0 )
    percent = cfg.readNumEntry("Mic",50);
  
  int fd = 0;
  int mic = micMuted ? 0 : percent;
  if ((fd = open("/dev/mixer", O_RDWR))>=0) {
    ioctl(fd, MIXER_WRITE(SOUND_MIXER_MIC), &mic);
    ::close(fd);
  }
  } break;
  }
}

int qpe_sysBrightnessSteps()
{
#if defined(QT_QWS_IPAQ)
    return 255;
#elif defined(QT_QWS_EBX)
    return 4;
#else
    return 255; // ?
#endif
}


static int& hack(int& i)
{
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
    // These should be created, but aren't in Qt 2.3.0
    (void)new QUtf8Codec;
    (void)new QUtf16Codec;
#endif
    return i;
}

static bool forced_off = FALSE;
static int curbl=-1;

static int backlight()
{
    if ( curbl == -1 ) {
  // Read from config
  Config config( "qpe" );
  config.setGroup( "Screensaver" );
  curbl = config.readNumEntry("Brightness",255);
    }
    return curbl;
}

static void setBacklight(int bright)
{
    if ( bright == -3 ) {
  // Forced on
  forced_off = FALSE;
  bright = -1;
    }
    if ( forced_off && bright != -2 )
  return;
    if ( bright == -2 ) {
  // Toggle between off and on
  bright = curbl ? 0 : -1;
  forced_off = !bright;
    }
    if ( bright == -1 ) {
  // Read from config
  Config config( "qpe" );
  config.setGroup( "Screensaver" );
  bright = config.readNumEntry("Brightness",255);
    }
#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)
    if ( QFile::exists("/usr/bin/bl") ) {
  QString cmd = "/usr/bin/bl 1 ";
  cmd += bright<=0 ? "0 " : "1 ";
  cmd += QString::number(bright);
  system(cmd.latin1());
#if defined(QT_QWS_EBX)
    } else if ( QFile::exists("/dev/fl") ) {
#define FL_IOCTL_STEP_CONTRAST    100
  int fd = open("/dev/fl", O_WRONLY);
  if (fd >= 0 ) {
      int steps = qpe_sysBrightnessSteps();
      int bl = ( bright * steps + 127 ) / 255;
      if ( bright && !bl ) bl = 1;
      bl = ioctl(fd, FL_IOCTL_STEP_CONTRAST, bl);
      close(fd);
  }
    }
#elif defined(QT_QWS_IPAQ)
    } else if ( QFile::exists("/dev/ts") || QFile::exists("/dev/h3600_ts") ) {
  typedef struct {
      unsigned char mode;
      unsigned char pwr;
      unsigned char brightness;
  } FLITE_IN;
# ifndef FLITE_ON
#  ifndef _LINUX_IOCTL_H
#   include <linux/ioctl.h>
#  endif
#  define FLITE_ON                _IOW('f', 7, FLITE_IN)
# endif
  int fd;
  if ( QFile::exists("/dev/ts") )
      fd = open("/dev/ts", O_WRONLY);
  else
      fd = open("/dev/h3600_ts", O_WRONLY);
  if (fd >= 0 ) {
      FLITE_IN bl;
      bl.mode = 1;
      bl.pwr = bright ? 1 : 0;
      bl.brightness = bright;
      ioctl(fd, FLITE_ON, &bl);
      close(fd);
  }
    }
#endif
#endif
    curbl = bright;
}

void qpe_setBacklight(int bright) { setBacklight(bright); }

static bool dim_on = FALSE;
static bool lightoff_on = FALSE;
static int disable_suspend = 100;

static bool powerOnline()
{
    return PowerStatusManager::readStatus().acStatus() == PowerStatus::Online;
}

static bool networkOnline()
{
    return Network::networkOnline();
}

class QPEScreenSaver : public QWSScreenSaver
{
private:
    int LcdOn;

public:
    QPEScreenSaver()
    {
    int fd;

    LcdOn = TRUE;
    // Make sure the LCD is in fact on, (if opie was killed while the LCD is off it would still be off)
    fd=open("/dev/fb0",O_RDWR);
    if (fd != -1) { ioctl(fd,FBIOBLANK,VESA_NO_BLANKING); close(fd); }
    }
    void restore()
    {
  if (!LcdOn) // We must have turned it off
    {
    int fd;
    fd=open("/dev/fb0",O_RDWR);
    if (fd != -1) { ioctl(fd,FBIOBLANK,VESA_NO_BLANKING); close(fd); }
    }
  setBacklight(-1);
    }
    bool save(int level)
    {
  int fd;

  switch ( level ) {
   case 0:
      if ( disable_suspend > 0 && dim_on ) {
    if (backlight() > 1)
        setBacklight(1); // lowest non-off
      }
      return TRUE;
      break;
   case 1:
      if ( disable_suspend > 1 && lightoff_on ) {
    setBacklight(0); // off
      }
      return TRUE;
      break;
   case 2:
       Config config( "qpe" );
       config.setGroup( "Screensaver" );       
       if (config.readNumEntry("LcdOffOnly",0) != 0)  // We're only turning off the LCD
    {
        fd=open("/dev/fb0",O_RDWR);
        if (fd != -1) { ioctl(fd,FBIOBLANK,VESA_POWERDOWN); close(fd); }
    LcdOn = FALSE;
    }        
       else // We're going to suspend the whole machine
    {
        if ( disable_suspend > 2 && !powerOnline() && !networkOnline() ) {
      QWSServer::sendKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );
      return TRUE;
      }
    }
      break;
  }
  return FALSE;
    }
};

static int ssi(int interval, Config &config, const QString &enable, const QString& value, int def)
{
    if ( !enable.isEmpty() && config.readNumEntry(enable,0) == 0 )
  return 0;

    if ( interval < 0 ) {
  // Restore screen blanking and power saving state
  interval = config.readNumEntry( value, def );
    }
    return interval;
}

static void setScreenSaverIntervals(int i1, int i2, int i3)
{
    Config config( "qpe" );
    config.setGroup( "Screensaver" );

    int v[4];
    i1 = ssi(i1, config, "Dim","Interval_Dim", 30);
    i2 = ssi(i2, config, "LightOff","Interval_LightOff", 20);
    i3 = ssi(i3, config, "","Interval", 60);

    //qDebug("screen saver intervals: %d %d %d", i1, i2, i3);

    v[0] = QMAX( 1000*i1, 100);
    v[1] = QMAX( 1000*i2, 100);
    v[2] = QMAX( 1000*i3, 100);
    v[3] = 0;
    dim_on = ( (i1 != 0) ? config.readNumEntry("Dim",1) : FALSE );
    lightoff_on = ( (i2 != 0 ) ? config.readNumEntry("LightOff",1) : FALSE );
    if ( !i1 && !i2 && !i3 )
  QWSServer::setScreenSaverInterval(0);
    else
  QWSServer::setScreenSaverIntervals(v);
}

static void setScreenSaverInterval(int interval)
{
    setScreenSaverIntervals(-1,-1,interval);
}


/*!
  \class QPEApplication qpeapplication.h
  \brief The QPEApplication class implements various system services
    that are available to all Qtopia applications.

  Simply by using QPEApplication instead of QApplication, a plain Qt
  application becomes a Qtopia application. It automatically follows
  style changes, quits and raises, and in the
  case of \link docwidget.html document-oriented\endlink applications,
  changes the current displayed document in response to the environment.
*/

/*!
  \fn void QPEApplication::clientMoused()

  \internal
*/

/*!
  \fn void QPEApplication::timeChanged();

  This signal is emitted when the time jumps forward or backwards
  by more than the normal passage of time.
*/

/*!
  \fn void QPEApplication::clockChanged( bool ampm );

  This signal is emitted when the user changes the style
  of clock. If \a ampm is TRUE, the user wants a 12-hour
  AM/PM close, otherwise, they want a 24-hour clock.
*/

/*!
  \fn void QPEApplication::appMessage( const QCString& msg, const QByteArray& data )

  This signal is emitted when a message is received on the
  QPE/Application/<i>appname</i> QCop channel for this application.

  The slot to which you connect this signal uses \a msg and \a data
  in the following way:

\code
    void MyWidget::receive( const QCString& msg, const QByteArray& data )
    {
  QDataStream stream( data, IO_ReadOnly );
  if ( msg == "someMessage(int,int,int)" ) {
      int a,b,c;
      stream >> a >> b >> c;
      ...
  } else if ( msg == "otherMessage(QString)" ) {
      ...
  }
    }
\endcode

  \sa qcop.html
*/

/*!
  Constructs a QPEApplication just as you would construct
  a QApplication, passing \a argc, \a argv, and \a t.
*/
QPEApplication::QPEApplication( int& argc, char **argv, Type t )
    : QApplication( hack(argc), argv, t )
{
    int dw = desktop()->width();
    if ( dw < 200 ) {
//  setFont( QFont( "helvetica", 8 ) );
  AppLnk::setSmallIconSize(10);
  AppLnk::setBigIconSize(28);
    }

    d = new QPEApplicationData;
    QMimeSourceFactory::setDefaultFactory(new ResourceMimeFactory);

    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(hideOrQuit()));
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)

    QString qcopfn("/tmp/qcop-msg-");
    qcopfn += QString(argv[0]); // append command name

    QFile f(qcopfn);
    if ( f.open(IO_ReadOnly) ) {
  flock(f.handle(), LOCK_EX);
    }

    sysChannel = new QCopChannel( "QPE/System", this );
    connect( sysChannel, SIGNAL(received(const QCString &, const QByteArray &)),
       this, SLOT(systemMessage( const QCString &, const QByteArray &)) );

    QCString channel = QCString(argv[0]);
    channel.replace(QRegExp(".*/"),"");
    d->appName = channel;
    channel = "QPE/Application/" + channel;
    pidChannel = new QCopChannel( channel, this);
    connect( pidChannel, SIGNAL(received(const QCString &, const QByteArray &)),
      this, SLOT(pidMessage(const QCString &, const QByteArray &)));

    if ( f.isOpen() ) {
  d->keep_running = FALSE;
  QDataStream ds(&f);
  QCString channel, message;
  QByteArray data;
  while(!ds.atEnd()) {
      ds >> channel >> message >> data;
      d->enqueueQCop(channel,message,data);
  }

  flock(f.handle(), LOCK_UN);
  f.close();
  f.remove();
    }

    for (int a=0; a<argc; a++) {
  if ( qstrcmp(argv[a],"-preload")==0 ) {
      argv[a] = argv[a+1];
      a++;
      d->preloaded = TRUE;
      argc-=1;
  } else if ( qstrcmp(argv[a],"-preload-show")==0 ) {
      argv[a] = argv[a+1];
      a++;
      d->preloaded = TRUE;
      d->forceshow = TRUE;
      argc-=1;
  }
    }

    /* overide stored arguments */
    setArgs(argc, argv);

#endif

    qwsSetDecoration( new QPEDecoration() );

#ifndef QT_NO_TRANSLATION
    QStringList langs = Global::languageList();
    for (QStringList::ConstIterator it = langs.begin(); it!=langs.end(); ++it) {
  QString lang = *it;

  QTranslator * trans;
  QString tfn;

  trans = new QTranslator(this);
  tfn = qpeDir()+"/i18n/"+lang+"/libqpe.qm";
  if ( trans->load( tfn ))
      installTranslator( trans );
  else
      delete trans;

  trans = new QTranslator(this);
  tfn = qpeDir()+"/i18n/"+lang+"/"+d->appName+".qm";
  if ( trans->load( tfn ))
      installTranslator( trans );
  else
      delete trans;

  //###language/font hack; should look it up somewhere
  if ( lang == "ja" || lang == "zh_CN" || lang == "zh_TW" || lang == "ko" ) {
      QFont fn = FontManager::unicodeFont( FontManager::Proportional );
      setFont( fn );
  }
  else {
      Config config( "qpe" );
      config.setGroup( "Appearance" );
      QString familyStr = config.readEntry( "FontFamily", "helvetica" );
      QString styleStr = config.readEntry( "FontStyle", "Regular" );
      QString sizeStr = config.readEntry( "FontSize", "10" );
      QString charSetStr = config.readEntry( "FontCharSet", QString::null );
      bool ok;
      int i_size = sizeStr.toInt( &ok, 10 );
      FontDatabase fdb;
      QFont selectedFont = fdb.font( familyStr, styleStr, i_size, charSetStr );
      setFont( selectedFont );
  }
    }

#endif

    applyStyle();

    if ( type() == GuiServer ) {
  setScreenSaverInterval(-1);
  setVolume();
  QWSServer::setScreenSaver(new QPEScreenSaver);
    }

    installEventFilter( this );

    QPEMenuToolFocusManager::initialize();

#ifdef QT_NO_QWS_CURSOR
    // if we have no cursor, probably don't want tooltips
    QToolTip::setEnabled( FALSE );
#endif
}

static QPtrDict<void>* inputMethodDict=0;
static void createInputMethodDict()
{
    if ( !inputMethodDict )
  inputMethodDict = new QPtrDict<void>;
}

/*!
  Returns the currently set hint to the system as to whether
  \a w has any use for text input methods.

  \sa setInputMethodHint()
*/
QPEApplication::InputMethodHint QPEApplication::inputMethodHint( QWidget* w )
{
    if ( inputMethodDict && w )
  return (InputMethodHint)(int)inputMethodDict->find(w);
    return Normal;
}

/*!
    \enum QPEApplication::InputMethodHint

    \value Normal the application sometimes needs text input (the default).
    \value AlwaysOff the application never needs text input.
    \value AlwaysOn the application always needs text input.
*/

/*!
  Hints to the system that \a w has use for text input methods
  as specified by \a mode.

  \sa inputMethodHint()
*/
void QPEApplication::setInputMethodHint( QWidget* w, InputMethodHint mode )
{
    createInputMethodDict();
    if ( mode == Normal ) {
  inputMethodDict->remove(w);
    } else {
  inputMethodDict->insert(w,(void*)mode);
    }
}

class HackDialog : public QDialog
{
public:
    void acceptIt() { accept(); }
    void rejectIt() { reject(); }
};


void QPEApplication::mapToDefaultAction( QWSKeyEvent *ke, int key )
{
    // specialised actions for certain widgets. May want to
    // add more stuff here.
    if ( activePopupWidget() && activePopupWidget()->inherits( "QListBox" )
   && activePopupWidget()->parentWidget()
   && activePopupWidget()->parentWidget()->inherits( "QComboBox" ) )
  key = Qt::Key_Return;

    if ( activePopupWidget() && activePopupWidget()->inherits( "QPopupMenu" ) )
  key = Qt::Key_Return;

    ke->simpleData.keycode = key;
}

class HackWidget : public QWidget
{
public:
    bool needsOk()
    { return (getWState() & WState_Reserved1 ); }
};

/*!
  \internal
*/
bool QPEApplication::qwsEventFilter( QWSEvent *e )
{
    if ( !d->notbusysent && e->type == QWSEvent::Focus ) {
  if ( qApp->type() != QApplication::GuiServer ) {
      QCopEnvelope e("QPE/System", "notBusy(QString)" );
      e << d->appName;
  }
  d->notbusysent=TRUE;
    }
    if ( type() == GuiServer ) {
  switch ( e->type ) {
      case QWSEvent::Mouse:
    if ( e->asMouse()->simpleData.state && !QWidget::find(e->window()) )
        emit clientMoused();
  }
    }
    if ( e->type == QWSEvent::Key ) {
  if ( d->kbgrabber == 1 )
      return TRUE;
  QWSKeyEvent *ke = (QWSKeyEvent *)e;
  if ( ke->simpleData.keycode == Qt::Key_F33 ) {
      // Use special "OK" key to press "OK" on top level widgets
      QWidget *active = activeWindow();
      QWidget *popup = 0;
      if ( active && active->isPopup() ) {
    popup = active;
    active = active->parentWidget();
      }
      if ( active && (int)active->winId() == ke->simpleData.window &&
     !active->testWFlags( WStyle_Customize|WType_Popup|WType_Desktop )) {
    if ( ke->simpleData.is_press ) {
        if ( popup )
      popup->close();
        if ( active->inherits( "QDialog" ) ) {
      HackDialog *d = (HackDialog *)active;
      d->acceptIt();
      return TRUE;
        } else if ( ((HackWidget *)active)->needsOk() ) {
      QSignal s;
      s.connect( active, SLOT( accept() ) );
      s.activate();
        } else {
      // do the same as with the select key: Map to the default action of the widget:
      mapToDefaultAction( ke, Qt::Key_Return );
        }
    }
      }
  } else if ( ke->simpleData.keycode == Qt::Key_F30 ) {
      // Use special "select" key to do whatever default action a widget has
      mapToDefaultAction( ke, Qt::Key_Space );
  } else if ( ke->simpleData.keycode == Qt::Key_Escape && 
        ke->simpleData.is_press ) {
      // Escape key closes app if focus on toplevel
      QWidget *active = activeWindow();
      if ( active && active->testWFlags( WType_TopLevel ) && 
     (int)active->winId() == ke->simpleData.window &&
     !active->testWFlags( WStyle_Dialog|WStyle_Customize|WType_Popup|WType_Desktop )) {
    if ( active->inherits( "QDialog" ) ) {
        HackDialog *d = (HackDialog *)active;
        d->rejectIt();
        return TRUE;
    } else if ( strcmp( argv()[0], "embeddedkonsole") != 0 ) {
        active->close();
    }
      }
  }

#if QT_VERSION < 231
    // Filter out the F4/Launcher key from apps
    // ### The launcher key may not always be F4 on all devices
  if ( ((QWSKeyEvent *)e)->simpleData.keycode == Qt::Key_F4 )
      return TRUE;
#endif
    }
    if ( e->type == QWSEvent::Focus ) {
  QWSFocusEvent *fe = (QWSFocusEvent*)e;
  QWidget* nfw = QWidget::find(e->window());
  if ( !fe->simpleData.get_focus ) {
      QWidget *active = activeWindow();
      while ( active && active->isPopup() ) {
    active->close();
    active = activeWindow();
      }
      if ( !nfw && d->kbgrabber == 2 ) {
    ungrabKeyboard();
    d->kbregrab = TRUE; // want kb back when we're active
      }
  } else {
      // make sure our modal widget is ALWAYS on top
      QWidget *topm = activeModalWidget();
      if ( topm ) {
    topm->raise();
      }
      if ( d->kbregrab ) {
    grabKeyboard();
    d->kbregrab = FALSE;
      }
  }
  if ( fe->simpleData.get_focus && inputMethodDict ) {
      InputMethodHint m = inputMethodHint( QWidget::find(e->window()) );
      if ( m == AlwaysOff )
    Global::hideInputMethod();
      if ( m == AlwaysOn )
    Global::showInputMethod();
  }
    }
    return QApplication::qwsEventFilter( e );
}

/*!
  Destroys the QPEApplication.
*/
QPEApplication::~QPEApplication()
{
    ungrabKeyboard();
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    // Need to delete QCopChannels early, since the display will
    // be gone by the time we get to ~QObject().
    delete sysChannel;
    delete pidChannel;
#endif
    delete d;
}

/*!
  Returns <tt>$OPIEDIR/</tt>.
*/
QString QPEApplication::qpeDir()
{
    const char *base = getenv( "OPIEDIR" );
    if ( base )
  return QString( base ) + "/";

    return QString( "../" );
}

/*!
  Returns the user's current Document directory. There is a trailing "/".
*/
QString QPEApplication::documentDir()
{
    const char *base = getenv( "HOME" );
    if ( base )
  return QString( base ) + "/Documents/";

    return QString( "../Documents/" );
}

static int deforient=-1;

/*!
  \internal
*/
int QPEApplication::defaultRotation()
{
    if ( deforient < 0 ) {
  QString d = getenv("QWS_DISPLAY");
  if ( d.contains("Rot90") ) {
      deforient = 90;
  } else if ( d.contains("Rot180") ) {
      deforient = 180;
  } else if ( d.contains("Rot270") ) {
      deforient = 270;
  } else {
      deforient=0;
  }
    }
    return deforient;
}

/*!
  \internal
*/
void QPEApplication::setDefaultRotation(int r)
{
    if ( qApp->type() == GuiServer ) {
  deforient = r;
  setenv("QWS_DISPLAY", QString("Transformed:Rot%1:0").arg(r).latin1(), 1);
    } else {
  QCopEnvelope("QPE/System", "setDefaultRotation(int)") << r;
    }
}

/*!
  \internal
*/
void QPEApplication::applyStyle()
{
    Config config( "qpe" );

    config.setGroup( "Appearance" );

    // Widget style
    QString style = config.readEntry( "Style", "Light" );
    internalSetStyle( style );

    // Colors
    QColor bgcolor( config.readEntry( "Background", "#E5E1D5" ) );
    QColor btncolor( config.readEntry( "Button", "#D6CDBB" ) );
    QPalette pal( btncolor, bgcolor );
    QString color = config.readEntry( "Highlight", "#800000" );
    pal.setColor( QColorGroup::Highlight, QColor(color) );
    color = config.readEntry( "HighlightedText", "#FFFFFF" );
    pal.setColor( QColorGroup::HighlightedText, QColor(color) );
    color = config.readEntry( "Text", "#000000" );
    pal.setColor( QColorGroup::Text, QColor(color) );
    color = config.readEntry( "ButtonText", "#000000" );
    pal.setColor( QPalette::Active, QColorGroup::ButtonText, QColor(color) );
    color = config.readEntry( "Base", "#FFFFFF" );
    pal.setColor( QColorGroup::Base, QColor(color) );

    pal.setColor( QPalette::Disabled, QColorGroup::Text,
      pal.color(QPalette::Active, QColorGroup::Background).dark() );

    setPalette( pal, TRUE );
}

void QPEApplication::systemMessage( const QCString &msg, const QByteArray &data)
{
#ifdef Q_WS_QWS
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "applyStyle()" ) {
    applyStyle();
    } else if ( msg == "setScreenSaverInterval(int)" ) {
    if ( type() == GuiServer ) {
      int time;
      stream >> time;
      setScreenSaverInterval(time);
    }
    } else if ( msg == "setScreenSaverIntervals(int,int,int)" ) {
    if ( type() == GuiServer ) {
      int t1,t2,t3;
      stream >> t1 >> t2 >> t3;
      setScreenSaverIntervals(t1,t2,t3);
    }
    } else if ( msg == "setBacklight(int)" ) {
    if ( type() == GuiServer ) {
      int bright;
      stream >> bright;
      setBacklight(bright);
    }
    } else if ( msg == "setDefaultRotation(int)" ) {
    if ( type() == GuiServer ) {
      int r;
      stream >> r;
      setDefaultRotation(r);
    }
    } else if ( msg == "shutdown()" ) {
    if ( type() == GuiServer )
      shutdown();
    } else if ( msg == "quit()" ) {
    if ( type() != GuiServer )
      tryQuit();
    } else if ( msg == "forceQuit()" ) {
    if ( type() != GuiServer )
      quit(); 
    } else if ( msg == "restart()" ) {
    if ( type() == GuiServer )
      restart();
    } else if ( msg == "grabKeyboard(QString)" ) {
    QString who;
    stream >> who;
    if ( who.isEmpty() )
      d->kbgrabber = 0;
    else if ( who != d->appName )
      d->kbgrabber = 1;
    else
      d->kbgrabber = 2;
    } else if ( msg == "language(QString)" ) {
    if ( type() == GuiServer ) {
      QString l;
      stream >> l;
      QString cl = getenv("LANG");
      if ( cl != l ) {
      if ( l.isNull() )
        unsetenv( "LANG" );
      else
        setenv( "LANG", l.latin1(), 1 );
      restart();
      }
    }
    } else if ( msg == "timeChange(QString)" ) {
    QString t;
    stream >> t;
    if ( t.isNull() )
      unsetenv( "TZ" );
    else
      setenv( "TZ", t.latin1(), 1 );
    // emit the signal so everyone else knows...
    emit timeChanged();
    } else if ( msg == "execute(QString)" ) {
    if ( type() == GuiServer ) {
      QString t;
      stream >> t;
      Global::execute( t );
    }
    } else if ( msg == "execute(QString,QString)" ) {
    if ( type() == GuiServer ) {
      QString t,d;
      stream >> t >> d;
      Global::execute( t, d );
    }
    } else if ( msg == "addAlarm(QDateTime,QCString,QCString,int)" ) {
    if ( type() == GuiServer ) {
      QDateTime when;
      QCString channel, message;
      int data;
      stream >> when >> channel >> message >> data;
      AlarmServer::addAlarm( when, channel, message, data );
    }
    } else if ( msg == "deleteAlarm(QDateTime,QCString,QCString,int)" ) {
    if ( type() == GuiServer ) {
      QDateTime when;
      QCString channel, message;
      int data;
      stream >> when >> channel >> message >> data;
      AlarmServer::deleteAlarm( when, channel, message, data );
    }
    } else if ( msg == "clockChange(bool)" ) {
    int tmp;
    stream >> tmp;
    emit clockChanged( tmp );
    } else if ( msg == "weekChange(bool)" ) {
    int tmp;
    stream >> tmp;
    emit weekChanged( tmp );
    } else if ( msg == "setDateFormat(DateFormat)" ) {
    DateFormat tmp;
    stream >> tmp;
    emit dateFormatChanged( tmp );
    } else if ( msg == "setVolume(int,int)" ) {
    int t,v;
    stream >> t >> v;
    setVolume(t,v);
    emit volumeChanged( muted );
    } else if ( msg == "volumeChange(bool)" ) {
    stream >> muted;
    setVolume();
    emit volumeChanged( muted );
  } else if ( msg == "setMic(int,int)") { // Added: 2002-02-08 by Jeremy Cowgar <jc@cowgar.com>
    int t,v;
    stream >> t >> v;
    setMic(t,v);
    emit micChanged( micMuted );
  } else if ( msg == "micChange(bool)" ) { // Added: 2002-02-08 by Jeremy Cowgar <jc@cowgar.com>
    stream >> micMuted;
    setMic();
    emit micChanged( micMuted );
    } else if ( msg == "setScreenSaverMode(int)" ) {
    if ( type() == GuiServer ) {
    int old = disable_suspend;
    stream >> disable_suspend;
    //qDebug("setScreenSaverMode(%d)", disable_suspend );
    if ( disable_suspend > old )
      setScreenSaverInterval( -1 );
    }
  }
#endif
}

/*!
  \internal
*/
bool QPEApplication::raiseAppropriateWindow()
{
    bool r=FALSE;
    // ########## raise()ing main window should raise and set active
    // ########## it and then all childen. This belongs in Qt/Embedded
    QWidget *top = d->qpe_main_widget;
    if ( !top ) top =mainWidget();
    if ( top && d->keep_running ) {
  if ( top->isVisible() )
      r = TRUE;
#ifdef Q_WS_QWS
  if ( !d->nomaximize )
      top->showMaximized();
  else
#endif
      top->show();
  top->raise();
  top->setActiveWindow();
    }
    QWidget *topm = activeModalWidget();
    if ( topm && topm != top ) {
  topm->show();
  topm->raise();
  topm->setActiveWindow();
  r = FALSE;
    }
    return r;
}

void QPEApplication::pidMessage( const QCString &msg, const QByteArray & data)
{
#ifdef Q_WS_QWS

    if ( msg == "quit()" ) {
  tryQuit();
    } else if ( msg == "quitIfInvisible()" ) {
  if ( d->qpe_main_widget && !d->qpe_main_widget->isVisible() )
      quit();
    } else if ( msg == "close()" ) {
  hideOrQuit();
    } else if ( msg == "disablePreload()" ) {
  d->preloaded = FALSE;
  d->keep_running = TRUE;
  /* so that quit will quit */
    } else if ( msg == "enablePreload()" ) {
  d->preloaded = TRUE;
  d->keep_running = TRUE;
  /* so next quit won't quit */
    } else if ( msg == "raise()" ) {
  d->keep_running = TRUE;
  d->notbusysent = FALSE;
  raiseAppropriateWindow();
    } else if ( msg == "flush()" ) {
  emit flush();
  // we need to tell the desktop
  QCopEnvelope e( "QPE/Desktop", "flushDone(QString)" );
  e << d->appName;
    } else if ( msg == "reload()" ) {
  emit reload();
    } else if ( msg == "setDocument(QString)" ) {
  d->keep_running = TRUE;
  QDataStream stream( data, IO_ReadOnly );
  QString doc;
  stream >> doc;
  QWidget *mw = mainWidget();
  if ( !mw )
      mw = d->qpe_main_widget;
  if ( mw )
      Global::setDocument( mw, doc );
    } else if ( msg == "nextView()" ) {
  if ( raiseAppropriateWindow() )
      emit appMessage( msg, data);
    } else {
  emit appMessage( msg, data);
    }
#endif
}


static bool setWidgetCaptionFromAppName( QWidget* /*mw*/, const QString& /*appName*/, const QString& /*appsPath*/ )
{
/*
    // This works but disable it for now until it is safe to apply
    // What is does is scan the .desktop files of all the apps for
    // the applnk that has the corresponding argv[0] as this program
    // then it uses the name stored in the .desktop file as the caption
    // for the main widget. This saves duplicating translations for
    // the app name in the program and in the .desktop files.

    AppLnkSet apps( appsPath );

    QList<AppLnk> appsList = apps.children();
    for ( QListIterator<AppLnk> it(appsList); it.current(); ++it ) {
  if ( (*it)->exec() == appName ) {
      mw->setCaption( (*it)->name() );
      return TRUE;
  }
    }
*/
    return FALSE;
}


/*!
  Sets \a mw as the mainWidget() and shows it. For small windows,
  consider passing TRUE for \a nomaximize rather than the default FALSE.

  \sa showMainDocumentWidget()
*/
void QPEApplication::showMainWidget( QWidget* mw, bool nomaximize )
{
    setWidgetCaptionFromAppName( mw, d->appName, qpeDir() + "apps" );

    d->nomaximize = nomaximize;
    d->qpe_main_widget = mw;
    d->sendQCopQ();
    if ( d->preloaded ) {
  if(d->forceshow) {
#ifdef Q_WS_QWS
      if ( !nomaximize )
    mw->showMaximized();
      else
#endif
      mw->show();
  }
    } else if ( d->keep_running ) {
#ifdef Q_WS_QWS
  if ( !nomaximize )
      mw->showMaximized();
  else
#endif
  mw->show();
    }
}

/*!
  Sets \a mw as the mainWidget() and shows it. For small windows,
  consider passing TRUE for \a nomaximize rather than the default FALSE.

  This calls designates the application as
  a \link docwidget.html document-oriented\endlink application.

  The \a mw widget must have a slot: setDocument(const QString&).

  \sa showMainWidget()
*/
void QPEApplication::showMainDocumentWidget( QWidget* mw, bool nomaximize )
{
    setWidgetCaptionFromAppName( mw, d->appName, qpeDir() + "apps" );

    if ( mw && argc() == 2 )
  Global::setDocument( mw, QString::fromUtf8(argv()[1]) );
    d->nomaximize = nomaximize;
    d->qpe_main_widget = mw;
    d->sendQCopQ();
    if ( d->preloaded ) {
  if(d->forceshow) {
#ifdef Q_WS_QWS
      if ( !nomaximize )
    mw->showMaximized();
      else
#endif
      mw->show();
  }
    } else if ( d->keep_running ) {
#ifdef Q_WS_QWS
  if ( !nomaximize )
      mw->showMaximized();
  else
#endif
  mw->show();
    }
}


/*!
  Sets that the application should continue running after processing
  qcop messages.  Normally if an application is started via a qcop message,
  the application will process the qcop message and then quit.  If while
  processing the qcop message it calls this function, then the application
  will show and start proper once it has finished processing qcop messages.

  \sa keepRunning()
*/
void QPEApplication::setKeepRunning()
{
    if ( qApp && qApp->inherits( "QPEApplication" ) ) {
  QPEApplication *qpeApp = (QPEApplication*)qApp;
  qpeApp->d->keep_running = TRUE;
    }
}

/*!
  Returns whether the application will quit after processing the current
  list of qcop messages.  

  \sa setKeepRunning()
*/
bool QPEApplication::keepRunning() const
{
    return d->keep_running;
}

/*!
  \internal
*/
void QPEApplication::internalSetStyle( const QString &style )
{
#if QT_VERSION >= 300
    if ( style == "QPE" ) {
  setStyle( new QPEStyle );
    } else {
  QStyle *s = QStyleFactory::create(style);
  if ( s ) setStyle(s);
    }
#else
    if ( style == "Windows" ) {
  setStyle( new QWindowsStyle );
    } else if ( style == "QPE" ) {
  setStyle( new QPEStyle );
    } else if ( style == "Light" ) {
  setStyle( new LightStyle );
    }
#ifndef QT_NO_STYLE_PLATINUM
    else if ( style == "Platinum" ) {
  setStyle( new QPlatinumStyle );
    }
#endif
#ifndef QT_NO_STYLE_MOTIF
    else if ( style == "Motif" ) {
  setStyle( new QMotifStyle );
    }
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
    else if ( style == "MotifPlus" ) {
  setStyle( new QMotifPlusStyle );
    }
#endif

	// HACK for Qt2 only
	else if ( style == "Liquid" ) {
		static void *lib = 0;	
		QStyle *sty = 0;
		

		if ( !lib ) {
			QString path = QPEApplication::qpeDir() + "/plugins/styles/" + "libliquid.so";		
			lib = ::dlopen ( path. local8Bit ( ), RTLD_NOW | RTLD_GLOBAL );
		}
		if ( lib ) {
			void *sym = ::dlsym ( lib, "allocate" );
		
			if ( sym )
				sty = ((QStyle * (*) ( )) sym ) ( );
		}
		if ( sty )
			setStyle ( sty );
	}
	// HACK for Qt2 only 
#endif
}

/*!
  \internal
*/
void QPEApplication::prepareForTermination(bool willrestart)
{
    if ( willrestart ) {
  // Draw a big wait icon, the image can be altered in later revisions
//  QWidget *d = QApplication::desktop();
  QImage img = Resource::loadImage( "launcher/new_wait" );
  QPixmap pix;
  pix.convertFromImage(img.smoothScale(1*img.width(), 1*img.height()));
  QLabel *lblWait = new QLabel(0, "wait hack!", QWidget::WStyle_Customize |
          QWidget::WStyle_NoBorder | QWidget::WStyle_Tool );
  lblWait->setPixmap( pix );
  lblWait->setAlignment( QWidget::AlignCenter );
  lblWait->show();
  lblWait->showMaximized();
    }
#ifndef SINGLE_APP
    { QCopEnvelope envelope("QPE/System", "forceQuit()"); }
    processEvents(); // ensure the message goes out.
    sleep(1); // You have 1 second to comply.
#endif
}

/*!
  \internal
*/
void QPEApplication::shutdown()
{
    // Implement in server's QPEApplication subclass
}

/*!
  \internal
*/
void QPEApplication::restart()
{
    // Implement in server's QPEApplication subclass
}

static QPtrDict<void>* stylusDict=0;
static void createDict()
{
    if ( !stylusDict )
  stylusDict = new QPtrDict<void>;
}

/*!
  Returns the current StylusMode for \a w.

  \sa setStylusOperation()
*/
QPEApplication::StylusMode QPEApplication::stylusOperation( QWidget* w )
{
    if ( stylusDict )
  return (StylusMode)(int)stylusDict->find(w);
    return LeftOnly;
}

/*!
    \enum QPEApplication::StylusMode

    \value LeftOnly the stylus only generates LeftButton
      events (the default).
    \value RightOnHold the stylus generates RightButton events
      if the user uses the press-and-hold gesture.

    See setStylusOperation().
*/

/*!
  Causes \a w to receive mouse events according to \a mode.

  \sa stylusOperation()
*/
void QPEApplication::setStylusOperation( QWidget* w, StylusMode mode )
{
    createDict();
    if ( mode == LeftOnly ) {
  stylusDict->remove(w);
  w->removeEventFilter(qApp);
    } else {
  stylusDict->insert(w,(void*)mode);
  connect(w,SIGNAL(destroyed()),qApp,SLOT(removeSenderFromStylusDict()));
  w->installEventFilter(qApp);
    }
}


/*!
  \reimp
*/
bool QPEApplication::eventFilter( QObject *o, QEvent *e )
{
    if ( stylusDict && e->type() >= QEvent::MouseButtonPress && e->type() <= QEvent::MouseMove ) {
  QMouseEvent* me = (QMouseEvent*)e;
  if ( me->button() == LeftButton ) {
      StylusMode mode = (StylusMode)(int)stylusDict->find(o);
      switch (mode) {
        case RightOnHold:
    switch ( me->type() ) {
      case QEvent::MouseButtonPress:
        d->presstimer = startTimer(500); // #### pref.
        d->presswidget = (QWidget*)o;
        d->presspos = me->pos();
        d->rightpressed = FALSE;
        break;
      case QEvent::MouseButtonRelease:
        if ( d->presstimer ) {
      killTimer(d->presstimer);
      d->presstimer = 0;
        }
        if ( d->rightpressed && d->presswidget ) {
      // Right released
      postEvent( d->presswidget,
          new QMouseEvent( QEvent::MouseButtonRelease, me->pos(),
            RightButton, LeftButton+RightButton ) );
      // Left released, off-widget
      postEvent( d->presswidget,
          new QMouseEvent( QEvent::MouseMove, QPoint(-1,-1),
            LeftButton, LeftButton ) );
      postEvent( d->presswidget,
          new QMouseEvent( QEvent::MouseButtonRelease, QPoint(-1,-1),
            LeftButton, LeftButton ) );
      d->rightpressed = FALSE;
      return TRUE; // don't send the real Left release
        }
        break;
      default:
        break;
    }
    break;
        default:
    ;
      }
  }
    } else if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease ) {
  QKeyEvent *ke = (QKeyEvent *)e;
  if ( ke->key() == Key_Enter ) {
      if ( o->isA( "QRadioButton" ) || o->isA( "QCheckBox" ) ) {
    postEvent( o, new QKeyEvent( e->type(), Key_Space, ' ',
        ke->state(), " ", ke->isAutoRepeat(), ke->count() ) );
    return TRUE;
      }
  }
    }

    return FALSE;
}

/*!
  \reimp
*/
void QPEApplication::timerEvent( QTimerEvent *e )
{
    if ( e->timerId() == d->presstimer && d->presswidget ) {
  // Right pressed
  postEvent( d->presswidget,
      new QMouseEvent( QEvent::MouseButtonPress, d->presspos,
      RightButton, LeftButton ) );
  killTimer( d->presstimer );
  d->presstimer = 0;
  d->rightpressed = TRUE;
    }
}

void QPEApplication::removeSenderFromStylusDict()
{
    stylusDict->remove((void*)sender());
    if ( d->presswidget == sender() )
  d->presswidget = 0;
}

/*!
  \internal
*/
bool QPEApplication::keyboardGrabbed() const
{
    return d->kbgrabber;
}


/*!
  Reverses the effect of grabKeyboard(). This is called automatically
  on program exit.
*/
void QPEApplication::ungrabKeyboard()
{
    QPEApplicationData* d = ((QPEApplication*)qApp)->d;
    if ( d->kbgrabber == 2 ) {
  QCopEnvelope e("QPE/System", "grabKeyboard(QString)" );
  e << QString::null;
  d->kbregrab = FALSE;
  d->kbgrabber = 0;
    }
}

/*!
  Grabs the keyboard such that the system's application launching
  keys no longer work, and instead they are receivable by this
  application.

  \sa ungrabKeyboard()
*/
void QPEApplication::grabKeyboard()
{
    QPEApplicationData* d = ((QPEApplication*)qApp)->d;
    if ( qApp->type() == QApplication::GuiServer )
  d->kbgrabber = 0;
    else {
  QCopEnvelope e("QPE/System", "grabKeyboard(QString)" );
  e << d->appName;
  d->kbgrabber = 2; // me
    }
}

/*!
  \reimp
*/
int QPEApplication::exec()
{
    d->sendQCopQ();
    if ( d->keep_running)
  //|| d->qpe_main_widget && d->qpe_main_widget->isVisible() )
  return QApplication::exec();

    {
    QCopEnvelope e("QPE/System", "closing(QString)" );
    e << d->appName;
    }
    processEvents();
    return 0;
}

/*!
  \internal
  External request for application to quit.  Quits if possible without
  loosing state.
*/
void QPEApplication::tryQuit()
{
    if ( activeModalWidget() || strcmp( argv()[0], "embeddedkonsole") == 0 )
  return; // Inside modal loop or konsole. Too hard to save state.
    {
       QCopEnvelope e("QPE/System", "closing(QString)" );
       e << d->appName;
    }
    processEvents();

    quit();
}

/*!
  \internal
  User initiated quit.  Makes the window 'Go Away'.  If preloaded this means
  hiding the window.  If not it means quitting the application.
  As this is user initiated we don't need to check state.
*/
void QPEApplication::hideOrQuit()
{
    // notify of our demise :)
    {
       QCopEnvelope e("QPE/System", "closing(QString)" );
       e << d->appName;
    }
    processEvents();
    if ( d->preloaded && d->qpe_main_widget )
  d->qpe_main_widget->hide();
    else
  quit();
}

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)

// The libraries with the skiff package (and possibly others) have
// completely useless implementations of builtin new and delete that
// use about 50% of your CPU. Here we revert to the simple libc
// functions.

void* operator new[](size_t size)
{
    return malloc(size);
}

void* operator new(size_t size)
{
    return malloc(size);
}

void operator delete[](void* p)
{
    free(p);
}

void operator delete[](void* p, size_t /*size*/)
{
    free(p);
}

void operator delete(void* p)
{
    free(p);
}

void operator delete(void* p, size_t /*size*/)
{
    free(p);
}

#endif

#if ( QT_VERSION <= 230 ) && !defined(SINGLE_APP)
#include <qwidgetlist.h>
#include <qgfx_qws.h>
extern QRect qt_maxWindowRect;
void qt_setMaxWindowRect(const QRect& r)
{
    qt_maxWindowRect = qt_screen->mapFromDevice(r,
  qt_screen->mapToDevice(QSize(qt_screen->width(),qt_screen->height())));
    // Re-resize any maximized windows
    QWidgetList* l = QApplication::topLevelWidgets();
    if ( l ) {
        QWidget *w = l->first();
        while ( w ) {
            if ( w->isVisible() && w->isMaximized() )
            {
                w->showMaximized();
            }
            w = l->next();
        }
        delete l;
    }
}
#endif
