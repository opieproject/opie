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
*/
#define QTOPIA_INTERNAL_LANGLIST
#include <stdlib.h>
#include <unistd.h>
#ifndef Q_OS_MACX
#include <linux/limits.h> // needed for some toolchains (PATH_MAX)
#endif
#include <qfile.h>
#include <qqueue.h>
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
#include <qtextcodec.h>
#include <qevent.h>
#include <qtooltip.h>
#include <qsignal.h>
#include <qmainwindow.h>
#include <qwidgetlist.h>
#include <qpixmapcache.h>

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#define QTOPIA_INTERNAL_INITAPP
#include "qpeapplication.h"
#include "qpestyle.h"
#include "styleinterface.h"
#if QT_VERSION >= 300
#include <qstylefactory.h>
#else
#include <qplatinumstyle.h>
#include <qwindowsstyle.h>
#include <qmotifstyle.h>
#include <qmotifplusstyle.h>
#include "lightstyle.h"

#include <qpe/qlibrary.h>
#endif
#include "global.h"
#include "resource.h"
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
#include "qutfcodec.h"
#endif
#include "config.h"
#include "network.h"
#ifdef QWS
#include "fontmanager.h"
#endif

#include "alarmserver.h"
#include "applnk.h"
#include "qpemenubar.h"
#include "textcodecinterface.h"
#include "imagecodecinterface.h"

#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#ifndef QT_NO_SOUND
#include <sys/soundcard.h>
#endif
#include "qt_override_p.h"


class QPEApplicationData
{
public:
	QPEApplicationData ( )
		: presstimer( 0 ), presswidget( 0 ), rightpressed( false ), kbgrabbed( false ),
		  notbusysent( false ), preloaded( false ), forceshow( false ), nomaximize( false ),
		  keep_running( true ), qcopQok( false ), qpe_main_widget( 0 )

	{}

	int presstimer;
	QWidget* presswidget;
	QPoint presspos;

	bool rightpressed : 1;
	bool kbgrabbed    : 1;
	bool notbusysent  : 1;
	bool preloaded    : 1;
	bool forceshow    : 1;
	bool nomaximize   : 1;
	bool keep_running : 1;
        bool qcopQok      : 1;


        QStringList langs;
	QString appName;
	struct QCopRec
	{
		QCopRec( const QCString &ch, const QCString &msg,
		         const QByteArray &d ) :
				channel( ch ), message( msg ), data( d )
		{ }

		QCString channel;
		QCString message;
		QByteArray data;
	};
	QWidget* qpe_main_widget;
        QGuardedPtr<QWidget> lastraised;
	QQueue<QCopRec> qcopq;
        QString styleName;
	QString decorationName;

	void enqueueQCop( const QCString &ch, const QCString &msg,
	                  const QByteArray &data )
	{
		qcopq.enqueue( new QCopRec( ch, msg, data ) );
	}
	void sendQCopQ()
	{
            if (!qcopQok )
                return;

		QCopRec * r;

           while((r=qcopq.dequeue())) {
               // remove from queue before sending...
               // event loop can come around again before getting
               // back from sendLocally
#ifndef QT_NO_COP
               QCopChannel::sendLocally( r->channel, r->message, r->data );
#endif

	       delete r;
           }
	}
	static void show_mx(QWidget* mw, bool nomaximize,  const QString & = QString::null )
	{

            // ugly hack, remove that later after finding a sane solution
            // Addendum: Only Sharp currently has models with high resolution but (physically) small displays,
            // so this is only useful if QT_QWS_SIMPAD is NOT defined. E.g. SIMpad has 800x600 but has
            // a (physically) large enough display to use the small icons
#if defined(OPIE_HIGH_RES_SMALL_PHY)
            if ( QPEApplication::desktop() ->width() >= 600 && ( mw->inherits("QMainWindow") || mw->isA("QMainWindow") ) )  {
                ( (  QMainWindow* ) mw )->setUsesBigPixmaps( true );
            }
#endif

		QPEApplication::showWidget( mw, nomaximize );
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


	void show(QWidget* mw, bool nomax)
	{
		setWidgetCaptionFromAppName( mw, appName, QPEApplication::qpeDir() + "apps" );
		nomaximize = nomax;
		qpe_main_widget = mw;
                qcopQok = TRUE;
#ifndef QT_NO_COP

		sendQCopQ();
#endif

		if ( preloaded ) {
			if (forceshow)
				show_mx(mw, nomax);
		}
		else if ( keep_running ) {
			show_mx(mw, nomax);
		}
	}

	void loadTextCodecs()
	{
		QString path = QPEApplication::qpeDir() + "/plugins/textcodecs";
#ifdef Q_OS_MACX
		QDir dir( path, "lib*.dylib" );
#else
		QDir dir( path, "lib*.so" );
#endif
		QStringList list;
		if ( dir. exists ( ))
		    list = dir.entryList();
		QStringList::Iterator it;
		for ( it = list.begin(); it != list.end(); ++it ) {
			TextCodecInterface *iface = 0;
			QLibrary *lib = new QLibrary( path + "/" + *it );
			if ( lib->queryInterface( IID_QtopiaTextCodec, (QUnknownInterface**)&iface ) == QS_OK && iface ) {
				QValueList<int> mibs = iface->mibEnums();
				for (QValueList<int>::ConstIterator i = mibs.begin(); i != mibs.end(); ++i) {
					(void)iface->createForMib(*i);
					// ### it exists now; need to remember if we can delete it
				}
			}
			else {
				lib->unload();
				delete lib;
			}
		}
	}

	void loadImageCodecs()
	{
		QString path = QPEApplication::qpeDir() + "/plugins/imagecodecs";
#ifdef Q_OS_MACX
		QDir dir( path, "lib*.dylib" );
#else
		QDir dir( path, "lib*.so" );
#endif
		QStringList list;
		if ( dir. exists ( ))
		    list = dir.entryList();
		QStringList::Iterator it;
		for ( it = list.begin(); it != list.end(); ++it ) {
			ImageCodecInterface *iface = 0;
			QLibrary *lib = new QLibrary( path + "/" + *it );
			if ( lib->queryInterface( IID_QtopiaImageCodec, (QUnknownInterface**)&iface ) == QS_OK && iface ) {
				QStringList formats = iface->keys();
				for (QStringList::ConstIterator i = formats.begin(); i != formats.end(); ++i) {
					(void)iface->installIOHandler(*i);
					// ### it exists now; need to remember if we can delete it
				}
			}
			else {
				lib->unload();
				delete lib;
			}
		}
	}
};

class ResourceMimeFactory : public QMimeSourceFactory
{
public:
        ResourceMimeFactory() : resImage( 0 )
	{
		setFilePath( Global::helpPath() );
		setExtensionType( "html", "text/html;charset=UTF-8" );
	}
        ~ResourceMimeFactory() {
            delete resImage;
        }

	const QMimeSource* data( const QString& abs_name ) const
	{
		const QMimeSource * r = QMimeSourceFactory::data( abs_name );
		if ( !r ) {
			int sl = abs_name.length();
			do {
				sl = abs_name.findRev( '/', sl - 1 );
				QString name = sl >= 0 ? abs_name.mid( sl + 1 ) : abs_name;
				int dot = name.findRev( '.' );
				if ( dot >= 0 )
					name = name.left( dot );
				QImage img = Resource::loadImage( name );
				if ( !img.isNull() ) {
                                    delete resImage;
                                    resImage = new QImageDrag( img );
                                    r = resImage;
                                }
			}
			while ( !r && sl > 0 );
		}
		return r;
	}
private:
    mutable QImageDrag *resImage;
};

static int& hack(int& i)
{
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
    // These should be created, but aren't in Qt 2.3.0
    (void)new QUtf8Codec;
    (void)new QUtf16Codec;
#endif
    return i;
}

static int muted = 0;
static int micMuted = 0;

static void setVolume( int t = 0, int percent = -1 )
{
	switch ( t ) {
		case 0: {
				Config cfg( "qpe" );
				cfg.setGroup( "Volume" );
				if ( percent < 0 )
					percent = cfg.readNumEntry( "VolumePercent", 50 );
#ifndef QT_NO_SOUND
				int fd = 0;
				if ( ( fd = open( "/dev/mixer", O_RDWR ) ) >= 0 ) {
					int vol = muted ? 0 : percent;
					// set both channels to same volume
					vol |= vol << 8;
					ioctl( fd, MIXER_WRITE( SOUND_MIXER_VOLUME ), &vol );
					::close( fd );
				}
#endif
			}
			break;
	}
}

static void setMic( int t = 0, int percent = -1 )
{
	switch ( t ) {
		case 0: {
				Config cfg( "qpe" );
				cfg.setGroup( "Volume" );
				if ( percent < 0 )
					percent = cfg.readNumEntry( "Mic", 50 );

#ifndef QT_NO_SOUND
				int fd = 0;
				int mic = micMuted ? 0 : percent;
				if ( ( fd = open( "/dev/mixer", O_RDWR ) ) >= 0 ) {
					ioctl( fd, MIXER_WRITE( SOUND_MIXER_MIC ), &mic );
					::close( fd );
				}
#endif
			}
			break;
	}
}


static void setBass( int t = 0, int percent = -1 )
{
	switch ( t ) {
		case 0: {
				Config cfg( "qpe" );
				cfg.setGroup( "Volume" );
				if ( percent < 0 )
					percent = cfg.readNumEntry( "BassPercent", 50 );

#ifndef QT_NO_SOUND
				int fd = 0;
				int bass = percent;
                                if ( ( fd = open( "/dev/mixer", O_RDWR ) ) >= 0 ) {
					ioctl( fd, MIXER_WRITE( SOUND_MIXER_BASS ), &bass );
					::close( fd );
				}
#endif
			}
			break;
	}
}


static void setTreble( int t = 0, int percent = -1 )
{
	switch ( t ) {
		case 0: {
				Config cfg( "qpe" );
				cfg.setGroup( "Volume" );
				if ( percent < 0 )
					percent = cfg.readNumEntry( "TreblePercent", 50 );

#ifndef QT_NO_SOUND
				int fd = 0;
				int treble = percent;
				if ( ( fd = open( "/dev/mixer", O_RDWR ) ) >= 0 ) {
					ioctl( fd, MIXER_WRITE( SOUND_MIXER_TREBLE ), &treble );
					::close( fd );
				}
#endif
			}
			break;
	}
}


/**
  \class QPEApplication
  \brief The QPEApplication class implements various system services
    that are available to all Qtopia applications.

  Simply by using QPEApplication instead of QApplication, a standard Qt
  application becomes a Qtopia application. It automatically follows
  style changes, quits and raises, and in the
  case of \link docwidget.html document-oriented\endlink applications,
  changes the currently displayed document in response to the environment.

  To create a \link docwidget.html document-oriented\endlink
  application use showMainDocumentWidget(); to create a
  non-document-oriented application use showMainWidget(). The
  keepRunning() function indicates whether the application will
  continue running after it's processed the last \link qcop.html
  QCop\endlink message. This can be changed using setKeepRunning().

  A variety of signals are emitted when certain events occur, for
  example, timeChanged(), clockChanged(), weekChanged(),
  dateFormatChanged() and volumeChanged(). If the application receives
  a \link qcop.html QCop\endlink message on the application's
  QPE/Application/\e{appname} channel, the appMessage() signal is
  emitted. There are also flush() and reload() signals, which
  are emitted when synching begins and ends respectively - upon these
  signals, the application should save and reload any data
  files that are involved in synching. Most of these signals will initially
  be received and unfiltered through the appMessage() signal.

  This class also provides a set of useful static functions. The
  qpeDir() and documentDir() functions return the respective paths.
  The grabKeyboard() and ungrabKeyboard() functions are used to
  control whether the application takes control of the device's
  physical buttons (e.g. application launch keys). The stylus' mode of
  operation is set with setStylusOperation() and retrieved with
  stylusOperation(). There are also setInputMethodHint() and
  inputMethodHint() functions.

  \ingroup qtopiaemb
*/

/*!
  \fn void QPEApplication::clientMoused()

  \internal
*/

/*!
  \fn void QPEApplication::timeChanged();
  This signal is emitted when the time changes outside the normal
  passage of time, i.e. if the time is set backwards or forwards.
*/

/*!
  \fn void QPEApplication::clockChanged( bool ampm );

  This signal is emitted when the user changes the clock's style. If
  \a ampm is TRUE, the user wants a 12-hour AM/PM clock, otherwise,
  they want a 24-hour clock.
*/

/*!
    \fn void QPEApplication::volumeChanged( bool muted )

    This signal is emitted whenever the mute state is changed. If \a
    muted is TRUE, then sound output has been muted.
*/

/*!
    \fn void QPEApplication::weekChanged( bool startOnMonday )

    This signal is emitted if the week start day is changed. If \a
    startOnMonday is TRUE then the first day of the week is Monday; if
    \a startOnMonday is FALSE then the first day of the week is
    Sunday.
*/

/*!
    \fn void QPEApplication::dateFormatChanged(DateFormat)

    This signal is emitted whenever the date format is changed.
*/

/*!
    \fn void QPEApplication::flush()

    ###
*/

/*!
  \fn void QPEApplication::reload()

*/



void QPEApplication::processQCopFile()
{
    QString qcopfn("/tmp/qcop-msg-");
    qcopfn += d->appName; // append command name

    QFile f(qcopfn);
    if ( f.open(IO_ReadWrite) ) {
#ifndef Q_OS_WIN32
	flock(f.handle(), LOCK_EX);
#endif
	QDataStream ds(&f);
	QCString channel, message;
	QByteArray data;
	while(!ds.atEnd()) {
	    ds >> channel >> message >> data;
	    d->enqueueQCop(channel,message,data);
	}
	::ftruncate(f.handle(), 0);
#ifndef Q_OS_WIN32
	f.flush();
	flock(f.handle(), LOCK_UN);
#endif
    }
#endif
}


/*!
  \fn void QPEApplication::appMessage( const QCString& msg, const QByteArray& data )

  This signal is emitted when a message is received on this
  application's QPE/Application/<i>appname</i> \link qcop.html
  QCop\endlink channel.

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
  Note that messages received here may be processed by qpe application
  and emitted as signals, such as flush() and reload().
*/

/*!
  Constructs a QPEApplication just as you would construct
  a QApplication, passing \a argc, \a argv, and \a t.

  For applications, \a t should be the default, GuiClient. Only
  the Qtopia server passes GuiServer.
*/
QPEApplication::QPEApplication( int & argc, char **argv, Type t )
		: QApplication( hack(argc), argv, t ), pidChannel( 0 )
{
        QPixmapCache::setCacheLimit(256); // sensible default for smaller devices.

	d = new QPEApplicationData;
	d->loadTextCodecs();
	d->loadImageCodecs();
       	int dw = desktop() ->width();

	if ( dw < 200 ) {
		setFont( QFont( "vera", 8 ) );
		AppLnk::setSmallIconSize( 10 );
		AppLnk::setBigIconSize( 28 );
	}
#if defined(OPIE_HIGH_RES_SMALL_PHY)
	else if ( dw > 600 ) {
                setFont( QFont( "vera", 16 ) );
                AppLnk::setSmallIconSize( 24 );
		AppLnk::setBigIconSize( 48 );
	}
#endif
	else if ( dw > 200 ) {
		setFont( QFont( "vera", 10 ) );
		AppLnk::setSmallIconSize( 14 );
		AppLnk::setBigIconSize( 32 );
	}

	QMimeSourceFactory::setDefaultFactory( new ResourceMimeFactory );

	connect( this, SIGNAL( lastWindowClosed() ), this, SLOT( hideOrQuit() ) );


	sysChannel = new QCopChannel( "QPE/System", this );
	connect( sysChannel, SIGNAL( received(const QCString&,const QByteArray&) ),
	         this, SLOT( systemMessage(const QCString&,const QByteArray&) ) );

/* COde now in initapp */
#if 0
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)

	QString qcopfn( "/tmp/qcop-msg-" );
	qcopfn += QString( argv[ 0 ] ); // append command name

	QFile f( qcopfn );
	if ( f.open( IO_ReadOnly ) ) {
		flock( f.handle(), LOCK_EX );
	}



	QCString channel = QCString( argv[ 0 ] );
	channel.replace( QRegExp( ".*/" ), "" );
	d->appName = channel;
	channel = "QPE/Application/" + channel;
	pidChannel = new QCopChannel( channel, this );
	connect( pidChannel, SIGNAL( received(const QCString&,const QByteArray&) ),
	         this, SLOT( pidMessage(const QCString&,const QByteArray&) ) );

	if ( f.isOpen() ) {
		d->keep_running = FALSE;
		QDataStream ds( &f );
		QCString channel, message;
		QByteArray data;
		while ( !ds.atEnd() ) {
			ds >> channel >> message >> data;
			d->enqueueQCop( channel, message, data );
		}

		flock( f.handle(), LOCK_UN );
		f.close();
		f.remove();
	}

	for ( int a = 0; a < argc; a++ ) {
		if ( qstrcmp( argv[ a ], "-preload" ) == 0 ) {
			argv[ a ] = argv[ a + 1 ];
			a++;
			d->preloaded = TRUE;
			argc -= 1;
		}
		else if ( qstrcmp( argv[ a ], "-preload-show" ) == 0 ) {
			argv[ a ] = argv[ a + 1 ];
			a++;
			d->preloaded = TRUE;
			d->forceshow = TRUE;
			argc -= 1;
		}
	}

	/* overide stored arguments */
	setArgs( argc, argv );

#endif
#else
        initApp( argc, argv );
#endif
	//  qwsSetDecoration( new QPEDecoration() );

#ifndef QT_NO_TRANSLATION

	d->langs = Global::languageList();
	for ( QStringList::ConstIterator it = d->langs.begin(); it != d->langs.end(); ++it ) {
		QString lang = *it;

                installTranslation( lang + "/libopie.qm");
		installTranslation( lang + "/libqpe.qm" );
		installTranslation( lang + "/" + d->appName + ".qm" );


		//###language/font hack; should look it up somewhere
#ifdef QWS

                if ( lang == "ja" || lang == "zh_CN" || lang == "zh_TW" || lang == "ko" ) {
                    QFont fn = FontManager::unicodeFont( FontManager::Proportional );
                    setFont( fn );
                }
#endif
        }
#endif

	applyStyle();

	if ( type() == GuiServer ) {
		setVolume();
	}

	installEventFilter( this );

	QPEMenuToolFocusManager::initialize();

#ifdef QT_NO_QWS_CURSOR
	// if we have no cursor, probably don't want tooltips
	QToolTip::setEnabled( FALSE );
#endif
}


#ifdef QTOPIA_INTERNAL_INITAPP
void QPEApplication::initApp( int argc, char **argv )
{
    delete pidChannel;
    d->keep_running = TRUE;
    d->preloaded = FALSE;
    d->forceshow = FALSE;

    QCString channel = QCString(argv[0]);

    channel.replace(QRegExp(".*/"),"");
    d->appName = channel;

    #if QT_VERSION > 235
    qt_fbdpy->setIdentity( channel ); // In Qt/E 2.3.6
    #endif

    channel = "QPE/Application/" + channel;
    pidChannel = new QCopChannel( channel, this);
    connect( pidChannel, SIGNAL(received(const QCString&,const QByteArray&)),
	    this, SLOT(pidMessage(const QCString&,const QByteArray&)));



    processQCopFile();
    d->keep_running = d->qcopq.isEmpty();

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

    /* install translation here */
    for ( QStringList::ConstIterator it = d->langs.begin(); it != d->langs.end(); ++it )
        installTranslation( (*it) + "/" + d->appName + ".qm" );
}
#endif


static QPtrDict<void>* inputMethodDict = 0;
static void createInputMethodDict()
{
	if ( !inputMethodDict )
		inputMethodDict = new QPtrDict<void>;
}

/*!
  Returns the currently set hint to the system as to whether
  widget \a w has any use for text input methods.


  \sa setInputMethodHint() InputMethodHint
*/
QPEApplication::InputMethodHint QPEApplication::inputMethodHint( QWidget * w )
{
	if ( inputMethodDict && w )
		return ( InputMethodHint ) ( int ) inputMethodDict->find( w );
	return Normal;
}

/*!
  \enum QPEApplication::InputMethodHint

  \value Normal the application sometimes needs text input (the default).
  \value AlwaysOff the application never needs text input.
  \value AlwaysOn the application always needs text input.
*/

/*!
  Hints to the system that widget \a w has use for text input methods
  as specified by \a mode.

  \sa inputMethodHint() InputMethodHint
*/
void QPEApplication::setInputMethodHint( QWidget * w, InputMethodHint mode )
{
	createInputMethodDict();
	if ( mode == Normal ) {
		inputMethodDict->remove
		( w );
	}
	else {
		inputMethodDict->insert( w, ( void* ) mode );
	}
}

class HackDialog : public QDialog
{
public:
	void acceptIt()
	{
		accept();
	}
	void rejectIt()
	{
		reject();
	}
};


void QPEApplication::mapToDefaultAction( QWSKeyEvent * ke, int key )
{
	// specialised actions for certain widgets. May want to
	// add more stuff here.
	if ( activePopupWidget() && activePopupWidget() ->inherits( "QListBox" )
	        && activePopupWidget() ->parentWidget()
	        && activePopupWidget() ->parentWidget() ->inherits( "QComboBox" ) )
		key = Qt::Key_Return;

	if ( activePopupWidget() && activePopupWidget() ->inherits( "QPopupMenu" ) )
		key = Qt::Key_Return;

#ifdef QWS

	ke->simpleData.keycode = key;
#endif
}

class HackWidget : public QWidget
{
public:
	bool needsOk()
	{
		return ( getWState() & WState_Reserved1 );
	}
};

/*!
  \internal
*/

#ifdef QWS
bool QPEApplication::qwsEventFilter( QWSEvent * e )
{
	if ( !d->notbusysent && e->type == QWSEvent::Focus ) {
		if ( qApp->type() != QApplication::GuiServer ) {
			QCopEnvelope e( "QPE/System", "notBusy(QString)" );
			e << d->appName;
		}
		d->notbusysent = TRUE;
	}
	if ( type() == GuiServer ) {
		switch ( e->type ) {
			case QWSEvent::Mouse:
				if ( e->asMouse() ->simpleData.state && !QWidget::find( e->window() ) )
					emit clientMoused();
				break;
			default:
				break;
		}
	}
	if ( e->type == QWSEvent::Key ) {
		QWSKeyEvent *ke = ( QWSKeyEvent * ) e;
		if ( ke->simpleData.keycode == Qt::Key_F33 ) {
			// Use special "OK" key to press "OK" on top level widgets
			QWidget * active = activeWindow();
			QWidget *popup = 0;
			if ( active && active->isPopup() ) {
				popup = active;
				active = active->parentWidget();
			}
			if ( active && ( int ) active->winId() == ke->simpleData.window &&
			        !active->testWFlags( WStyle_Customize | WType_Popup | WType_Desktop ) ) {
				if ( ke->simpleData.is_press ) {
					if ( popup )
						popup->close();
					if ( active->inherits( "QDialog" ) ) {
						HackDialog * d = ( HackDialog * ) active;
						d->acceptIt();
						return TRUE;
					}
					else if ( ( ( HackWidget * ) active ) ->needsOk() ) {
						QSignal s;
						s.connect( active, SLOT( accept() ) );
						s.activate();
					}
					else {
						// do the same as with the select key: Map to the default action of the widget:
						mapToDefaultAction( ke, Qt::Key_Return );
					}
				}
			}
		}
		else if ( ke->simpleData.keycode == Qt::Key_F30 ) {
			// Use special "select" key to do whatever default action a widget has
			mapToDefaultAction( ke, Qt::Key_Space );
		}
		else if ( ke->simpleData.keycode == Qt::Key_Escape &&
		          ke->simpleData.is_press ) {
			// Escape key closes app if focus on toplevel
			QWidget * active = activeWindow();
			if ( active && active->testWFlags( WType_TopLevel ) &&
			        ( int ) active->winId() == ke->simpleData.window &&
			        !active->testWFlags( WStyle_Dialog | WStyle_Customize | WType_Popup | WType_Desktop ) ) {
				if ( active->inherits( "QDialog" ) ) {
					HackDialog * d = ( HackDialog * ) active;
					d->rejectIt();
					return TRUE;
				}
				else if ( strcmp( argv() [ 0 ], "embeddedkonsole" ) != 0 ) {
					active->close();
				}
			}
		}
		else if ( ke->simpleData.keycode >= Qt::Key_F1 && ke->simpleData.keycode <= Qt::Key_F29 ) {
			// this should be if ( ODevice::inst ( )-> buttonForKeycode ( ... ))
			// but we cannot access libopie function within libqpe :(

			QWidget * active = activeWindow ( );
			if ( active && ((int) active-> winId ( ) == ke-> simpleData.window )) {
				if ( d-> kbgrabbed ) { // we grabbed the keyboard
					QChar ch ( ke-> simpleData.unicode );
					QKeyEvent qke ( ke-> simpleData. is_press ? QEvent::KeyPress : QEvent::KeyRelease,
					                ke-> simpleData.keycode,
					                ch. latin1 ( ),
					                ke-> simpleData.modifiers,
					                QString ( ch ),
					                ke-> simpleData.is_auto_repeat, 1 );

					QObject *which = QWidget::keyboardGrabber ( );
					if ( !which )
						which = QApplication::focusWidget ( );
					if ( !which )
						which = QApplication::activeWindow ( );
					if ( !which )
						which = qApp;

					QApplication::sendEvent ( which, &qke );
				}
				else { // we didn't grab the keyboard, so send the event to the launcher
					QCopEnvelope e ( "QPE/Launcher", "deviceButton(int,int,int)" );
					e << int( ke-> simpleData.keycode ) << int( ke-> simpleData. is_press ) << int( ke-> simpleData.is_auto_repeat );
				}
			}
			return true;
		}
	}
	if ( e->type == QWSEvent::Focus ) {
		QWSFocusEvent * fe = ( QWSFocusEvent* ) e;
		if ( !fe->simpleData.get_focus ) {
			QWidget * active = activeWindow();
			while ( active && active->isPopup() ) {
				active->close();
				active = activeWindow();
			}
		}
		else {
			// make sure our modal widget is ALWAYS on top
			QWidget *topm = activeModalWidget();
			if ( topm && static_cast<int>( topm->winId() ) != fe->simpleData.window) {
				topm->raise();
			}
		}
		if ( fe->simpleData.get_focus && inputMethodDict ) {
			InputMethodHint m = inputMethodHint( QWidget::find( e->window() ) );
			if ( m == AlwaysOff )
				Global::hideInputMethod();
			if ( m == AlwaysOn )
				Global::showInputMethod();
		}
	}


	return QApplication::qwsEventFilter( e );
}
#endif

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
	const char * base = getenv( "OPIEDIR" );
	if ( base )
		return QString( base ) + "/";

	return QString( "../" );
}

/*!
  Returns the user's current Document directory. There is a trailing "/".
  .. well, it does now,, and there's no trailing '/'
*/
QString QPEApplication::documentDir()
{
	const char* base = getenv( "HOME");
	if ( base )
		return QString( base ) + "/Documents";

	return QString( "../Documents" );
}

static int deforient = -1;

/*!
  \internal
*/
int QPEApplication::defaultRotation()
{
	if ( deforient < 0 ) {
		QString d = getenv( "QWS_DISPLAY" );
		if ( d.contains( "Rot90" ) ) {
			deforient = 90;
		}
		else if ( d.contains( "Rot180" ) ) {
			deforient = 180;
		}
		else if ( d.contains( "Rot270" ) ) {
			deforient = 270;
		}
		else {
			deforient = 0;
		}
	}
	return deforient;
}

/*!
  \internal
*/
void QPEApplication::setDefaultRotation( int r )
{
	if ( qApp->type() == GuiServer ) {
		deforient = r;
		setenv( "QWS_DISPLAY", QString( "Transformed:Rot%1:0" ).arg( r ).latin1(), 1 );
		Config config("qpe");
		config.setGroup( "Rotation" );
		config.writeEntry( "Rot", r );
	}
	else {
#ifndef QT_NO_COP
		{ QCopEnvelope e( "QPE/System", "setDefaultRotation(int)" );
			e << r;
		}
#endif

	}
}

#include <qgfx_qws.h>
#include <qwindowsystem_qws.h>

extern void qws_clearLoadedFonts();

void QPEApplication::setCurrentMode( int x, int y, int depth )
{
    // Reset the caches
    qws_clearLoadedFonts();
    QPixmapCache::clear();

    // Change the screen mode
    qt_screen->setMode(x, y, depth);

    if ( qApp->type() == GuiServer ) {
        // Reconfigure the GuiServer
        qwsServer->beginDisplayReconfigure();
        qwsServer->endDisplayReconfigure();

        // Get all the running apps to reset
        QCopEnvelope env( "QPE/System", "reset()" );
    }
}

void QPEApplication::reset() {
    // Reconnect to the screen
    qt_screen->disconnect();
    qt_screen->connect( QString::null );

    // Redraw everything
    applyStyle();
}

/*!
  \internal
*/
void QPEApplication::applyStyle()
{
	Config config( "qpe" );
	config.setGroup( "Appearance" );

#if QT_VERSION > 233
#if !defined(OPIE_NO_OVERRIDE_QT)
	// don't block ourselves ...
	Opie::force_appearance = 0;

	static QString appname = Opie::binaryName ( );

	QStringList ex = config. readListEntry ( "NoStyle", ';' );
	int nostyle = 0;
	for ( QStringList::Iterator it = ex. begin ( ); it != ex. end ( ); ++it ) {
		if ( QRegExp (( *it ). mid ( 1 ), false, true ). find ( appname, 0 ) >= 0 ) {
			nostyle = ( *it ). left ( 1 ). toInt ( 0, 32 );
			break;
		}
	}
#else
        int nostyle = 0;
#endif

	// Widget style
	QString style = config.readEntry( "Style", "FlatStyle" );

	// don't set a custom style
	if ( nostyle & Opie::Force_Style )
		style = "FlatStyle";

	internalSetStyle ( style );

	// Colors - from /etc/colors/Liquid.scheme
	QColor bgcolor( config.readEntry( "Background", "#E0E0E0" ) );
	QColor btncolor( config.readEntry( "Button", "#96c8fa" ) );
	QPalette pal( btncolor, bgcolor );
	QString color = config.readEntry( "Highlight", "#73adef" );
	pal.setColor( QColorGroup::Highlight, QColor( color ) );
	color = config.readEntry( "HighlightedText", "#FFFFFF" );
	pal.setColor( QColorGroup::HighlightedText, QColor( color ) );
	color = config.readEntry( "Text", "#000000" );
	pal.setColor( QColorGroup::Text, QColor( color ) );
	color = config.readEntry( "ButtonText", "#000000" );
	pal.setColor( QPalette::Active, QColorGroup::ButtonText, QColor( color ) );
	color = config.readEntry( "Base", "#FFFFFF" );
	pal.setColor( QColorGroup::Base, QColor( color ) );

	pal.setColor( QPalette::Disabled, QColorGroup::Text,
	              pal.color( QPalette::Active, QColorGroup::Background ).dark() );

	setPalette( pal, TRUE );

	// Window Decoration
	QString dec = config.readEntry( "Decoration", "Flat" );

	// don't set a custom deco
	if ( nostyle & Opie::Force_Decoration )
		dec = "";

	//qDebug ( "Setting Deco: %s -- old %s (%d)", dec.latin1(), d-> decorationName.latin1(), nostyle);

	if ( dec != d->decorationName ) {
		qwsSetDecoration( new QPEDecoration( dec ) );
		d->decorationName = dec;
	}

	// Font
	QString ff = config.readEntry( "FontFamily", font().family() );
	int fs = config.readNumEntry( "FontSize", font().pointSize() );

	// don't set a custom font
	if ( nostyle & Opie::Force_Font ) {
		ff = "Vera";
		fs = 10;
	}

	setFont ( QFont ( ff, fs ), true );

#if !defined(OPIE_NO_OVERRIDE_QT)
	// revert to global blocking policy ...
	Opie::force_appearance = config. readBoolEntry ( "ForceStyle", false ) ? Opie::Force_All : Opie::Force_None;
	Opie::force_appearance &= ~nostyle;
#endif
#endif
}

void QPEApplication::systemMessage( const QCString& msg, const QByteArray& data )
{
#ifdef Q_WS_QWS
	QDataStream stream( data, IO_ReadOnly );
	if ( msg == "applyStyle()" ) {
		applyStyle();
	}
	else if ( msg == "toggleApplicationMenu()" ) {
		QWidget *active = activeWindow ( );

		if ( active ) {
			QPEMenuToolFocusManager *man = QPEMenuToolFocusManager::manager ( );
			bool oldactive = man-> isActive ( );

			man-> setActive( !man-> isActive() );

			if ( !oldactive && !man-> isActive ( )) { // no menubar to toggle -> try O-Menu
				QCopEnvelope e ( "QPE/TaskBar", "toggleStartMenu()" );
			}
		}
	}
	else if ( msg == "setDefaultRotation(int)" ) {
		if ( type() == GuiServer ) {
			int r;
			stream >> r;
			setDefaultRotation( r );
		}
	}
	else if ( msg == "setCurrentMode(int,int,int)" ) { // Added: 2003-06-11 by Tim Ansell <mithro@mithis.net>
            if ( type() == GuiServer ) {
		int x, y, depth;
		stream >> x;
		stream >> y;
		stream >> depth;
		setCurrentMode( x, y, depth );
            }
 	}
	else if ( msg == "reset()" ) {
            if ( type() != GuiServer )
                reset();
	}
	else if ( msg == "setCurrentRotation(int)" ) {
		int r;
		stream >> r;
		setCurrentRotation( r );
 	}
	else if ( msg == "shutdown()" ) {
		if ( type() == GuiServer )
			shutdown();
	}
	else if ( msg == "quit()" ) {
		if ( type() != GuiServer )
			tryQuit();
	}
	else if ( msg == "forceQuit()" ) {
		if ( type() != GuiServer )
			quit();
	}
	else if ( msg == "restart()" ) {
		if ( type() == GuiServer )
			restart();
	}
	else if ( msg == "language(QString)" ) {
		if ( type() == GuiServer ) {
			QString l;
			stream >> l;
			QString cl = getenv( "LANG" );
			if ( cl != l ) {
				if ( l.isNull() )
					unsetenv( "LANG" );
				else
					setenv( "LANG", l.latin1(), 1 );
				restart();
			}
		}
	}
	else if ( msg == "timeChange(QString)" ) {
		QString t;
		stream >> t;
		if ( t.isNull() )
			unsetenv( "TZ" );
		else
			setenv( "TZ", t.latin1(), 1 );
		// emit the signal so everyone else knows...
		emit timeChanged();
	}
	else if ( msg == "addAlarm(QDateTime,QCString,QCString,int)" ) {
		if ( type() == GuiServer ) {
			QDateTime when;
			QCString channel, message;
			int data;
			stream >> when >> channel >> message >> data;
			AlarmServer::addAlarm( when, channel, message, data );
		}
	}
	else if ( msg == "deleteAlarm(QDateTime,QCString,QCString,int)" ) {
		if ( type() == GuiServer ) {
			QDateTime when;
			QCString channel, message;
			int data;
			stream >> when >> channel >> message >> data;
			AlarmServer::deleteAlarm( when, channel, message, data );
		}
	}
	else if ( msg == "clockChange(bool)" ) {
		int tmp;
		stream >> tmp;
		emit clockChanged( tmp );
	}
	else if ( msg == "weekChange(bool)" ) {
		int tmp;
		stream >> tmp;
		emit weekChanged( tmp );
	}
	else if ( msg == "setDateFormat(DateFormat)" ) {
		DateFormat tmp;
		stream >> tmp;
		emit dateFormatChanged( tmp );
	}
	else if ( msg == "setVolume(int,int)" ) {
		int t, v;
		stream >> t >> v;
		setVolume( t, v );
		emit volumeChanged( muted );
	}
	else if ( msg == "volumeChange(bool)" ) {
		stream >> muted;
		setVolume();
		emit volumeChanged( muted );
	}
	else if ( msg == "setMic(int,int)" ) { // Added: 2002-02-08 by Jeremy Cowgar <jc@cowgar.com>
		int t, v;
		stream >> t >> v;
		setMic( t, v );
		emit micChanged( micMuted );
	}
	else if ( msg == "micChange(bool)" ) { // Added: 2002-02-08 by Jeremy Cowgar <jc@cowgar.com>
		stream >> micMuted;
		setMic();
		emit micChanged( micMuted );
	}
	else if ( msg == "setBass(int,int)" ) { // Added: 2002-12-13 by Maximilian Reiss <harlekin@handhelds.org>
		int t, v;
		stream >> t >> v;
		setBass( t, v );
	}
	else if ( msg == "bassChange(bool)" ) { // Added: 2002-12-13 by Maximilian Reiss <harlekin@handhelds.org>
	        setBass();
	}
    	else if ( msg == "setTreble(int,int)" ) { // Added: 2002-12-13 by Maximilian Reiss <harlekin@handhelds.org>
		int t, v;
		stream >> t >> v;
		setTreble( t, v );
	}
	else if ( msg == "trebleChange(bool)" ) { // Added: 2002-12-13 by Maximilian Reiss <harlekin@handhelds.org>
	        setTreble();
	} else if ( msg == "getMarkedText()" ) {
	if ( type() == GuiServer ) {
	    const ushort unicode = 'C'-'@';
	    const int scan = Key_C;
	    qwsServer->processKeyEvent( unicode, scan, ControlButton, TRUE, FALSE );
	    qwsServer->processKeyEvent( unicode, scan, ControlButton, FALSE, FALSE );
	}
    } else if ( msg == "newChannel(QString)") {
	QString myChannel = "QPE/Application/" + d->appName;
	QString channel;
	stream >> channel;
	if (channel == myChannel) {
	    processQCopFile();
	    d->sendQCopQ();
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

    // 1. Raise the main widget
    QWidget *top = d->qpe_main_widget;
    if ( !top ) top = mainWidget();

    if ( top && d->keep_running ) {
	if ( top->isVisible() )
	    r = TRUE;
	else if (d->preloaded) {
	    // We are preloaded and not visible.. pretend we just started..
#ifndef QT_NO_COP
	    QCopEnvelope e("QPE/System", "fastAppShowing(QString)");
	    e << d->appName;
#endif
	}

	d->show_mx(top,d->nomaximize, d->appName);
	top->raise();
    }

    QWidget *topm = activeModalWidget();

    // 2. Raise any parentless widgets (except top and topm, as they
    //     are raised before and after this loop).  Order from most
    //     recently raised as deepest to least recently as top, so
    //     that repeated calls cycle through widgets.
    QWidgetList *list = topLevelWidgets();
    if ( list ) {
	bool foundlast = FALSE;
	QWidget* topsub = 0;
	if ( d->lastraised ) {
	    for (QWidget* w = list->first(); w; w = list->next()) {
		if ( !w->parentWidget() && w != topm && w->isVisible() && !w->isDesktop() ) {
		    if ( w == d->lastraised )
			foundlast = TRUE;
		    if ( foundlast ) {
			w->raise();
			topsub = w;
		    }
		}
	    }
	}
	for (QWidget* w = list->first(); w; w = list->next()) {
	    if ( !w->parentWidget() && w != topm && w->isVisible() && !w->isDesktop() ) {
		if ( w == d->lastraised )
		    break;
		w->raise();
		topsub = w;
	    }
	}
	d->lastraised = topsub;
	delete list;
    }

    // 3. Raise the active modal widget.
    if ( topm ) {
	topm->show();
	topm->raise();
	// If we haven't already handled the fastAppShowing message
	if (!top && d->preloaded) {
#ifndef QT_NO_COP
	    QCopEnvelope e("QPE/System", "fastAppShowing(QString)");
	    e << d->appName;
#endif
	}
	r = FALSE;
    }

    return r;
}


void QPEApplication::pidMessage( const QCString& msg, const QByteArray& data)
{
#ifdef Q_WS_QWS

	if ( msg == "quit()" ) {
		tryQuit();
	}
	else if ( msg == "quitIfInvisible()" ) {
		if ( d->qpe_main_widget && !d->qpe_main_widget->isVisible() )
			quit();
	}
	else if ( msg == "close()" ) {
		hideOrQuit();
	}
	else if ( msg == "disablePreload()" ) {
		d->preloaded = FALSE;
		d->keep_running = TRUE;
		/* so that quit will quit */
	}
	else if ( msg == "enablePreload()" ) {
		if (d->qpe_main_widget)
			d->preloaded = TRUE;
		d->keep_running = TRUE;
		/* so next quit won't quit */
	}
	else if ( msg == "raise()" ) {
		d->keep_running = TRUE;
		d->notbusysent = FALSE;
		raiseAppropriateWindow();
		// Tell the system we're still chugging along...
		QCopEnvelope e("QPE/System", "appRaised(QString)");
		e << d->appName;
	}
	else if ( msg == "flush()" ) {
		emit flush();
		// we need to tell the desktop
		QCopEnvelope e( "QPE/Desktop", "flushDone(QString)" );
		e << d->appName;
	}
	else if ( msg == "reload()" ) {
		emit reload();
	}
	else if ( msg == "setDocument(QString)" ) {
		d->keep_running = TRUE;
		QDataStream stream( data, IO_ReadOnly );
		QString doc;
		stream >> doc;
		QWidget *mw = mainWidget();
		if ( !mw )
			mw = d->qpe_main_widget;
		if ( mw )
			Global::setDocument( mw, doc );

	} else if ( msg == "QPEProcessQCop()" ) {
            processQCopFile();
            d->sendQCopQ();
        }else
        {
            bool p = d->keep_running;
            d->keep_running = FALSE;
            emit appMessage( msg, data);
            if ( d->keep_running ) {
                d->notbusysent = FALSE;
                raiseAppropriateWindow();
                if ( !p ) {
                    // Tell the system we're still chugging along...
#ifndef QT_NO_COP
                    QCopEnvelope e("QPE/System", "appRaised(QString)");
                    e << d->appName;
#endif
                }
            }
            if ( p )
                d->keep_running = p;
	}
#endif
}


/*!
  Sets widget \a mw as the mainWidget() and shows it. For small windows,
  consider passing TRUE for \a nomaximize rather than the default FALSE.

  \sa showMainDocumentWidget()
*/
void QPEApplication::showMainWidget( QWidget* mw, bool nomaximize )
{
//	    setMainWidget(mw); this breaks FastLoading because lastWindowClose() would quit
            d->show(mw, nomaximize );
}

/*!
  Sets widget \a mw as the mainWidget() and shows it. For small windows,
  consider passing TRUE for \a nomaximize rather than the default FALSE.

  This calls designates the application as
  a \link docwidget.html document-oriented\endlink application.

  The \a mw widget \e must have this slot: setDocument(const QString&).

  \sa showMainWidget()
*/
void QPEApplication::showMainDocumentWidget( QWidget* mw, bool nomaximize )
{
	if ( mw && argc() == 2 )
		Global::setDocument( mw, QString::fromUtf8(argv()[1]) );


//	setMainWidget(mw); see above
	d->show(mw, nomaximize );
}


/*!
  If an application is started via a \link qcop.html QCop\endlink
  message, the application will process the \link qcop.html
  QCop\endlink message and then quit. If the application calls this
  function while processing a \link qcop.html QCop\endlink message,
  after processing its outstanding \link qcop.html QCop\endlink
  messages the application will start 'properly' and show itself.

  \sa keepRunning()
*/
void QPEApplication::setKeepRunning()
{
	if ( qApp && qApp->inherits( "QPEApplication" ) ) {
		QPEApplication * qpeApp = ( QPEApplication* ) qApp;
		qpeApp->d->keep_running = TRUE;
	}
}

/*!
  Returns TRUE if the application will quit after processing the
  current list of qcop messages; otherwise returns FALSE.

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
	}
	else {
		QStyle *s = QStyleFactory::create( style );
		if ( s )
			setStyle( s );
	}
#else
	if ( style == "Windows" ) {
		setStyle( new QWindowsStyle );
	}
	else if ( style == "QPE" ) {
		setStyle( new QPEStyle );
	}
	else if ( style == "Light" ) {
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

	else {
		QStyle *sty = 0;
		QString path = QPEApplication::qpeDir ( ) + "/plugins/styles/";

#ifdef Q_OS_MACX
		if ( style. find ( ".dylib" ) > 0 )
			path += style;
		else
			path = path + "lib" + style. lower ( ) + ".dylib"; // compatibility
#else
		if ( style. find ( ".so" ) > 0 )
			path += style;
		else
			path = path + "lib" + style. lower ( ) + ".so"; // compatibility
#endif
		static QLibrary *lastlib = 0;
		static StyleInterface *lastiface = 0;

		QLibrary *lib = new QLibrary ( path );
		StyleInterface *iface = 0;

		if (( lib-> queryInterface ( IID_Style, ( QUnknownInterface ** ) &iface ) == QS_OK ) && iface )
			sty = iface-> style ( );

		if ( sty ) {
			setStyle ( sty );

			if ( lastiface )
				lastiface-> release ( );
			lastiface = iface;

			if ( lastlib ) {
				lastlib-> unload ( );
				delete lastlib;
			}
			lastlib = lib;
		}
		else {
			if ( iface )
				iface-> release ( );
			delete lib;

			setStyle ( new LightStyle ( ));
		}
	}
#endif
}

/*!
  \internal
*/
void QPEApplication::prepareForTermination( bool willrestart )
{
	if ( willrestart ) {
		// Draw a big wait icon, the image can be altered in later revisions
		//  QWidget *d = QApplication::desktop();
		QImage img = Resource::loadImage( "launcher/new_wait" );
		QPixmap pix;
		pix.convertFromImage( img.smoothScale( 1 * img.width(), 1 * img.height() ) );
		QLabel *lblWait = new QLabel( 0, "wait hack!", QWidget::WStyle_Customize |
		                              QWidget::WStyle_NoBorder | QWidget::WStyle_Tool );
		lblWait->setPixmap( pix );
		lblWait->setAlignment( QWidget::AlignCenter );
		lblWait->show();
		lblWait->showMaximized();
	}
#ifndef SINGLE_APP
	{ QCopEnvelope envelope( "QPE/System", "forceQuit()" );
	}
	processEvents(); // ensure the message goes out.
	sleep( 1 ); // You have 1 second to comply.
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

static QPtrDict<void>* stylusDict = 0;
static void createDict()
{
	if ( !stylusDict )
		stylusDict = new QPtrDict<void>;
}

/*!
  Returns the current StylusMode for widget \a w.

  \sa setStylusOperation() StylusMode
*/
QPEApplication::StylusMode QPEApplication::stylusOperation( QWidget* w )
{
	if ( stylusDict )
		return ( StylusMode ) ( int ) stylusDict->find( w );
	return LeftOnly;
}

/*!
    \enum QPEApplication::StylusMode

    \value LeftOnly the stylus only generates LeftButton
                       events (the default).
    \value RightOnHold the stylus generates RightButton events
                       if the user uses the press-and-hold gesture.

    \sa setStylusOperation() stylusOperation()
*/

/*!
  Causes widget \a w to receive mouse events according to the stylus
  \a mode.

  \sa stylusOperation() StylusMode
*/
void QPEApplication::setStylusOperation( QWidget * w, StylusMode mode )
{
	createDict();
	if ( mode == LeftOnly ) {
		stylusDict->remove
		( w );
		w->removeEventFilter( qApp );
	}
	else {
		stylusDict->insert( w, ( void* ) mode );
		connect( w, SIGNAL( destroyed() ), qApp, SLOT( removeSenderFromStylusDict() ) );
		w->installEventFilter( qApp );
	}
}


/*!
  \reimp
*/
bool QPEApplication::eventFilter( QObject *o, QEvent *e )
{
        if ( !o->isWidgetType() )
            return FALSE;

	if ( stylusDict && e->type() >= QEvent::MouseButtonPress && e->type() <= QEvent::MouseMove ) {
		QMouseEvent * me = ( QMouseEvent* ) e;
		StylusMode mode = (StylusMode)(int)stylusDict->find(o);
		switch (mode) {
			case RightOnHold:
				switch ( me->type() ) {
					case QEvent::MouseButtonPress:
						if ( me->button() == LeftButton ) {
                                                    if (!d->presstimer )
                                                        d->presstimer = startTimer(500); // #### pref.
							d->presswidget = (QWidget*)o;
							d->presspos = me->pos();
							d->rightpressed = FALSE;
						}
						break;
					case QEvent::MouseMove:
						if (d->presstimer && (me->pos() - d->presspos).manhattanLength() > 8) {
							killTimer(d->presstimer);
							d->presstimer = 0;
						}
						break;
					case QEvent::MouseButtonRelease:
						if ( me->button() == LeftButton ) {
							if ( d->presstimer ) {
								killTimer(d->presstimer);
								d->presstimer = 0;
							}
							if ( d->rightpressed && d->presswidget ) {
								// Right released
								postEvent( d->presswidget,
								           new QMouseEvent( QEvent::MouseButtonRelease, me->pos(),
								                            RightButton, LeftButton + RightButton ) );
								// Left released, off-widget
								postEvent( d->presswidget,
								           new QMouseEvent( QEvent::MouseMove, QPoint( -1, -1),
								                            LeftButton, LeftButton ) );
								postEvent( d->presswidget,
								           new QMouseEvent( QEvent::MouseButtonRelease, QPoint( -1, -1),
								                            LeftButton, LeftButton ) );
								d->rightpressed = FALSE;
								return TRUE; // don't send the real Left release
							}
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
	else if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease ) {
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
	stylusDict->remove
	( ( void* ) sender() );
	if ( d->presswidget == sender() )
		d->presswidget = 0;
}

/*!
  \internal
*/
bool QPEApplication::keyboardGrabbed() const
{
	return d->kbgrabbed;
}


/*!
  Reverses the effect of grabKeyboard(). This is called automatically
  on program exit.
*/
void QPEApplication::ungrabKeyboard()
{
	((QPEApplication *) qApp )-> d-> kbgrabbed = false;
}

/*!
  Grabs the physical keyboard keys, e.g. the application's launching
  keys. Instead of launching applications when these keys are pressed
  the signals emitted are sent to this application instead. Some games
  programs take over the launch keys in this way to make interaction
  easier.

  \sa ungrabKeyboard()
*/
void QPEApplication::grabKeyboard()
{
	((QPEApplication *) qApp )-> d-> kbgrabbed = true;
}

/*!
  \reimp
*/
int QPEApplication::exec()
{
        d->qcopQok = true;
#ifndef QT_NO_COP
	d->sendQCopQ();
        if ( !d->keep_running )
            processEvents(); // we may have received QCop messages in the meantime.
#endif

	if ( d->keep_running )
		//|| d->qpe_main_widget && d->qpe_main_widget->isVisible() )
		return QApplication::exec();

#ifndef QT_NO_COP

	{
		QCopEnvelope e( "QPE/System", "closing(QString)" );
		e << d->appName;
	}
#endif
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
	if ( activeModalWidget() || strcmp( argv() [ 0 ], "embeddedkonsole" ) == 0 )
		return ; // Inside modal loop or konsole. Too hard to save state.
#ifndef QT_NO_COP

	{
		QCopEnvelope e( "QPE/System", "closing(QString)" );
		e << d->appName;
	}
#endif
	processEvents();

	quit();
}

/*!
  \internal
*/
void QPEApplication::installTranslation( const QString& baseName ) {
    QTranslator* trans = new QTranslator(this);
    QString tfn = qpeDir() + "/i18n/"+baseName;
    if ( trans->load( tfn ) )
        installTranslator( trans );
    else
        delete trans;
}

/*!
  \internal
  User initiated quit.  Makes the window 'Go Away'.  If preloaded this means
  hiding the window.  If not it means quitting the application.
  As this is user initiated we don't need to check state.
*/
void QPEApplication::hideOrQuit()
{
	processEvents();

	// If we are a preloaded application we don't actually quit, so emit
	// a System message indicating we're quasi-closing.
	if ( d->preloaded && d->qpe_main_widget )
#ifndef QT_NO_COP

	{
		QCopEnvelope e("QPE/System", "fastAppHiding(QString)" );
		e << d->appName;
		d->qpe_main_widget->hide();
	}
#endif
	else
		quit();
}

#if (__GNUC__ > 2 )
extern "C" void __cxa_pure_virtual();

void __cxa_pure_virtual()
{
    fprintf( stderr, "Pure virtual called\n");
    abort();

}

#endif


#if defined(OPIE_NEW_MALLOC)

// The libraries with the skiff package (and possibly others) have
// completely useless implementations of builtin new and delete that
// use about 50% of your CPU. Here we revert to the simple libc
// functions.

void* operator new[]( size_t size )
{
	return malloc( size );
}

void* operator new( size_t size )
{
	return malloc( size );
}

void operator delete[]( void* p )
{
	free( p );
}

void operator delete[]( void* p, size_t /*size*/ )
{
	free( p );
}


void operator delete( void* p )
{
	free( p );
}

void operator delete( void* p, size_t /*size*/ )
{
	free( p );
}

#endif

#if ( QT_VERSION <= 230 ) && !defined(SINGLE_APP)
#include <qwidgetlist.h>
#ifdef QWS
#include <qgfx_qws.h>
extern QRect qt_maxWindowRect;
void qt_setMaxWindowRect(const QRect& r )
{
	qt_maxWindowRect = qt_screen->mapFromDevice( r,
	                   qt_screen->mapToDevice( QSize( qt_screen->width(), qt_screen->height() ) ) );
	// Re-resize any maximized windows
	QWidgetList* l = QApplication::topLevelWidgets();
	if ( l ) {
		QWidget * w = l->first();
		while ( w ) {
			if ( w->isVisible() && w->isMaximized() ) {
				w->showMaximized();
			}
			w = l->next();
		}
		delete l;
	}
}
#endif
#endif
