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

#ifndef QT_H
# include <qfeatures.h>
#endif // QT_H

#ifndef QT_NO_PROCESS

//#include "qplatformdefs.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "process.h"
#include "qapplication.h"
//#include "qptrqueue.h"
//#include "qptrlist.h"
#include "qsocketnotifier.h"
#include "qtimer.h"
//#include "qcleanuphandler.h"
#include "qregexp.h"

#include <stdlib.h>
#include <errno.h>

#define QPtrList QList

//#define QT_QPROCESS_DEBUG


class Proc;
class ProcessManager;
class ProcessPrivate
{
public:
    ProcessPrivate();
    ~ProcessPrivate();

    void closeOpenSocketsForChild();
    void newProc( pid_t pid, Process *process );

    QByteArray bufStdout;
    QByteArray bufStderr;

    QSocketNotifier *notifierStdin;
    QSocketNotifier *notifierStdout;
    QSocketNotifier *notifierStderr;

    ssize_t stdinBufRead;
    Proc *proc;

    bool exitValuesCalculated;
    bool socketReadCalled;

    static ProcessManager *procManager;
};


/***********************************************************************
 *
 * Proc
 *
 **********************************************************************/
/*
  The class Process does not necessarily map exactly to the running
  child processes: if the process is finished, the Process class may still be
  there; furthermore a user can use Process to start more than one process.

  The helper-class Proc has the semantics that one instance of this class maps
  directly to a running child process.
*/
class Proc
{
public:
    Proc( pid_t p, Process *proc=0 ) : pid(p), process(proc)
    {
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "Proc: Constructor for pid %d and Process %p", pid, process );
#endif
	socketStdin = 0;
	socketStdout = 0;
	socketStderr = 0;
    }
    ~Proc()
    {
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "Proc: Destructor for pid %d and Process %p", pid, process );
#endif
	if ( process != 0 ) {
	    if ( process->d->notifierStdin )
		process->d->notifierStdin->setEnabled( FALSE );
	    if ( process->d->notifierStdout )
		process->d->notifierStdout->setEnabled( FALSE );
	    if ( process->d->notifierStderr )
		process->d->notifierStderr->setEnabled( FALSE );
	    process->d->proc = 0;
	}
	if( socketStdin != 0 )
	    ::close( socketStdin );
	// ### close these sockets even on parent exit or is it better only on
	// sigchld (but what do I have to do with them on exit then)?
	if( socketStdout != 0 )
	    ::close( socketStdout );
	if( socketStderr != 0 )
	    ::close( socketStderr );
    }

    pid_t pid;
    int socketStdin;
    int socketStdout;
    int socketStderr;
    Process *process;
};

/***********************************************************************
 *
 * ProcessManager
 *
 **********************************************************************/
class ProcessManager : public QObject
{
    Q_OBJECT

public:
    ProcessManager();
    ~ProcessManager();

    void append( Proc *p );
    void remove( Proc *p );

public slots:
    void removeMe();

public:
    struct sigaction oldactChld;
    struct sigaction oldactPipe;
    QPtrList<Proc> *procList;
    int sigchldFd[2];
};


ProcessManager::ProcessManager()
{
    procList = new QPtrList<Proc>;
    procList->setAutoDelete( TRUE );
}

ProcessManager::~ProcessManager()
{
    delete procList;
}

void ProcessManager::append( Proc *p )
{
    procList->append( p );
#if defined(QT_QPROCESS_DEBUG)
    qDebug( "ProcessManager: append process (procList.count(): %d)", procList->count() );
#endif
}

void ProcessManager::remove( Proc *p )
{
    procList->remove( p );
#if defined(QT_QPROCESS_DEBUG)
    qDebug( "ProcessManager: remove process (procList.count(): %d)", procList->count() );
#endif
    if ( procList->count() == 0 ) {
	QTimer::singleShot( 0, this, SLOT(removeMe()) );
    }
}

void ProcessManager::removeMe()
{
    ProcessPrivate::procManager = 0;
    delete this;
}

#include "process_unix.moc"


/***********************************************************************
 *
 * ProcessPrivate
 *
 **********************************************************************/
ProcessManager *ProcessPrivate::procManager = 0;

ProcessPrivate::ProcessPrivate()
{
#if defined(QT_QPROCESS_DEBUG)
    qDebug( "ProcessPrivate: Constructor" );
#endif
    stdinBufRead = 0;

    notifierStdin = 0;
    notifierStdout = 0;
    notifierStderr = 0;

    exitValuesCalculated = FALSE;
    socketReadCalled = FALSE;

    proc = 0;
}

ProcessPrivate::~ProcessPrivate()
{
#if defined(QT_QPROCESS_DEBUG)
    qDebug( "ProcessPrivate: Destructor" );
#endif

    if ( proc != 0 ) {
	if ( proc->socketStdin != 0 ) {
	    ::close( proc->socketStdin );
	    proc->socketStdin = 0;
	}
	proc->process = 0;
    }

    delete notifierStdin;
    delete notifierStdout;
    delete notifierStderr;
}

/*
  Closes all open sockets in the child process that are not needed by the child
  process. Otherwise one child may have an open socket on standard input, etc.
  of another child.
*/
void ProcessPrivate::closeOpenSocketsForChild()
{
    if ( procManager != 0 ) {
	if ( procManager->sigchldFd[0] != 0 )
	    ::close( procManager->sigchldFd[0] );
	if ( procManager->sigchldFd[1] != 0 )
	    ::close( procManager->sigchldFd[1] );

	// close also the sockets from other Process instances
	Proc *proc;
	for ( proc=procManager->procList->first(); proc!=0; proc=procManager->procList->next() ) {
	    ::close( proc->socketStdin );
	    ::close( proc->socketStdout );
	    ::close( proc->socketStderr );
	}
    }
}

void ProcessPrivate::newProc( pid_t pid, Process *process )
{
    proc = new Proc( pid, process );
    if ( procManager == 0 ) {
	procManager = new ProcessManager;
    }
    // the ProcessManager takes care of deleting the Proc instances
    procManager->append( proc );
}


/***********************************************************************
 *
 * Process
 *
 **********************************************************************/
/*!
  This private class does basic initialization.
*/
void Process::init()
{
    d = new ProcessPrivate();
    exitStat = 0;
    exitNormal = FALSE;
}

/*!
  Destroys the class.

  If the process is running, it is NOT terminated! Standard input, standard
  output and standard error of the process are closed.

  \sa hangUp() kill()
*/
Process::~Process()
{
    delete d;
}

bool Process::exec( const QByteArray& in, QByteArray& out, QStringList *env )
{
#if defined(QT_QPROCESS_DEBUG)
    qDebug( "Process::exec()" );
#endif

    int sStdin[2];
    int sStdout[2];
    int sStderr[2];

    // open sockets for piping
    if ( ::socketpair( AF_UNIX, SOCK_STREAM, 0, sStdin ) ) {
	return FALSE;
    }
    if ( ::socketpair( AF_UNIX, SOCK_STREAM, 0, sStderr ) ) {
	return FALSE;
    }
    if ( ::socketpair( AF_UNIX, SOCK_STREAM, 0, sStdout ) ) {
	return FALSE;
    }

    // the following pipe is only used to determine if the process could be
    // started
    int fd[2];
    if ( pipe( fd ) < 0 ) {
	// non critical error, go on
	fd[0] = 0;
	fd[1] = 0;
    }

    // construct the arguments for exec
    QCString *arglistQ = new QCString[ _arguments.count() + 1 ];
    const char** arglist = new const char*[ _arguments.count() + 1 ];
    int i = 0;
    for ( QStringList::Iterator it = _arguments.begin(); it != _arguments.end(); ++it ) {
	arglistQ[i] = (*it).local8Bit();
	arglist[i] = arglistQ[i];
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "Process::start(): arg %d = %s", i, arglist[i] );
#endif
	i++;
    }
    arglist[i] = 0;

    // fork and exec
    QApplication::flushX();
    pid_t pid = fork();
    if ( pid == 0 ) {
	// child
	d->closeOpenSocketsForChild();
	::close( sStdin[1] );
	::close( sStdout[0] );
	::close( sStderr[0] );
	::dup2( sStdin[0], STDIN_FILENO );
	::dup2( sStdout[1], STDOUT_FILENO );
	::dup2( sStderr[1], STDERR_FILENO );
	if ( fd[0] )
	    ::close( fd[0] );
	if ( fd[1] )
	    ::fcntl( fd[1], F_SETFD, FD_CLOEXEC ); // close on exec shows sucess

	if ( env == 0 ) { // inherit environment and start process
	    ::execvp( arglist[0], (char*const*)arglist ); // ### cast not nice
	} else { // start process with environment settins as specified in env
	    // construct the environment for exec
	    int numEntries = env->count();
	    bool setLibraryPath =
		env->grep( QRegExp( "^LD_LIBRARY_PATH=" ) ).isEmpty() &&
		getenv( "LD_LIBRARY_PATH" ) != 0;
	    if ( setLibraryPath )
		numEntries++;
	    QCString *envlistQ = new QCString[ numEntries + 1 ];
	    const char** envlist = new const char*[ numEntries + 1 ];
	    int i = 0;
	    if ( setLibraryPath ) {
		envlistQ[i] = QString( "LD_LIBRARY_PATH=%1" ).arg( getenv( "LD_LIBRARY_PATH" ) ).local8Bit();
		envlist[i] = envlistQ[i];
		i++;
	    }
	    for ( QStringList::Iterator it = env->begin(); it != env->end(); ++it ) {
		envlistQ[i] = (*it).local8Bit();
		envlist[i] = envlistQ[i];
		i++;
	    }
	    envlist[i] = 0;

	    // look for the executable in the search path
	    if ( _arguments.count()>0 && getenv("PATH")!=0 ) {
		QString command = _arguments[0];
		if ( !command.contains( '/' ) ) {
		    QStringList pathList = QStringList::split( ':', getenv( "PATH" ) );
		    for (QStringList::Iterator it = pathList.begin(); it != pathList.end(); ++it ) {
			QFileInfo fileInfo( *it, command );
			if ( fileInfo.isExecutable() ) {
			    arglistQ[0] = fileInfo.filePath().local8Bit();
			    arglist[0] = arglistQ[0];
			    break;
			}
		    }
		}
	    }
	    ::execve( arglist[0], (char*const*)arglist, (char*const*)envlist ); // ### casts not nice
	}
	if ( fd[1] ) {
	    char buf = 0;
	    ::write( fd[1], &buf, 1 );
	    ::close( fd[1] );
	}
	::exit( -1 );
    } else if ( pid == -1 ) {
	// error forking
	goto error;
    }
    // test if exec was successful
    if ( fd[1] )
	close( fd[1] );
    if ( fd[0] ) {
	char buf;
	for ( ;; ) {
	    int n = ::read( fd[0], &buf, 1 );
	    if ( n==1 ) {
		// socket was not closed => error
		goto error;
	    } else if ( n==-1 ) {
		if ( errno==EAGAIN || errno==EINTR )
		    // try it again
		    continue;
	    }
	    break;
	}
    }


    ::close( sStdin[0] );
    ::close( sStdout[1] );
    ::close( sStderr[1] );

    // DIFFERENT

    {
	int written=0;
	int readden=0; // sic.
	while (1) {
	    const int bufsize=4096;
	    struct timeval *timeout = 0; // #### could have this
	    fd_set r; FD_ZERO(&r);
	    fd_set w; FD_ZERO(&w);
	    FD_SET( sStdout[0], &r );
	    out.resize( readden+bufsize );
	    if ( int(in.size()) > written )
		FD_SET( sStdin[1], &w );
	    int highest = QMAX(sStdout[0],sStdin[1])+1;
	    select(highest, &r, &w, 0, timeout);
	    if ( FD_ISSET( sStdout[0], &r ) ) {
		int n = read( sStdout[0], out.data()+readden, bufsize );
		if ( n > 0 )
		    readden += n;
		else
		    break;
	    }
	    if ( FD_ISSET( sStdin[1], &w ) ) {
		int n = write( sStdin[1], in.data()+written, in.size()-written );
		if ( n > 0 )
		    written += n;
	    }
	}
	out.resize(readden);
    }

    // cleanup and return
    delete[] arglistQ;
    delete[] arglist;
    ::close( sStdin[1] );
    ::close( sStdout[0] );
    ::close( sStderr[0] );
    return TRUE;

error:
#if defined(QT_QPROCESS_DEBUG)
    qDebug( "Process::start(): error starting process" );
#endif
    ::close( sStdin[1] );
    ::close( sStdout[0] );
    ::close( sStderr[0] );
    ::close( sStdin[0] );
    ::close( sStdout[1] );
    ::close( sStderr[1] );
    ::close( fd[0] );
    ::close( fd[1] );
    delete[] arglistQ;
    delete[] arglist;
    return FALSE;
}


#endif // QT_NO_PROCESS
