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
#define QTOPIA_INTERNAL_LANGLIST
#include <qpe/qpedebug.h>
#include <qpe/global.h>
#include <qpe/qdawg.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/storage.h>
#include <qpe/applnk.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include "qpe/qcopenvelope_qws.h"
#endif

#include <qfile.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qmap.h>
#include <qdict.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <qwindowsystem_qws.h> // for qwsServer
#include <qdatetime.h>

#include <qfile.h>

namespace {
  // checks if the storage should be searched
  bool checkStorage(const QString &path ){ // this is a small Config replacement cause config is too limited -zecke
    QFile file(path );
    if(!file.open(IO_ReadOnly ) )
       return true;
    
    QByteArray array = file.readAll();
    QStringList list = QStringList::split('\n', QString( array ) );
    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it ){
      if( (*it).startsWith("autocheck = 0" ) ){
	return true;
      }else if( (*it).startsWith("autocheck = 1" ) ){
	return false;
      }
    }
    return true;
  }
}

//#include "quickexec_p.h"

class Emitter : public QObject {
    Q_OBJECT
public:
    Emitter( QWidget* receiver, const QString& document )
    {
	connect(this, SIGNAL(setDocument(const QString&)),
	    receiver, SLOT(setDocument(const QString&)));
	emit setDocument(document);
	disconnect(this, SIGNAL(setDocument(const QString&)),
	       receiver, SLOT(setDocument(const QString&)));
    }

signals:
    void setDocument(const QString&);
};


class StartingAppList : public QObject {
    Q_OBJECT
public:
    static void add( const QString& name );
    static bool isStarting( const QString name );
private slots:
    void handleNewChannel( const QString &);
private:
    StartingAppList( QObject *parent=0, const char* name=0 ) ;

    QDict<QTime> dict;
    static StartingAppList *appl;
};

StartingAppList* StartingAppList::appl = 0;

StartingAppList::StartingAppList( QObject *parent, const char* name )
    :QObject( parent, name )
{
#if QT_VERSION >= 232 && !defined(QT_NO_COP)
    connect( qwsServer, SIGNAL( newChannel(const QString&)),
	     this, SLOT( handleNewChannel(const QString&)) );
    dict.setAutoDelete( TRUE );
#endif
}

void StartingAppList::add( const QString& name )
{
#if QT_VERSION >= 232  && !defined(QT_NO_COP)
    if ( !appl )
	appl = new StartingAppList;
    QTime *t = new QTime;
    t->start();
    appl->dict.insert( "QPE/Application/" + name, t );
#endif
}

bool StartingAppList::isStarting( const QString name )
{
#if QT_VERSION >= 232  && !defined(QT_NO_COP)
    if ( appl ) {
	QTime *t  = appl->dict.find( "QPE/Application/" + name );
	if ( !t )
	    return FALSE;
	if ( t->elapsed() > 10000 ) {
	    // timeout in case of crash or something
	    appl->dict.remove( "QPE/Application/" + name );
	    return FALSE;
	}
	return TRUE;
    }
#endif
    return FALSE;
}

void StartingAppList::handleNewChannel( const QString & name )
{
#if QT_VERSION >= 232  && !defined(QT_NO_COP)
    dict.remove( name );
#endif
}

static bool docDirCreated = FALSE;
static QDawg* fixed_dawg = 0;
static QDict<QDawg> *named_dawg = 0;

static QString qpeDir()
{
    QString dir = getenv("OPIEDIR");
    if ( dir.isEmpty() ) dir = "..";
    return dir;
}

static QString dictDir()
{
    return qpeDir() + "/etc/dict";
}

/*!
  \class Global global.h
  \brief The Global class collects application-wide global functions.
*/

/*!
  \internal
*/
Global::Global()
{
}

/*!
  Returns the unchangeable QDawg that contains general
  words for the current locale.

  \sa addedDawg()
*/
const QDawg& Global::fixedDawg()
{
    if ( !fixed_dawg ) {
	if ( !docDirCreated )
	    createDocDir();

	fixed_dawg = new QDawg;
	QString dawgfilename = dictDir() + "/dawg";
	QString words_lang;
	QStringList langs = Global::languageList();
	for (QStringList::ConstIterator it = langs.begin(); it!=langs.end(); ++it) {
	    QString lang = *it;
	    words_lang = dictDir() + "/words." + lang;
	    QString dawgfilename_lang = dawgfilename + "." + lang;
	    if ( QFile::exists(dawgfilename_lang) ||
		 QFile::exists(words_lang) ) {
		dawgfilename = dawgfilename_lang;
		break;
	    }
	}
	QFile dawgfile(dawgfilename);

	if ( !dawgfile.exists() ) {
	    QString fn = dictDir() + "/words";
	    if ( QFile::exists(words_lang) )
		fn = words_lang;
	    QFile in(fn);
	    if ( in.open(IO_ReadOnly) ) {
		fixed_dawg->createFromWords(&in);
		dawgfile.open(IO_WriteOnly);
		fixed_dawg->write(&dawgfile);
		dawgfile.close();
	    }
	} else {
	    fixed_dawg->readFile(dawgfilename);
	}
    }

    return *fixed_dawg;
}

/*!
  Returns the changeable QDawg that contains general
  words for the current locale.

  \sa fixedDawg()
*/
const QDawg& Global::addedDawg()
{
    return dawg("local");
}

/*!
  Returns the QDawg with the given \a name.
  This is an application-specific word list.

  \a name should not contain "/".
*/
const QDawg& Global::dawg(const QString& name)
{
    createDocDir();
    if ( !named_dawg )
	named_dawg = new QDict<QDawg>;
    QDawg* r = named_dawg->find(name);
    if ( !r ) {
	r = new QDawg;
	named_dawg->insert(name,r);
	QString dawgfilename = dictDir() + "/" + name + ".dawg";
	QFile dawgfile(dawgfilename);
	if ( dawgfile.open(IO_ReadOnly) )
	    r->readFile(dawgfilename);
    }
    return *r;
}

/*!
  Adds \a wordlist to the addedDawg().
*/
void Global::addWords(const QStringList& wordlist)
{
    addWords("local",wordlist);
}

/*!
  Adds \a wordlist to the dawg() named \a dictname.
*/
void Global::addWords(const QString& dictname, const QStringList& wordlist)
{
    QDawg& d = (QDawg&)dawg(dictname);
    QStringList all = d.allWords() + wordlist;
    d.createFromWords(all);

    QString dawgfilename = dictDir() + "/" + dictname + ".dawg";
    QFile dawgfile(dawgfilename);
    if ( dawgfile.open(IO_WriteOnly) ) {
	d.write(&dawgfile);
	dawgfile.close();
    }

    // #### Re-read the dawg here if we use mmap().

    // #### Signal other processes to re-read.
}


/*!
  Returns a full path for the application named \a appname, with the
  given \a filename or QString::null if there was a problem creating
  the directory tree for \a appname.  
  If \a filename contains "/", it is the caller's responsibility to 
  ensure those directories exist.
*/
QString Global::applicationFileName(const QString& appname, const QString& filename)
{
    QDir d;
    QString r = getenv("HOME");
    r += "/Applications/";
    if ( !QFile::exists( r ) )
	if ( d.mkdir(r) == false )
	    return QString::null;
    r += appname;
    if ( !QFile::exists( r ) )
	if ( d.mkdir(r) == false )
	    return QString::null;
    r += "/"; r += filename;
    return r;
}

/*!
  \internal
*/
void Global::createDocDir()
{
    if ( !docDirCreated ) {
	docDirCreated = TRUE;
	mkdir( QPEApplication::documentDir().latin1(), 0755 );
    }
}


/*!
  Displays a status \a message to the user. This generally appears
  in the taskbar for some amount of time, then disappears.
*/
void Global::statusMessage(const QString& message)
{
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopEnvelope e( "QPE/TaskBar", "message(QString)" );
    e << message;
#endif
}

/*!
  \internal
*/
void Global::applyStyle()
{
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel::send( "QPE/System", "applyStyle()" );
#else
    ((QPEApplication *)qApp)->applyStyle(); // apply without needing QCop for floppy version
#endif
}

/*!
  \internal
*/
QWidget *Global::shutdown( bool )
{
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel::send( "QPE/System", "shutdown()" );
#endif
    return 0;
}

/*!
  \internal
*/
QWidget *Global::restart( bool )
{
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel::send( "QPE/System", "restart()" );
#endif
    return 0;
}

/*!
  Explicitly show the current input method.
*/
void Global::showInputMethod()
{
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel::send( "QPE/TaskBar", "showInputMethod()" );
#endif
}

/*!
  Explicitly hide the current input method.
*/
void Global::hideInputMethod()
{
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel::send( "QPE/TaskBar", "hideInputMethod()" );
#endif
}


/*!
  \internal
*/
bool Global::isBuiltinCommand( const QString &name )
{
    if(!builtin)
	return FALSE; // yes, it can happen
    for (int i = 0; builtin[i].file; i++) {
	if ( builtin[i].file == name ) {
	    return TRUE;
	}
    }
    return FALSE;
}

Global::Command* Global::builtin=0;
QGuardedPtr<QWidget> *Global::running=0;

/*!
  \class Global::Command
  \brief The Global::Command class is internal.
  \internal
*/

/*!
  \internal
*/
void Global::setBuiltinCommands( Command* list )
{
    if ( running )
	delete [] running;

    builtin = list;
    int count = 0;
    if (!builtin)
	return;
    while ( builtin[count].file )
	count++;

    running = new QGuardedPtr<QWidget> [ count ];
}

/*!
  \internal
*/
void Global::setDocument( QWidget* receiver, const QString& document )
{
    Emitter emitter(receiver,document);
}

/*!
  \internal
*/
bool Global::terminateBuiltin( const QString& n )
{
    if (!builtin)
	return FALSE;
    for (int i = 0; builtin[i].file; i++) {
	if ( builtin[i].file == n ) {
	    delete running[i];
	    return TRUE;
	}
    }
    return FALSE;
}

/*!
  \internal
*/
void Global::terminate( const AppLnk* app )
{
    //if ( terminateBuiltin(app->exec()) ) return; // maybe? haven't tried this

    QCString channel = "QPE/Application/" + app->exec().utf8();
    if ( QCopChannel::isRegistered(channel) ) {
	QCopEnvelope e(channel, "quit()");
    }
}

/*!
  Low-level function to run command \a c. Not recommended.
*/
void Global::invoke(const QString &c) 
{
    // Convert the command line in to a list of arguments
    QStringList list = QStringList::split(QRegExp("  *"),c);

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QString ap=list[0];
    // see if the application is already running
    // XXX should lock file /tmp/qcop-msg-ap 
    if ( QCopChannel::isRegistered( ("QPE/Application/" + ap).latin1() ) ) {
	QCopEnvelope e("QPE/System", "notBusy(QString)" );
	e << ap;
	return;
    }
    // XXX should unlock file /tmp/qcop-msg-ap 
    //see if it is being started
    if ( StartingAppList::isStarting( ap ) ) {
	QCopEnvelope e("QPE/System", "notBusy(QString)" );
	e << ap;
	return;
    }

#endif

#ifdef QT_NO_QWS_MULTIPROCESS
    QMessageBox::warning( 0, "Error", "Could not find the application " + c, "Ok", 0, 0, 0, 1 );
#else

    QStrList slist;
    unsigned int j;
    for ( j = 0; j < list.count(); j++ )
	slist.append( list[j].utf8() );

    const char **args = new (const char *)[slist.count() + 1];
    for ( j = 0; j < slist.count(); j++ )
	args[j] = slist.at(j);
    args[j] = NULL;

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    // an attempt to show a wait...
    // more logic should be used, but this will be fine for the moment...
    QCopEnvelope ( "QPE/System", "busy()" );
#endif

#ifdef HAVE_QUICKEXEC
    QString libexe = qpeDir()+"/binlib/lib"+args[0] + ".so";
    qDebug("libfile = %s", libexe.latin1() );
    if ( QFile::exists( libexe ) ) {
	qDebug("calling quickexec %s", libexe.latin1() );
	quickexecv( libexe.utf8().data(), (const char **)args );
    } else 
#endif
    {
	if ( !::vfork() ) {
	    for ( int fd = 3; fd < 100; fd++ )
		::close( fd );
	    ::setpgid( ::getpid(), ::getppid() );
	    // Try bindir first, so that foo/bar works too
	    ::execv( qpeDir()+"/bin/"+args[0], (char * const *)args );
	    ::execvp( args[0], (char * const *)args );
	    _exit( -1 );
	}
    }
    StartingAppList::add( list[0] );
#endif //QT_NO_QWS_MULTIPROCESS
}

/*!
  Executes application identfied by \a c, passing \a document.

  Note that you might be better off sending a QCop message to
  the application's QPE/Application/<i>appname</i> channel.
*/
void Global::execute( const QString &c, const QString& document )
{
    if ( qApp->type() != QApplication::GuiServer ) {
	// ask the server to do the work
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
	if ( document.isNull() ) {
	    QCopEnvelope e( "QPE/System", "execute(QString)" );
	    e << c;
	} else {
	    QCopEnvelope e( "QPE/System", "execute(QString,QString)" );
	    e << c << document;
	}
#endif
	return;
    }

    // Attempt to execute the app using a builtin class for the app first
    // else try and find it in the bin directory
    if (builtin) {
	for (int i = 0; builtin[i].file; i++) {
	    if ( builtin[i].file == c ) {
		if ( running[i] ) {
		    if ( !document.isNull() && builtin[i].documentary )
			setDocument(running[i], document);
		    running[i]->raise();
		    running[i]->show();
		    running[i]->setActiveWindow();
		} else {
		    running[i] = builtin[i].func( builtin[i].maximized );
		}
		QCopEnvelope e("QPE/System", "notBusy(QString)" );
		e << c; // that was quick ;-)
		return;
	    }
	}
    }

    //Global::invoke(c, document);

    // Convert the command line in to a list of arguments
    QStringList list = QStringList::split(QRegExp("  *"),c);

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QString ap=list[0];

    qDebug("executing %s", ap.latin1() );
    if ( ap == "suspend" ) {
	QWSServer::sendKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );	
	return;
    }

    /* if need be, sending a qcop message will result in an invoke, see 
       preceeding function */
    { QCopEnvelope env( ("QPE/Application/" + ap).latin1(), "raise()" ); }
    if ( !document.isEmpty() ) {
	QCopEnvelope env( ("QPE/Application/" + ap).latin1(), "setDocument(QString)" );
	env << document;
    }
#endif
}

/*!
  Returns the string \a s with the characters backslash, ", and $
  quoted by a preceeding backslash.
*/
QString Global::shellQuote(const QString& s)
{
    QString r="\"";
    for (int i=0; i<(int)s.length(); i++) {
	char c = s[i].latin1();
	switch (c) {
	    case '\\': case '"': case '$':
		r+="\\";
	}
	r += s[i];
    }
    r += "\"";
    return r;
}

/*!
  Returns the string \a s with the characters backslash and "
  quoted by a preceeding backslash.
*/
QString Global::stringQuote(const QString& s)
{
    QString r="\"";
    for (int i=0; i<(int)s.length(); i++) {
	char c = s[i].latin1();
	switch (c) {
	    case '\\': case '"':
		r+="\\";
	}
	r += s[i];
    }
    r += "\"";
    return r;
}

/*!
  Finds all documents on the system's document directories which
  match the filter \a mimefilter, and appends the resulting DocLnk
  objects to \a folder.
*/
void Global::findDocuments(DocLnkSet* folder, const QString &mimefilter)
{
    QString homedocs = QString(getenv("HOME")) + "/Documents";
    DocLnkSet d(homedocs,mimefilter);
    folder->appendFrom(d);
    StorageInfo storage;
    const QList<FileSystem> &fs = storage.fileSystems();
    QListIterator<FileSystem> it ( fs );
    for ( ; it.current(); ++it ) {
      if ( (*it)->isRemovable() ) { // let's find out  if we should search on it
	// this is a candidate look at the cf and see if we should search on it 
	QString path = (*it)->path();
	if( !checkStorage((*it)->path() + "/.opiestorage.cf" ) )
	  continue;
	DocLnkSet ide( path, mimefilter );
	folder->appendFrom(ide);
      }
    }
}

QStringList Global::languageList()
{
    QString lang = getenv("LANG");
    QStringList langs;
    langs.append(lang);
    int i  = lang.find(".");
    if ( i > 0 )
	lang = lang.left( i );
    i = lang.find( "_" );
    if ( i > 0 )
	langs.append(lang.left(i));
    return langs;
}

QStringList Global::helpPath()
{
    QStringList path;
    QStringList langs = Global::languageList();
    for (QStringList::ConstIterator it = langs.fromLast(); it!=langs.end(); --it) {
	QString lang = *it;
	if ( !lang.isEmpty() )
	    path += QPEApplication::qpeDir() + "/help/" + lang + "/html";
    }
    path += QPEApplication::qpeDir() + "/pics";
    path += QPEApplication::qpeDir() + "/help/en/html";
    path += QPEApplication::qpeDir() + "/docs";
    QString dir = QDir::current().canonicalPath();
    if ( dir == "/" )
	dir += "/docs";
    else {
	path += dir + "/../pics";
	dir += "/../docs";
	path += dir;
    }
    return path;
}


#include "global.moc"
