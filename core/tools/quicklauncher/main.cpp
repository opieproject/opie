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

#include <qpainter.h>
#include <qstrlist.h>
#include <qtimer.h>
#include <qguardedptr.h>
#include <qcopchannel_qws.h>
#define QTOPIA_INTERNAL_INITAPP

#ifdef private
#       undef  private
#endif
#define private public
#include <qtopia/qpeapplication.h>
#undef private
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dropins.h"

static QPEApplication *app = 0;
static PluginLoader *loader = 0;
static ApplicationInterface *appIface = 0;
static QGuardedPtr<QWidget> mainWindow;

#ifdef _OS_LINUX_
static char **argv0 = 0;
static int argv_lth;
extern char **environ;
#ifndef SPT_BUFSIZE
#define SPT_BUFSIZE     2048
#endif
#include <stdarg.h>
void setproctitle (const char *fmt,...) {
    int        i;
    char       buf[SPT_BUFSIZE];
    va_list    ap;

    if (!argv0)
	return;

    va_start(ap, fmt);
    (void) vsnprintf(buf, SPT_BUFSIZE, fmt, ap);
    va_end(ap);

    i = strlen (buf);
    if (i > argv_lth - 2) {
	i = argv_lth - 2;
	buf[i] = '\0';
    }
    memset(argv0[0], '\0', argv_lth);       /* clear the memory area */
    (void) strcpy (argv0[0], buf);

    argv0[1] = NULL;
}
#endif


class QuickLauncher : public QObject
{
    Q_OBJECT
public:
    QuickLauncher() : QObject()
    {
	QCString ch("QPE/QuickLauncher-");
	ch += QString::number(getpid());
	qlChannel = new QCopChannel( ch, this);
	connect( qlChannel, SIGNAL(received(const QCString&, const QByteArray&)),
		 this, SLOT(message(const QCString&, const QByteArray&)) );
    }

    static void exec( int /*argc*/, char **argv )
    {
	QString appName = argv[0];
	int sep = appName.findRev( '/' );
	if ( sep > 0 )
	    appName = appName.mid( sep+1 );

	appIface = 0;
	if ( loader->queryInterface(appName, IID_QtopiaApplication, (QUnknownInterface**)&appIface) == QS_OK ) {
	    mainWindow = appIface->createMainWindow( appName );
	}
	if ( mainWindow ) {
	    if ( mainWindow->metaObject()->slotNames().contains("setDocument(const QString&)") ) {
		app->showMainDocumentWidget( mainWindow );
	    } else {
		app->showMainWidget( mainWindow );
	    }
	} else {
	    qWarning( "Could not create application main window" );
	    exit(-1);
	}
    }

private slots:
    void message(const QCString &msg, const QByteArray & data)
    {
	QStrList argList;

	if ( msg == "execute(QStrList)" ) {
	    delete qlChannel;
	    QDataStream stream( data, IO_ReadOnly );
	    QStrList argList;
	    stream >> argList;
	    qDebug( "QuickLauncher execute: %s", argList.at(0) );
	    doQuickLaunch( argList );
	    delete this;
	} else if ( msg == "execute(QString)" ) {
	    delete qlChannel;
	    QDataStream stream( data, IO_ReadOnly );
	    QString arg;
	    stream >> arg;
	    qDebug( "QuickLauncher execute: %s", arg.latin1() );
	    QStrList argList;
	    argList.append( arg.utf8() );
	    doQuickLaunch( argList );
	    delete this;
	}
    }

private:
    void doQuickLaunch( QStrList &argList )
    {
	static int myargc = argList.count();
	static char **myargv = new char *[myargc + 1];
	for ( int j = 0; j < myargc; j++ ) {
	    myargv[j] = new char [strlen(argList.at(j))+1];
	    strcpy( myargv[j], argList.at(j) );
	}
	myargv[myargc] = NULL;
#ifdef _OS_LINUX_
	// Change name of process
	setproctitle(myargv[0]);
#endif

	connect(app, SIGNAL(lastWindowClosed()), app, SLOT(hideOrQuit()));
	app->exit_loop();
	app->initApp( myargc, myargv );
	exec( myargc, myargv );
    }

private:
    QCopChannel *qlChannel;
};

int main( int argc, char** argv )
{
    app = new QPEApplication( argc, argv );

    loader = new PluginLoader( "application" );

    unsetenv( "LD_BIND_NOW" );

    QCString arg0 = argv[0];
    int sep = arg0.findRev( '/' );
    if ( sep > 0 )
	arg0 = arg0.mid( sep+1 );
    if ( arg0 != "quicklauncher" ) {
	qDebug( "QuickLauncher invoked as: %s", arg0.data() );
	QuickLauncher::exec( argc, argv );
    } else {
#ifdef _OS_LINUX_
	// Setup to change proc title
	int i;
	char **envp = environ;
	/* Move the environment so we can reuse the memory.
	 * (Code borrowed from sendmail.) */
	for (i = 0; envp[i] != NULL; i++)
	    continue;
	environ = (char **) malloc(sizeof(char *) * (i + 1));
	if (environ == NULL)
	    return -1;
	for (i = 0; envp[i] != NULL; i++)
	    if ((environ[i] = strdup(envp[i])) == NULL)
		return -1;
	environ[i] = NULL;

	argv0 = argv;
	if (i > 0)
	    argv_lth = envp[i-1] + strlen(envp[i-1]) - argv0[0];
	else
	    argv_lth = argv0[argc-1] + strlen(argv0[argc-1]) - argv0[0];
#endif
	(void)new QuickLauncher();
	qDebug( "QuickLauncher running" );
	// Pre-load default fonts
	QFontMetrics fm( QApplication::font() );
	fm.ascent(); // causes font load.
	QFont f( QApplication::font() );
	f.setWeight( QFont::Bold );
	QFontMetrics fmb( f );
	fmb.ascent(); // causes font load.

	// Each of the following force internal structures/internal
	// initialization to be performed.  This may mean allocating
	// memory that is not needed by all applications.
#if 0
	TimeZone::current().isValid(); // popuplate timezone cache
	TimeString::currentDateFormat(); // create internal structures
	TimeString::currentAMPM();
#endif
	Resource::loadIconSet("new"); // do internal init

        /* make sure libopie gets lined in */
        {
            OWait item;
        }

	// Create a widget to force initialization of title bar images, etc.
	QObject::disconnect(app, SIGNAL(lastWindowClosed()), app, SLOT(hideOrQuit()));
	QWidget *w = new QWidget(0,0,Qt::WDestructiveClose|Qt::WStyle_ContextHelp|Qt::WStyle_Tool);
	w->setGeometry( -100, -100, 10, 10 );
	w->show();
	QTimer::singleShot( 0, w, SLOT(close()) );

	app->enter_loop();
    }

    int rv = app->exec();

    if ( mainWindow )
	delete (QWidget*)mainWindow;
    if ( appIface )
	loader->releaseInterface( appIface );
    delete loader;
    // The problem is there are some 'static' resources not freed
    // in the apps and on destructing these objects are not available
    // anymore. In future fix up the apps but for now
    // we just skip deletion and hope things go well -zecke
//    delete app;
    // hack instead -zecke
    delete app->pidChannel;
    app->pidChannel = 0;

    return rv;
}

#include "main.moc"
