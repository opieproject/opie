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
**********************************************************************/

#include "server.h"
#include "serverapp.h"
#include "launcher.h"
#include "startmenu.h"
#include "transferserver.h"
#include "qcopbridge.h"
#include "irserver.h"
#include "packageslave.h"
#include "calibrate.h"
#include "qrsync.h"
#include "syncdialog.h"
#include "launcher.h"
#include "shutdownimpl.h"
#include "applauncher.h"
#if 0
#include "suspendmonitor.h"
#endif
#include "documentlist.h"

#include <qtopia/applnk.h>
#include <qtopia/private/categories.h>
#include <qtopia/mimetype.h>
#include <qtopia/config.h>
#include <qtopia/resource.h>
#include <qtopia/version.h>
#include <qtopia/storage.h>

#include <qtopia/qcopenvelope_qws.h>
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>
#include <qtopia/global.h>
//#include <qtopia/custom.h>

#include <opie2/odevicebutton.h>
#include <opie2/odevice.h>

#include <unistd.h>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qtextstream.h>

#include <stdlib.h>

extern QRect qt_maxWindowRect;

using namespace Opie;

static QWidget *calibrate(bool)
{
#ifdef Q_WS_QWS
    Calibrate *c = new Calibrate;
    c->show();
    return c;
#else
    return 0;
#endif
}

#define FACTORY(T) \
    static QWidget *new##T( bool maximized ) { \
	QWidget *w = new T( 0, 0, QWidget::WDestructiveClose | QWidget::WGroupLeader ); \
	if ( maximized ) { \
	    if ( qApp->desktop()->width() <= 350 ) { \
		w->showMaximized(); \
	    } else { \
		w->resize( QSize( 300, 300 ) ); \
	    } \
	} \
	w->show(); \
	return w; \
    }


#ifdef SINGLE_APP
#define APP(a,b,c,d) FACTORY(b)
#include "apps.h"
#undef APP
#endif // SINGLE_APP

static Global::Command builtins[] = {

#ifdef SINGLE_APP
#define APP(a,b,c,d) { a, new##b, c, d },
#include "apps.h"
#undef APP
#endif

    /* FIXME defines need to be defined*/
#if !defined(OPIE_NO_BUILTIN_CALIBRATE)
        { "calibrate",          calibrate,	        1, 0 }, // No tr
#endif
#if !defined(OPIE_NO_BUILTIN_SHUTDOWN)
	{ "shutdown",           Global::shutdown,      	1, 0 }, // No tr
//	{ "run",                run,			1, 0 }, // No tr
#endif

	{ 0,            calibrate,	0, 0 },
};


//---------------------------------------------------------------------------


//===========================================================================

Server::Server() :
    QWidget( 0, 0, WStyle_Tool | WStyle_Customize ),
    qcopBridge( 0 ),
    transferServer( 0 ),
    packageHandler( 0 ),
    syncDialog( 0 )
{
    Global::setBuiltinCommands(builtins);

    tid_xfer = 0;
    /* ### FIXME ### */
/*    tid_today = startTimer(3600*2*1000);*/
    last_today_show = QDate::currentDate();

#if 0
    tsmMonitor = new TempScreenSaverMode();
    connect( tsmMonitor, SIGNAL(forceSuspend()), qApp, SIGNAL(power()) );
#endif

    serverGui = new Launcher;
    serverGui->createGUI();

    docList = new DocumentList( serverGui );
    appLauncher = new AppLauncher(this);
    connect(appLauncher, SIGNAL(launched(int, const QString &)), this, SLOT(applicationLaunched(int, const QString &)) );
    connect(appLauncher, SIGNAL(terminated(int, const QString &)), this, SLOT(applicationTerminated(int, const QString &)) );
    connect(appLauncher, SIGNAL(connected(const QString &)), this, SLOT(applicationConnected(const QString &)) );

    storage = new StorageInfo( this );
    connect( storage, SIGNAL(disksChanged()), this, SLOT(storageChanged()) );

    // start services
    startTransferServer();
    (void) new IrServer( this );

    packageHandler = new PackageHandler( this );
    connect(qApp, SIGNAL(activate(const Opie::ODeviceButton*,bool)),
            this,SLOT(activate(const Opie::ODeviceButton*,bool)));

    setGeometry( -10, -10, 9, 9 );

    QCopChannel *channel = new QCopChannel("QPE/System", this);
    connect(channel, SIGNAL(received(const QCString &, const QByteArray &)),
	    this, SLOT(systemMsg(const QCString &, const QByteArray &)) );

    QCopChannel *tbChannel = new QCopChannel( "QPE/TaskBar", this );
    connect( tbChannel, SIGNAL(received(const QCString&, const QByteArray&)),
	    this, SLOT(receiveTaskBar(const QCString&, const QByteArray&)) );

    connect( qApp, SIGNAL(prepareForRestart()), this, SLOT(terminateServers()) );
    connect( qApp, SIGNAL(timeChanged()), this, SLOT(pokeTimeMonitors()) );

    preloadApps();
}

void Server::show()
{
    ServerApplication::login(TRUE);
    QWidget::show();
}

Server::~Server()
{
    serverGui->destroyGUI();
    delete docList;
    delete qcopBridge;
    delete transferServer;
    delete serverGui;
#if 0
    delete tsmMonitor;
#endif
}

static bool hasVisibleWindow(const QString& clientname, bool partial)
{
#ifdef QWS
    const QList<QWSWindow> &list = qwsServer->clientWindows();
    QWSWindow* w;
    for (QListIterator<QWSWindow> it(list); (w=it.current()); ++it) {
	if ( w->client()->identity() == clientname ) {
	    if ( partial && !w->isFullyObscured() )
		return TRUE;
	    if ( !partial && !w->isFullyObscured() && !w->isPartiallyObscured() ) {
# if QT_VERSION < 0x030000
		QRect mwr = qt_screen->mapToDevice(qt_maxWindowRect,
			QSize(qt_screen->width(),qt_screen->height()) );
# else
		QRect mwr = qt_maxWindowRect;
# endif
		if ( mwr.contains(w->requested().boundingRect()) )
		    return TRUE;
	    }
	}
    }
#endif
    return FALSE;
}

void Server::activate(const Opie::ODeviceButton* button, bool held)
{
    Global::terminateBuiltin("calibrate"); // No tr
    Opie::OQCopMessage om;
    if ( held ) {
	om = button->heldAction();
    } else {
	om = button->pressedAction();
    }

    if ( om.channel() != "ignore" )
        om.send();

    // A button with no action defined, will return a null ServiceRequest.  Don't attempt
    // to send/do anything with this as it will crash
    /* ### FIXME */
#if 0
    if ( !sr.isNull() ) {
	QString app = sr.app();
	bool vis = hasVisibleWindow(app, app != "qpe");
	if ( sr.message() == "raise()" && vis ) {
	    sr.setMessage("nextView()");
	} else {
	    // "back door"
	    sr << (int)vis;
	}

	sr.send();
    }
#endif
}


#ifdef Q_WS_QWS


typedef struct KeyOverride {
    ushort scan_code;
    QWSServer::KeyMap map;
};


static const KeyOverride jp109keys[] = {
   { 0x03, {   Qt::Key_2,      '2'     , 0x22     , 0xffff  } },
   { 0x07, {   Qt::Key_6,      '6'     , '&'     , 0xffff  } },
   { 0x08, {   Qt::Key_7,      '7'     , '\''     , 0xffff  } },
   { 0x09, {   Qt::Key_8,      '8'     , '('     , 0xffff  } },
   { 0x0a, {   Qt::Key_9,      '9'     , ')'     , 0xffff  } },
   { 0x0b, {   Qt::Key_0,      '0'     , 0xffff  , 0xffff  } },
   { 0x0c, {   Qt::Key_Minus,      '-'     , '='     , 0xffff  } },
   { 0x0d, {   Qt::Key_AsciiCircum,'^'     , '~'     , '^' - 64  } },
   { 0x1a, {   Qt::Key_At,     '@'     , '`'     , 0xffff  } },
   { 0x1b, {   Qt::Key_BraceLeft, '['     , '{'  , '[' - 64  } },
   { 0x27, {   Qt::Key_Semicolon,  ';'     , '+'     , 0xffff  } },
   { 0x28, {   Qt::Key_Colon,  ':'    ,  '*'     , 0xffff  } },
   { 0x29, {   Qt::Key_Zenkaku_Hankaku,  0xffff  , 0xffff     , 0xffff  } },
   { 0x2b, {   Qt::Key_BraceRight,  ']'    , '}'     , ']'-64  } },
   { 0x70, {   Qt::Key_Hiragana_Katakana,    0xffff  , 0xffff  , 0xffff  } },
   { 0x73, {   Qt::Key_Backslash,  '\\'    , '_'  ,    0xffff  } },
   { 0x79, {   Qt::Key_Henkan,     0xffff  , 0xffff  , 0xffff  } },
   { 0x7b, {   Qt::Key_Muhenkan,   0xffff  , 0xffff  , 0xffff  } },
   { 0x7d, {   Qt::Key_yen,        0x00a5  , '|'     , 0xffff  } },
   { 0x00, {   0,          0xffff  , 0xffff  , 0xffff  } }
};

bool Server::setKeyboardLayout( const QString &kb )
{
    //quick demo version that can be extended

    QIntDict<QWSServer::KeyMap> *om = 0;
    if ( kb == "us101" ) { // No tr
	om = 0;
    } else if ( kb == "jp109" ) {
	om = new QIntDict<QWSServer::KeyMap>(37);
	const KeyOverride *k = jp109keys;
	while ( k->scan_code ) {
	    om->insert( k->scan_code, &k->map );
	    k++;
	}
    }
    QWSServer::setOverrideKeys( om );

    return TRUE;
}

#endif

void Server::systemMsg(const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );

    if ( msg == "securityChanged()" ) {
	if ( transferServer )
	   transferServer->authorizeConnections();
	if ( qcopBridge )
	    qcopBridge->authorizeConnections();
    }
    /* ### FIXME support TempScreenSaverMode */
#if 0
    else if ( msg == "setTempScreenSaverMode(int,int)" ) {
	int mode, pid;
	stream >> mode >> pid;
	tsmMonitor->setTempMode(mode, pid);
    }
#endif
    else if ( msg == "linkChanged(QString)" ) {
	QString link;
	stream >> link;
	qDebug( "desktop.cpp systemMsg -> linkchanged( %s )", link.latin1() );
	docList->linkChanged(link);
    } else if ( msg == "serviceChanged(QString)" ) {
	MimeType::updateApplications();
    } else if ( msg == "mkdir(QString)" ) {
	QString dir;
	stream >> dir;
	if ( !dir.isEmpty() )
	    mkdir( dir );
    } else if ( msg == "rdiffGenSig(QString,QString)" ) {
	QString baseFile, sigFile;
	stream >> baseFile >> sigFile;
	QRsync::generateSignature( baseFile, sigFile );
    } else if ( msg == "rdiffGenDiff(QString,QString,QString)" ) {
	QString baseFile, sigFile, deltaFile;
	stream >> baseFile >> sigFile >> deltaFile;
	QRsync::generateDiff( baseFile, sigFile, deltaFile );
    } else if ( msg == "rdiffApplyPatch(QString,QString)" ) {
	QString baseFile, deltaFile;
	stream >> baseFile >> deltaFile;
	if ( !QFile::exists( baseFile ) ) {
	    QFile f( baseFile );
	    f.open( IO_WriteOnly );
	    f.close();
	}
	QRsync::applyDiff( baseFile, deltaFile );
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "patchApplied(QString)" );
	e << baseFile;
#endif
    } else if ( msg == "rdiffCleanup()" ) {
	mkdir( "/tmp/rdiff" );
	QDir dir;
	dir.setPath( "/tmp/rdiff" );
	QStringList entries = dir.entryList();
	for ( QStringList::Iterator it = entries.begin(); it != entries.end(); ++it )
	    dir.remove( *it );
    } else if ( msg == "sendHandshakeInfo()" ) {
	QString home = getenv( "HOME" );
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "handshakeInfo(QString,bool)" );
	e << home;
	int locked = (int) ServerApplication::screenLocked();
	e << locked;
#endif

    }
    /*
     * QtopiaDesktop relies on the major number
     * to start with 1. We're at 0.9
     * so wee need to fake at least 1.4 to be able
     * to sync with QtopiaDesktop1.6
     */
    else if ( msg == "sendVersionInfo()" ) {
	QCopEnvelope e( "QPE/Desktop", "versionInfo(QString,QString)" );
        /* ### FIXME Architecture ### */
	e << QString::fromLatin1("1.7") << "Uncustomized Device";
    } else if ( msg == "sendCardInfo()" ) {
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "cardInfo(QString)" );
#endif
	storage->update();
	const QList<FileSystem> &fs = storage->fileSystems();
	QListIterator<FileSystem> it ( fs );
	QString s;
	QString homeDir = getenv("HOME");
	QString homeFs, homeFsPath;
	for ( ; it.current(); ++it ) {
	    int k4 = (*it)->blockSize()/256;
	    if ( (*it)->isRemovable() ) {
		s += (*it)->name() + "=" + (*it)->path() + "/Documents " // No tr
		     + QString::number( (*it)->availBlocks() * k4/4 )
		     + "K " + (*it)->options() + ";";
	    } else if ( homeDir.contains( (*it)->path() ) &&
		      (*it)->path().length() > homeFsPath.length() ) {
		homeFsPath = (*it)->path();
		homeFs =
		    (*it)->name() + "=" + homeDir + "/Documents " // No tr
		    + QString::number( (*it)->availBlocks() * k4/4 )
		    + "K " + (*it)->options() + ";";
	    }
	}
	if ( !homeFs.isEmpty() )
	    s += homeFs;

#ifndef QT_NO_COP
	e << s;
#endif
    } else if ( msg == "sendSyncDate(QString)" ) {
	QString app;
	stream >> app;
	Config cfg( "qpe" );
	cfg.setGroup("SyncDate");
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "syncDate(QString,QString)" );
	e  << app  << cfg.readEntry( app );
#endif
	//qDebug("QPE/System sendSyncDate for %s: response %s", app.latin1(),
	//cfg.readEntry( app ).latin1() );
    } else if ( msg == "setSyncDate(QString,QString)" ) {
	QString app, date;
	stream >> app >> date;
	Config cfg( "qpe" );
	cfg.setGroup("SyncDate");
	cfg.writeEntry( app, date );
	//qDebug("setSyncDate(QString,QString) %s %s", app.latin1(), date.latin1());
    } else if ( msg == "startSync(QString)" ) {
	QString what;
	stream >> what;
	delete syncDialog;
	syncDialog = new SyncDialog( this, what );
	syncDialog->show();
	connect( syncDialog, SIGNAL(cancel()), SLOT(cancelSync()) );
    } else if ( msg == "stopSync()") {
	delete syncDialog;
	syncDialog = 0;
    } else if (msg == "restoreDone(QString)") {
	docList->restoreDone();
    } else if ( msg == "getAllDocLinks()" ) {
	docList->sendAllDocLinks();
    }
#ifdef Q_WS_QWS
    else if ( msg == "setMouseProto(QString)" ) {
	QString mice;
	stream >> mice;
	setenv("QWS_MOUSE_PROTO",mice.latin1(),1);
	qwsServer->openMouse();
    } else if ( msg == "setKeyboard(QString)" ) {
	QString kb;
	stream >> kb;
	setenv("QWS_KEYBOARD",kb.latin1(),1);
	qwsServer->openKeyboard();

    } else if ( msg == "setKeyboardAutoRepeat(int,int)" ) {
	int delay, period;
	stream >> delay >> period;
	qwsSetKeyboardAutoRepeat( delay, period );
	Config cfg( "qpe" );
	cfg.setGroup("Keyboard");
	cfg.writeEntry( "RepeatDelay", delay );
	cfg.writeEntry( "RepeatPeriod", period );
    } else if ( msg == "setKeyboardLayout(QString)" ) {
	QString kb;
	stream >> kb;
	setKeyboardLayout( kb );
	Config cfg( "qpe" );
	cfg.setGroup("Keyboard");
	cfg.writeEntry( "Layout", kb );
    } else if ( msg == "autoStart(QString)" ) {
        QString appName;
        stream >> appName;
        Config cfg( "autostart" );
        cfg.setGroup( "AutoStart" );
        if ( appName.compare("clear") == 0){
            cfg.writeEntry("Apps", "");
        }
    } else if ( msg == "autoStart(QString,QString)" ) {
        QString modifier, appName;
        stream >> modifier >> appName;
        Config cfg( "autostart" );
        cfg.setGroup( "AutoStart" );
        if ( modifier.compare("add") == 0 ){
            // only add if appname is entered
           if (!appName.isEmpty()) {
                cfg.writeEntry("Apps", appName);
            }
        } else if (modifier.compare("remove") == 0 ) {
            // need to change for multiple entries
            // actually remove is right now simular to clear, but in future there
            // should be multiple apps in autostart possible.
            QString checkName;
            checkName = cfg.readEntry("Apps", "");
            if (checkName == appName) {
                cfg.writeEntry("Apps", "");
            }
        }
        // case the autostart feature should be delayed
    } else if ( msg == "autoStart(QString,QString,QString)") {
         QString modifier, appName, delay;
         stream >> modifier >> appName >> delay;
        Config cfg( "autostart" );

        cfg.setGroup( "AutoStart" );
        if ( modifier.compare("add") == 0 ){
            // only add it appname is entered
            if (!appName.isEmpty()) {
                cfg.writeEntry("Apps", appName);
                cfg.writeEntry("Delay", delay);
            }
        } else {
        }
    }
#endif
}

void Server::receiveTaskBar(const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );

    if ( msg == "reloadApps()" ) {
	docList->reloadAppLnks();
    } else if ( msg == "soundAlarm()" ) {
	ServerApplication::soundAlarm();
    }
    else if ( msg == "setLed(int,bool)" ) {
	int led, status;
	stream >> led >> status;

        QValueList <OLed> ll = ODevice::inst ( )-> ledList ( );
        if ( ll. count ( ))	{
            OLed l = ll. contains ( Led_Mail ) ? Led_Mail : ll [0];
            bool canblink = ODevice::inst ( )-> ledStateList ( l ). contains ( Led_BlinkSlow );

            ODevice::inst ( )-> setLedState ( l, status ? ( canblink ? Led_BlinkSlow : Led_On ) : Led_Off );
        }
    }
}

void Server::cancelSync()
{
#ifndef QT_NO_COP
    QCopEnvelope e( "QPE/Desktop", "cancelSync()" );
#endif
    delete syncDialog;
    syncDialog = 0;
}

bool Server::mkdir(const QString &localPath)
{
    QDir fullDir(localPath);
    if (fullDir.exists())
	return true;

    // at this point the directory doesn't exist
    // go through the directory tree and start creating the direcotories
    // that don't exist; if we can't create the directories, return false

    QString dirSeps = "/";
    int dirIndex = localPath.find(dirSeps);
    QString checkedPath;

    // didn't find any seps; weird, use the cur dir instead
    if (dirIndex == -1) {
	//qDebug("No seperators found in path %s", localPath.latin1());
	checkedPath = QDir::currentDirPath();
    }

    while (checkedPath != localPath) {
	// no more seperators found, use the local path
	if (dirIndex == -1)
	    checkedPath = localPath;
	else {
	    // the next directory to check
	    checkedPath = localPath.left(dirIndex) + "/";
	    // advance the iterator; the next dir seperator
	    dirIndex = localPath.find(dirSeps, dirIndex+1);
	}

	QDir checkDir(checkedPath);
	if (!checkDir.exists()) {
	    //qDebug("mkdir making dir %s", checkedPath.latin1());

	    if (!checkDir.mkdir(checkedPath)) {
		qDebug("Unable to make directory %s", checkedPath.latin1());
		return FALSE;
	    }
	}

    }
    return TRUE;
}

void Server::styleChange( QStyle &s )
{
    QWidget::styleChange( s );
}

void Server::startTransferServer()
{
    if ( !qcopBridge ) {
	// start qcop bridge server
	qcopBridge = new QCopBridge( 4243 );
	if ( qcopBridge->ok() ) {
	    // ... OK
	    connect( qcopBridge, SIGNAL(connectionClosed(const QHostAddress &)),
		    this, SLOT(syncConnectionClosed(const QHostAddress &)) );
	} else {
	    delete qcopBridge;
	    qcopBridge = 0;
	}
    }
    if ( !transferServer ) {
	// start transfer server
	transferServer = new TransferServer( 4242 );
	if ( transferServer->ok() ) {
	    // ... OK
	} else {
	    delete transferServer;
	    transferServer = 0;
	}
    }
    if ( !transferServer || !qcopBridge )
	tid_xfer = startTimer( 2000 );
}

void Server::timerEvent( QTimerEvent *e )
{
    if ( e->timerId() == tid_xfer ) {
	killTimer( tid_xfer );
	tid_xfer = 0;
	startTransferServer();
    }
    /* ### FIXME today startin */
#if 0
    else if ( e->timerId() == tid_today ) {
	QDate today = QDate::currentDate();
	if ( today != last_today_show ) {
	    last_today_show = today;
	    Config cfg("today");
	    cfg.setGroup("Start");
#ifndef QPE_DEFAULT_TODAY_MODE
#define QPE_DEFAULT_TODAY_MODE "Never"
#endif
	    if ( cfg.readEntry("Mode",QPE_DEFAULT_TODAY_MODE) == "Daily" ) {
		QCopEnvelope env(Service::channel("today"),"raise()");
	    }
	}
    }
#endif
}

void Server::terminateServers()
{
    delete transferServer;
    delete qcopBridge;
    transferServer = 0;
    qcopBridge = 0;
}

void Server::syncConnectionClosed( const QHostAddress & )
{
    qDebug( "Lost sync connection" );
    delete syncDialog;
    syncDialog = 0;
}

void Server::pokeTimeMonitors()
{
#if 0
    // inform all TimeMonitors
    QStrList tms = Service::channels("TimeMonitor");
    for (const char* ch = tms.first(); ch; ch=tms.next()) {
	QString t = getenv("TZ");
	QCopEnvelope e(ch, "timeChange(QString)");
	e << t;
    }
#endif
}

void Server::applicationLaunched(int, const QString &app)
{
    serverGui->applicationStateChanged( app, ServerInterface::Launching );
}

void Server::applicationTerminated(int pid, const QString &app)
{
    serverGui->applicationStateChanged( app, ServerInterface::Terminated );
#if 0
    tsmMonitor->applicationTerminated( pid );
#endif
}

void Server::applicationConnected(const QString &app)
{
    serverGui->applicationStateChanged( app, ServerInterface::Running );
}

void Server::storageChanged()
{
    system( "qtopia-update-symlinks" );
    serverGui->storageChanged( storage->fileSystems() );
    docList->storageChanged();
}



void Server::preloadApps()
{
    Config cfg("Launcher");
    cfg.setGroup("Preload");
    QStringList apps = cfg.readListEntry("Apps",',');
    for (QStringList::ConstIterator it=apps.begin(); it!=apps.end(); ++it) {
#ifndef QT_NO_COP
	QCopEnvelope e("QPE/Application/"+(*it).local8Bit(), "enablePreload()");
#endif
    }
}

