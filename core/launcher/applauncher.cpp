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

#ifndef QTOPIA_INTERNAL_PRELOADACCESS
#define QTOPIA_INTERNAL_PRELOADACCESS
#endif
#ifndef QTOPIA_INTERNAL_FILEOPERATIONS
#define QTOPIA_INTERNAL_FILEOPERATIONS
#endif
#ifndef QTOPIA_PROGRAM_MONITOR
#define QTOPIA_PROGRAM_MONITOR
#endif
#include <opie2/oglobal.h>

#ifndef Q_OS_WIN32
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#else
#include <process.h>
#include <windows.h>
#include <winbase.h>
#endif

#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

#include <qtimer.h>
#include <qwindowsystem_qws.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qfileinfo.h>

#include <qtopia/qcopenvelope_qws.h>
#include <qtopia/applnk.h>
#include <qtopia/qpeapplication.h>
#include <qtopia/config.h>
#include <qtopia/global.h>

#include "applauncher.h"
#include "documentlist.h"

const int AppLauncher::RAISE_TIMEOUT_MS = 5000;

//---------------------------------------------------------------------------

static AppLauncher* appLauncherPtr;

const int appStopEventID = 1290;

class AppStoppedEvent : public QCustomEvent
{
public:
    AppStoppedEvent(int pid, int status)
	: QCustomEvent( appStopEventID ), mPid(pid), mStatus(status) { }

    int pid() { return mPid; }
    int status() { return mStatus; }

private:
    int mPid, mStatus;
};

AppLauncher::AppLauncher(QObject *parent, const char *name)
    : QObject(parent, name), qlPid(0), qlReady(FALSE),
      appKillerBox(0)
{
    connect(qwsServer, SIGNAL(newChannel(const QString&)), this, SLOT(newQcopChannel(const QString&)));
    connect(qwsServer, SIGNAL(removedChannel(const QString&)), this, SLOT(removedQcopChannel(const QString&)));
    QCopChannel* channel = new QCopChannel( "QPE/System", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	     this, SLOT(received(const QCString&, const QByteArray&)) );

    channel = new QCopChannel( "QPE/Server", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	     this, SLOT(received(const QCString&, const QByteArray&)) );

#ifndef Q_OS_WIN32
    signal(SIGCHLD, signalHandler);
#else
    runningAppsProc.setAutoDelete( TRUE );
#endif
    QString tmp = qApp->argv()[0];
    int pos = tmp.findRev('/');
    if ( pos > -1 )
	tmp = tmp.mid(++pos);
    runningApps[::getpid()] = tmp;

    appLauncherPtr = this;

    QTimer::singleShot( 1000, this, SLOT(createQuickLauncher()) );
}

AppLauncher::~AppLauncher()
{
    appLauncherPtr = 0;
#ifndef Q_OS_WIN32
    signal(SIGCHLD, SIG_DFL);
#endif
    if ( qlPid ) {
	int status;
	::kill( qlPid, SIGTERM );
	waitpid( qlPid, &status, 0 );
    }
}

/*  We use the QCopChannel of the app as an indicator of when it has been launched
    so that we can disable the busy indicators */
void AppLauncher::newQcopChannel(const QString& channelName)
{
//  qDebug("channel %s added", channelName.data() );
    QString prefix("QPE/Application/");
    if (channelName.startsWith(prefix)) {
	{
	    QCopEnvelope e("QPE/System", "newChannel(QString)");
	    e << channelName;
	}
	QString appName = channelName.mid(prefix.length());
	if ( appName != "quicklauncher" ) {
	    emit connected( appName );
	    QCopEnvelope e("QPE/System", "notBusy(QString)");
	    e << appName;
	}
    } else if (channelName.startsWith("QPE/QuickLauncher-")) {
	qDebug("Registered %s", channelName.latin1());
	int pid = channelName.mid(18).toInt();
	if (pid == qlPid)
	    qlReady = TRUE;
    }
}

void AppLauncher::removedQcopChannel(const QString& channelName)
{
    if (channelName.startsWith("QPE/Application/")) {
	QCopEnvelope e("QPE/System", "removedChannel(QString)");
	e << channelName;
    }
}

void AppLauncher::received(const QCString& msg, const QByteArray& data)
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "execute(QString)" ) {
	QString t;
	stream >> t;
	if ( !executeBuiltin( t, QString::null ) )
	    execute(t, QString::null);
    } else if ( msg == "execute(QString,QString)" ) {
	QString t,d;
	stream >> t >> d;
	if ( !executeBuiltin( t, d ) )
	    execute( t, d );
    } else if ( msg == "processQCop(QString)" ) { // from QPE/Server
	QString t;
	stream >> t;
	if ( !executeBuiltin( t, QString::null ) )
	    execute( t, QString::null, TRUE);
    } else if ( msg == "raise(QString)" ) {
	QString appName;
	stream >> appName;

	if ( !executeBuiltin( appName, QString::null ) ) {
	    if ( !waitingHeartbeat.contains( appName ) && appKillerName != appName ) {
		//qDebug( "Raising: %s", appName.latin1() );
		QCString channel = "QPE/Application/";
		channel += appName.latin1();

		// Need to lock it to avoid race conditions with QPEApplication::processQCopFile
		QFile f("/tmp/qcop-msg-" + appName);
		if ( f.open(IO_WriteOnly | IO_Append) ) {
#ifndef Q_OS_WIN32
		    flock(f.handle(), LOCK_EX);
#endif
		    QDataStream ds(&f);
		    QByteArray b;
		    QDataStream bstream(b, IO_WriteOnly);
		    ds << channel << QCString("raise()") << b;
		    f.flush();
#ifndef Q_OS_WIN32
		    flock(f.handle(), LOCK_UN);
#endif
		    f.close();
		}
		bool alreadyRunning = isRunning( appName );
		if ( execute(appName, QString::null) ) {
		    int id = startTimer(RAISE_TIMEOUT_MS + alreadyRunning?2000:0);
		    waitingHeartbeat.insert( appName, id );
		}
	    }
	}
    } else if ( msg == "sendRunningApps()" ) {
	QStringList apps;
	QMap<int,QString>::Iterator it;
        for( it = runningApps.begin(); it != runningApps.end(); ++it )
	    apps.append( *it );
	QCopEnvelope e( "QPE/Desktop", "runningApps(QStringList)" );
	e << apps;
    } else if ( msg == "appRaised(QString)" ) {
	QString appName;
	stream >> appName;
	qDebug("Got a heartbeat from %s", appName.latin1());
	QMap<QString,int>::Iterator it = waitingHeartbeat.find(appName);
	if ( it != waitingHeartbeat.end() ) {
	    killTimer( *it );
	    waitingHeartbeat.remove(it);
	}
	// Check to make sure we're not waiting on user input...
	if ( appKillerBox && appName == appKillerName ) {
	    // If we are, we kill the dialog box, and the code waiting on the result
	    // will clean us up (basically the user said "no").
	    delete appKillerBox;
	    appKillerBox = 0;
	    appKillerName = QString::null;
	}
    }
}

void AppLauncher::signalHandler(int)
{
#ifndef Q_OS_WIN32
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
/*    if (pid == 0 || &status == 0 ) {
	qDebug("hmm, could not get return value from signal");
    }
*/
    QApplication::postEvent(appLauncherPtr, new AppStoppedEvent(pid, status) );
#else
    qDebug("Unhandled signal see by AppLauncher::signalHandler(int)");
#endif
}

bool AppLauncher::event(QEvent *e)
{
    if ( e->type() == appStopEventID ) {
	AppStoppedEvent *ae = (AppStoppedEvent *) e;
	sigStopped(ae->pid(), ae->status() );
	return TRUE;
    }

    return QObject::event(e);
}

void AppLauncher::timerEvent( QTimerEvent *e )
{
    int id = e->timerId();
    QMap<QString,int>::Iterator it;
    for ( it = waitingHeartbeat.begin(); it != waitingHeartbeat.end(); ++it ) {
	if ( *it == id ) {
	    if ( appKillerBox ) // we're already dealing with one
		return;

	    appKillerName = it.key();
	    killTimer( id );
	    waitingHeartbeat.remove( it );

	    // qDebug("Checking in on %s", appKillerName.latin1());

	    // We store this incase the application responds while we're
	    // waiting for user input so we know not to delete ourselves.
	    appKillerBox = new QMessageBox(tr("Application Problem"),
		    tr("<p>%1 is not responding.</p>").arg(appKillerName) +
		    tr("<p>Would you like to force the application to exit?</p>"),
		    QMessageBox::Warning, QMessageBox::Yes,
		    QMessageBox::No | QMessageBox::Default,
		    QMessageBox::NoButton);
	    if (appKillerBox->exec() == QMessageBox::Yes) {
		// qDebug("Killing the app!!! Bwuhahahaha!");
		int pid = pidForName(appKillerName);
		if ( pid > 0 )
		    kill( pid );
	    }
	    appKillerName = QString::null;
	    delete appKillerBox;
	    appKillerBox = 0;
	    return;
	}
    }

    QObject::timerEvent( e );
}

#ifndef Q_OS_WIN32
void AppLauncher::sigStopped(int sigPid, int sigStatus)
{
    int exitStatus = 0;

    bool crashed = WIFSIGNALED(sigStatus);
    if ( !crashed ) {
	if ( WIFEXITED(sigStatus) )
	    exitStatus = WEXITSTATUS(sigStatus);
    } else {
	exitStatus  = WTERMSIG(sigStatus);
    }

    QMap<int,QString>::Iterator it = runningApps.find( sigPid );
    if ( it == runningApps.end() ) {
	if ( sigPid == qlPid ) {
	    qDebug( "quicklauncher stopped" );
	    qlPid = 0;
	    qlReady = FALSE;
	    QFile::remove("/tmp/qcop-msg-quicklauncher" );
	    QTimer::singleShot( 2000, this, SLOT(createQuickLauncher()) );
	}
/*
	if ( sigPid == -1 )
	    qDebug("non-qtopia application exited (disregarded)");
	else
	    qDebug("==== no pid matching %d in list, definite bug", sigPid);
*/
	return;
    }
    QString appName = *it;
    runningApps.remove(it);

    QMap<QString,int>::Iterator hbit = waitingHeartbeat.find(appName);
    if ( hbit != waitingHeartbeat.end() ) {
	killTimer( *hbit );
	waitingHeartbeat.remove( hbit );
    }
    if ( appName == appKillerName ) {
	appKillerName = QString::null;
	delete appKillerBox;
	appKillerBox = 0;
    }

    /* we must disable preload for an app that crashes as the system logic relies on preloaded apps
       actually being loaded.  If eg. the crash happened in the constructor, we can't automatically reload
       the app (withouth some timeout value for eg. 3 tries (which I think is a bad solution)
    */
    bool preloadDisabled = FALSE;
    if ( !DocumentList::appLnkSet ) return;
    const AppLnk* app = DocumentList::appLnkSet->findExec( appName );
    if ( !app ) return; // QCop messages processed to slow?
    if ( crashed && app->isPreloaded() ) {
	Config cfg("Launcher");
	cfg.setGroup("Preload");
	QStringList apps = cfg.readListEntry("Apps",',');
	QString exe = app->exec();
	apps.remove(exe);
	cfg.writeEntry("Apps",apps,',');
	preloadDisabled = TRUE;
    }

    // clean up
    if ( exitStatus ) {
	QCopEnvelope e("QPE/System", "notBusy(QString)");
	e << app->exec();
    }
/*
    // debug info
    for (it = runningApps.begin(); it != runningApps.end(); ++it) {
	qDebug("running according to internal list: %s, with pid %d", (*it).data(), it.key() );
    }
*/

#ifdef QTOPIA_PROGRAM_MONITOR
    if ( crashed ) {
	QString sig;
	switch( exitStatus ) {
	    case SIGABRT: sig = "SIGABRT"; break;
	    case SIGALRM: sig = "SIGALRM"; break;
	    case SIGBUS: sig = "SIGBUS"; break;
	    case SIGFPE: sig = "SIGFPE"; break;
	    case SIGHUP: sig = "SIGHUP"; break;
	    case SIGILL: sig = "SIGILL"; break;
	    case SIGKILL: sig = "SIGKILL"; break;
	    case SIGPIPE: sig = "SIGPIPE"; break;
	    case SIGQUIT: sig = "SIGQUIT"; break;
	    case SIGSEGV: sig = "SIGSEGV"; break;
	    case SIGTERM: sig = "SIGTERM"; break;
	    case SIGTRAP: sig = "SIGTRAP"; break;
	    default: sig = QString("Unkown %1").arg(exitStatus);
	}
	if ( preloadDisabled )
	    sig += tr("<qt><p>Fast loading has been disabled for this application.  Tap and hold the application icon to reenable it.</qt>");

	QString str = tr("<qt><b>%1</b> was terminated due to signal code %2</qt>").arg( app->name() ).arg( sig );
	QMessageBox::information(0, tr("Application terminated"), str );
    } else {
	if ( exitStatus == 255 ) {  //could not find app (because global returns -1)
	    QMessageBox::information(0, tr("Application not found"), tr("<qt>Could not locate application <b>%1</b></qt>").arg( app->exec() ) );
	} else  {
	    QFileInfo fi(QString::fromLatin1("/tmp/qcop-msg-") + appName);
	    if ( fi.exists() && fi.size() ) {
		emit terminated(sigPid, appName);
                qWarning("Re executing obmitted for %s", appName.latin1() );
//		execute( appName, QString::null );
		return;
	    }
	}
    }

#endif

    emit terminated(sigPid, appName);
}
#else
void AppLauncher::sigStopped(int sigPid, int sigStatus)
{
    qDebug("Unhandled signal : AppLauncher::sigStopped(int sigPid, int sigStatus)");
}
#endif // Q_OS_WIN32

bool AppLauncher::isRunning(const QString &app)
{
    for (QMap<int,QString>::ConstIterator it = runningApps.begin(); it != runningApps.end(); ++it) {
	if ( *it == app ) {
#ifdef Q_OS_UNIX
	    pid_t t = ::__getpgid( it.key() );
	    if ( t == -1 ) {
		qDebug("appLauncher bug, %s believed running, but pid %d is not existing", app.data(), it.key() );
		runningApps.remove( it.key() );
		return FALSE;
	    }
#endif
	    return TRUE;
	}
    }

    return FALSE;
}

bool AppLauncher::executeBuiltin(const QString &c, const QString &document)
{
    Global::Command* builtin = OGlobal::builtinCommands();
    QGuardedPtr<QWidget> *running = OGlobal::builtinRunning();

    // Attempt to execute the app using a builtin class for the app
    if (builtin) {
	for (int i = 0; builtin[i].file; i++) {
	    if ( builtin[i].file == c ) {
		if ( running[i] ) {
		    if ( !document.isNull() && builtin[i].documentary )
			Global::setDocument(running[i], document);
		    running[i]->raise();
		    running[i]->show();
		    running[i]->setActiveWindow();
		} else {
		    running[i] = builtin[i].func( builtin[i].maximized );
		}
#ifndef QT_NO_COP
		QCopEnvelope e("QPE/System", "notBusy(QString)" );
		e << c; // that was quick ;-)
#endif
		return TRUE;
	    }
	}
    }

    // Convert the command line in to a list of arguments
    QStringList list = QStringList::split(QRegExp("  *"),c);
    QString ap=list[0];

    if ( ap == "suspend" ) { // No tr
	QWSServer::processKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );
	return TRUE;
    }

    return FALSE;
}

bool AppLauncher::execute(const QString &c, const QString &docParam, bool noRaise)
{
    qWarning("AppLauncher::execute");
    // Convert the command line in to a list of arguments
    QStringList list = QStringList::split(QRegExp("  *"),c);
    if ( !docParam.isEmpty() )
	list.append( docParam );

    QString appName = list[0];
    if ( isRunning(appName) ) {
	QCString channel = "QPE/Application/";
	channel += appName.latin1();

	// Need to lock it to avoid race conditions with QPEApplication::processQCopFile
        QFile f(QString::fromLatin1("/tmp/qcop-msg-") + appName);
	if ( !noRaise && f.open(IO_WriteOnly | IO_Append) ) {
#ifndef Q_OS_WIN32
	    flock(f.handle(), LOCK_EX);
#endif

	    QDataStream ds(&f);
	    QByteArray b;
	    QDataStream bstream(b, IO_WriteOnly);
	    if ( !f.size() ) {
		ds << channel << QCString("raise()") << b;
		if ( !waitingHeartbeat.contains( appName ) && appKillerName != appName ) {
		    int id = startTimer(RAISE_TIMEOUT_MS);
		    waitingHeartbeat.insert( appName, id );
		}
	    }
	    if ( !docParam.isEmpty() ) {
		bstream << docParam;
		ds << channel << QCString("setDocument(QString)") << b;
	    }

	    f.flush();
#ifndef Q_OS_WIN32
	    flock(f.handle(), LOCK_UN);
#endif
	    f.close();
	}
	if ( QCopChannel::isRegistered(channel) ) // avoid unnecessary warnings
	    QCopChannel::send(channel,"QPEProcessQCop()");

	return TRUE;
    }

#ifdef QT_NO_QWS_MULTIPROCESS
    QMessageBox::warning( 0, tr("Error"), tr("<qt>Could not find the application %1</qt>").arg(c),
	tr("OK"), 0, 0, 0, 1 );
#else

    QStrList slist;
    unsigned j;
    for ( j = 0; j < list.count(); j++ )
	slist.append( list[j].utf8() );

    const char **args = new const char *[slist.count() + 1];
    for ( j = 0; j < slist.count(); j++ )
	args[j] = slist.at(j);
    args[j] = NULL;

#ifndef Q_OS_WIN32
#ifdef Q_OS_MACX
    if ( qlPid && qlReady && QFile::exists( QPEApplication::qpeDir()+"plugins/application/lib"+args[0] + ".dylib" ) ) {
#else
    if ( qlPid && qlReady && QFile::exists( QPEApplication::qpeDir()+"plugins/application/lib"+args[0] + ".so" ) ) {
#endif /* Q_OS_MACX */
	qDebug( "Quick launching: %s", args[0] );
	if ( getuid() == 0 )
	    setpriority( PRIO_PROCESS, qlPid, 0 );
	QCString qlch("QPE/QuickLauncher-");
	qlch += QString::number(qlPid);
	QCopEnvelope env( qlch, "execute(QStrList)" );
	env << slist;
	runningApps[qlPid] = QString(args[0]);
	emit launched(qlPid, QString(args[0]));
	QCopEnvelope e("QPE/System", "busy()");
	qlPid = 0;
	qlReady = FALSE;
	QTimer::singleShot( getuid() == 0 ? 800 : 1500, this, SLOT(createQuickLauncher()) );
    } else {
	int pid = ::vfork();
	if ( !pid ) {
	    for ( int fd = 3; fd < 100; fd++ )
		::close( fd );
	    ::setpgid( ::getpid(), ::getppid() );
	    // Try bindir first, so that foo/bar works too
	    ::execv( QPEApplication::qpeDir()+"bin/"+args[0], (char * const *)args );
	    ::execvp( args[0], (char * const *)args );
	    _exit( -1 );
	}

	runningApps[pid] = QString(args[0]);
	emit launched(pid, QString(args[0]));
	QCopEnvelope e("QPE/System", "busy()");
    }
#else
    QProcess *proc = new QProcess(this);
    if (proc){
	for (int i=0; i < slist.count(); i++)
	    proc->addArgument(args[i]);
	connect(proc, SIGNAL(processExited()), this, SLOT(processExited()));
	if (!proc->start()){
	    qDebug("Unable to start application %s", args[0]);
	}else{
	    PROCESS_INFORMATION *procInfo = (PROCESS_INFORMATION *)proc->processIdentifier();
	    if (procInfo){
		DWORD pid = procInfo->dwProcessId;
		runningApps[pid] = QString(args[0]);
		runningAppsProc.append(proc);
		emit launched(pid, QString(args[0]));
		QCopEnvelope e("QPE/System", "busy()");
	    }else{
		qDebug("Unable to read process inforation #1 for %s", args[0]);
	    }
	}
    }else{
	qDebug("Unable to create process for application %s", args[0]);
	return FALSE;
    }
#endif
#endif //QT_NO_QWS_MULTIPROCESS

    delete [] args;
    return TRUE;
}

void AppLauncher::kill( int pid )
{
#ifndef Q_OS_WIN32
    ::kill( pid, SIGTERM );
#else
    for ( QProcess *proc = runningAppsProc.first(); proc; proc = runningAppsProc.next() ) {
	if ( proc->processIdentifier() == pid ) {
	    proc->kill();
	    break;
	}
    }
#endif
}

int AppLauncher::pidForName( const QString &appName )
{
    int pid = -1;

    QMap<int, QString>::Iterator it;
    for (it = runningApps.begin(); it!= runningApps.end(); ++it) {
	if (*it == appName) {
	    pid = it.key();
	    break;
	}
    }

    return pid;
}

void AppLauncher::createQuickLauncher()
{
    static bool disabled = FALSE;
    if (disabled)
        return;

    qlReady = FALSE;
    qlPid = ::vfork();
    if ( !qlPid ) {
	char **args = new char *[2];
	args[0] = "quicklauncher";
	args[1] = 0;
	for ( int fd = 3; fd < 100; fd++ )
	    ::close( fd );
	::setpgid( ::getpid(), ::getppid() );
	// Try bindir first, so that foo/bar works too
        /*
         * LD_BIND_NOW will change the behaviour of ld.so and dlopen
         * RTLD_LAZY will be made RTLD_NOW which leads to problem
         * with miscompiled libraries... if LD_BIND_NOW is set.. there
         * is no way back.. We will wait for numbers from TT to see
         * if using LD_BIND_NOW is worth it - zecke
         */
//	setenv( "LD_BIND_NOW", "1", 1 );
	::execv( QPEApplication::qpeDir()+"bin/quicklauncher", args );
	::execvp( "quicklauncher", args );
        delete []args;
        disabled = TRUE;
	_exit( -1 );
    } else if ( qlPid == -1 ) {
	qlPid = 0;
    } else {
	if ( getuid() == 0 )
	    setpriority( PRIO_PROCESS, qlPid, 19 );
    }
}

// Used only by Win32
void AppLauncher::processExited()
{
#ifdef Q_OS_WIN32
    qDebug("AppLauncher::processExited()");
    bool found = FALSE;
    QProcess *proc = (QProcess *) sender();
    if (!proc){
	qDebug("Interanl error NULL proc");
	return;
    }

    QString appName = proc->arguments()[0];
    qDebug("Removing application %s", appName.latin1());
    runningAppsProc.remove(proc);

    QMap<QString,int>::Iterator hbit = waitingHeartbeat.find(appName);
    if ( hbit != waitingHeartbeat.end() ) {
	killTimer( *hbit );
	waitingHeartbeat.remove( hbit );
    }
    if ( appName == appKillerName ) {
	appKillerName = QString::null;
	delete appKillerBox;
	appKillerBox = 0;
    }

    // Search for the app to find its PID
    QMap<int, QString>::Iterator it;
    for (it = runningApps.begin(); it!= runningApps.end(); ++it){
	if (it.data() == appName){
	    found = TRUE;
	    break;
	}
    }

    if (found){
	emit terminated(it.key(), it.data());
	runningApps.remove(it.key());
    }else{
	qDebug("Internal error application %s not listed as running", appName.latin1());
    }

#endif
}

