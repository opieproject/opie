/****************************************************************************
** $Id: qprocess.cpp,v 1.1 2002-01-25 22:14:56 kergoth Exp $
**
** Implementation of QProcess class
**
** Created : 20000905
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "qprocess.h"

#ifndef QT_NO_PROCESS

#include "qapplication.h"


//#define QT_QPROCESS_DEBUG


/*!
  \class QProcess qprocess.h

  \brief The QProcess class is used to start external programs and to
  communicate with them.

  This is a temporary class. This will be replaced by Qt 3's QProcess class.
*/

/*!
  \enum QProcess::Communication

  This enum type defines the communication channels connected to the
  process.

  \value Stdin  Data can be written to the process's standard input.

  \value Stdout  Data can be read from the process's standard output.

  \value Stderr  Data can be read from the process's standard error.

  \value DupStderr  Duplicates standard error to standard output for new
  processes; i.e.  everything that the process writes to standard error, is
  reported by QProcess on standard output instead. This is especially useful if
  your application requires that the output on standard output and standard
  error is read in the same order as the process output it. Please note that
  this is a binary flag, so if you want to activate this together with standard
  input, output and error redirection (the default), you have to specify
  \c{Stdin|Stdout|Stderr|DupStderr} for the setCommunication() call.

  \sa setCommunication() communication()
*/

/*!
  Constructs a QProcess object. The \a parent and \a name parameters are passed
  to the QObject constructor.

  \sa setArguments() addArgument() start()
*/
QProcess::QProcess( QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( FALSE ), notifyOnExit( FALSE ),
    wroteToStdinConnected( FALSE ),
    readStdoutCalled( FALSE ), readStderrCalled( FALSE ),
    comms( Stdin|Stdout|Stderr )
{
    init();
}

/*!
  Constructs a QProcess with \a arg0 as the command to be executed. The
  \a parent and \a name parameters are passed to the QObject constructor.

  The process is not started. You must call start() or launch()
  to start the process.

  \sa setArguments() addArgument() start()
*/
QProcess::QProcess( const QString& arg0, QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( FALSE ), notifyOnExit( FALSE ),
    wroteToStdinConnected( FALSE ),
    readStdoutCalled( FALSE ), readStderrCalled( FALSE ),
    comms( Stdin|Stdout|Stderr )
{
    init();
    addArgument( arg0 );
}

/*!
  Constructs a QProcess with \a args as the arguments of the process. The first
  element in the list is the command to be executed. The other elements in the
  list are the arguments to this command. The \a parent and \a name
  parameters are passed to the QObject constructor.

  The process is not started. You must call start() or launch()
  to start the process.

  \sa setArguments() addArgument() start()
*/
QProcess::QProcess( const QStringList& args, QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( FALSE ), notifyOnExit( FALSE ),
    wroteToStdinConnected( FALSE ),
    readStdoutCalled( FALSE ), readStderrCalled( FALSE ),
    comms( Stdin|Stdout|Stderr )
{
    init();
    setArguments( args );
}


/*!
  Returns the list of arguments that are set for the process. Arguments can be
  specified with the constructor or with the functions setArguments() and
  addArgument().

  \sa setArguments() addArgument()
*/
QStringList QProcess::arguments() const
{
    return _arguments;
}

/*!
  Clears the list of arguments that are set for the process.

  \sa setArguments() addArgument()
*/
void QProcess::clearArguments()
{
    _arguments.clear();
}

/*!
  Sets \a args as the arguments for the process. The first element in the list
  is the command to be executed. The other elements in the list are the
  arguments to the command. Any previous arguments are deleted.

  \sa arguments() addArgument()
*/
void QProcess::setArguments( const QStringList& args )
{
    _arguments = args;
}

/*!
  Adds \a arg to the end of the list of arguments.

  The first element in the list of arguments is the command to be
  executed; the following elements are the arguments to the command.

  \sa arguments() setArguments()
*/
void QProcess::addArgument( const QString& arg )
{
    _arguments.append( arg );
}

#ifndef QT_NO_DIR
/*!
  Returns the working directory that was set with
  setWorkingDirectory(), or the current directory if none has been
  set.

  \sa setWorkingDirectory() QDir::current()
*/
QDir QProcess::workingDirectory() const
{
    return workingDir;
}

/*!
  Sets \a dir as the working directory for a process. This does not affect
  running processes; only processes that are started afterwards are affected.

  Setting the working directory is especially useful for processes that try to
  access files with relative filenames.

  \sa workingDirectory() start()
*/
void QProcess::setWorkingDirectory( const QDir& dir )
{
    workingDir = dir;
}
#endif //QT_NO_DIR

/*!
  Returns the communication required with the process.

  \sa setCommunication()
*/
int QProcess::communication() const
{
    return comms;
}

/*!
  Sets \a commFlags as the communication required with the process.

  \a commFlags is a bitwise OR between the flags defined in \c Communication.

  The default is \c{Stdin|Stdout|Stderr}.

  \sa communication()
*/
void QProcess::setCommunication( int commFlags )
{
    comms = commFlags;
}

/*!
  Returns TRUE if the process has exited normally; otherwise returns
  FALSE. This implies that this function returns FALSE if the process
  is still running.

  \sa isRunning() exitStatus() processExited()
*/
bool QProcess::normalExit() const
{
    // isRunning() has the side effect that it determines the exit status!
    if ( isRunning() )
	return FALSE;
    else
	return exitNormal;
}

/*!
  Returns the exit status of the process or 0 if the process is still
  running. This function returns immediately and does not wait until
  the process is finished.

  If normalExit() is FALSE (e.g. if the program was killed or
  crashed), this function returns 0, so you should check the return
  value of normalExit() before relying on this value.

  \sa normalExit() processExited()
*/
int QProcess::exitStatus() const
{
    // isRunning() has the side effect that it determines the exit status!
    if ( isRunning() )
	return 0;
    else
	return exitStat;
}


/*!
  Reads the data that the process has written to standard output. When
  new data is written to standard output, the class emits the signal
  readyReadStdout().

  If there is no data to read, this function returns a QByteArray of
  size 0: it does not wait until there is something to read.

  \sa readyReadStdout() readLineStdout() readStderr() writeToStdin()
*/
QByteArray QProcess::readStdout()
{
    if ( readStdoutCalled ) {
	return QByteArray();
    }
    readStdoutCalled = TRUE;

    QByteArray buf = bufStdout()->copy();
    consumeBufStdout( -1 ); // consume everything

    readStdoutCalled = FALSE;
    return buf;
}

/*!
  Reads the data that the process has written to standard error. When
  new data is written to standard error, the class emits the signal
  readyReadStderr().

  If there is no data to read, this function returns a QByteArray of
  size 0: it does not wait until there is something to read.

  \sa readyReadStderr() readLineStderr() readStdout() writeToStdin()
*/
QByteArray QProcess::readStderr()
{
    if ( readStderrCalled ) {
	return QByteArray();
    }
    readStderrCalled = TRUE;

    QByteArray buf = bufStderr()->copy();
    consumeBufStderr( -1 ); // consume everything

    readStderrCalled = FALSE;
    return buf;
}

/*!
  Returns TRUE if it's possible to read an entire line of text from
  standard output at this time; otherwise returns FALSE.

  \sa readLineStdout() canReadLineStderr()
*/
bool QProcess::canReadLineStdout() const
{
    QProcess *that = (QProcess*)this;
    return that->scanNewline( TRUE, 0 );
}

/*!
  Returns TRUE if it's possible to read an entire line of text from
  standard error at this time; otherwise returns FALSE.

  \sa readLineStderr() canReadLineStdout()
*/
bool QProcess::canReadLineStderr() const
{
    QProcess *that = (QProcess*)this;
    return that->scanNewline( FALSE, 0 );
}

/*!
  Reads a line of text from standard output, excluding any trailing newline or
  carriage return characters, and returns it. Returns QString::null if
  canReadLineStdout() returns FALSE.

  \sa canReadLineStdout() readyReadStdout() readStdout() readLineStderr()
*/
QString QProcess::readLineStdout()
{
    QByteArray a;
    QString s;
    if ( scanNewline( TRUE, &a ) ) {
	if ( a.isEmpty() )
	    s = "";
	else
	    s = QString( a );
    }
    return s;
}

/*!
  Reads a line of text from standard error, excluding any trailing newline or
  carriage return characters and returns it. Returns QString::null if
  canReadLineStderr() returns FALSE.

  \sa canReadLineStderr() readyReadStderr() readStderr() readLineStdout()
*/
QString QProcess::readLineStderr()
{
    QByteArray a;
    QString s;
    if ( scanNewline( FALSE, &a ) ) {
	if ( a.isEmpty() )
	    s = "";
	else
	    s = QString( a );
    }
    return s;
}

/*!
  This private function scans for any occurrence of \n or \r\n in the
  buffer \e buf. It stores the text in the byte array \a store if it is
  non-null.
*/
bool QProcess::scanNewline( bool stdOut, QByteArray *store )
{
    QByteArray *buf;
    if ( stdOut )
	buf = bufStdout();
    else
	buf = bufStderr();
    uint n = buf->size();
    uint i;
    for ( i=0; i<n; i++ ) {
	if ( buf->at(i) == '\n' ) {
	    break;
	}
    }
    if ( i >= n )
	return FALSE;

    if ( store ) {
	uint lineLength = i;
	if ( lineLength>0 && buf->at(lineLength-1) == '\r' )
	    lineLength--; // (if there are two \r, let one stay)
	store->resize( lineLength );
	memcpy( store->data(), buf->data(), lineLength );
	if ( stdOut )
	    consumeBufStdout( i+1 );
	else
	    consumeBufStderr( i+1 );
    }
    return TRUE;
}

/*!
  \fn void QProcess::launchFinished()

  This signal is emitted when the process was started with launch().
  If the start was successful, this signal is emitted after all the
  data has been written to standard input. If the start failed, then
  this signal is emitted immediately.

  \sa launch() QObject::deleteLater()
*/

/*!
  Runs the process and writes the data \a buf to the process's standard input.
  If all the data is written to standard input, standard input is
  closed. The command is searched for in the path for executable programs;
  you can also use an absolute path in the command itself.

  If \a env is null, then the process is started with the same environment as
  the starting process. If \a env is non-null, then the values in the
  stringlist are interpreted as environment setttings of the form \c
  {key=value} and the process is started with these environment settings. For
  convenience, there is a small exception to this rule under Unix: if \a env
  does not contain any settings for the environment variable \c
  LD_LIBRARY_PATH, then this variable is inherited from the starting process.

  Returns TRUE if the process could be started; otherwise returns FALSE.

  Note that you should not use the slots writeToStdin() and closeStdin() on
  processes started with launch(), since the result is not well-defined. If you
  need these slots, use start() instead.

  The process may or may not read the \a buf data sent to its standard
  input.

  You can call this function even when a process that was started with
  this instance is still running. Be aware that if you do this the
  standard input of the process that was launched first will be
  closed, with any pending data being deleted, and the process will be
  left to run out of your control. Similarly, if the process could not
  be started the standard input will be closed and the pending data
  deleted. (On operating systems that have zombie processes, Qt will
  also wait() on the old process.)

  The object emits the signal launchFinished() when this function
  call is finished. If the start was successful, this signal is
  emitted after all the data has been written to standard input. If
  the start failed, then this signal is emitted immediately.

  \sa start() launchFinished();
*/
bool QProcess::launch( const QByteArray& buf, QStringList *env )
{
    if ( start( env ) ) {
	if ( !buf.isEmpty() ) {
	    connect( this, SIGNAL(wroteToStdin()),
		    this, SLOT(closeStdinLaunch()) );
	    writeToStdin( buf );
	} else {
	    closeStdin();
	    emit launchFinished();
	}
	return TRUE;
    } else {
	emit launchFinished();
	return FALSE;
    }
}

/*! \overload

  The data \a buf is written to standard input with writeToStdin()
  using the QString::local8Bit() representation of the strings.
*/
bool QProcess::launch( const QString& buf, QStringList *env )
{
    if ( start( env ) ) {
	if ( !buf.isEmpty() ) {
	    connect( this, SIGNAL(wroteToStdin()),
		    this, SLOT(closeStdinLaunch()) );
	    writeToStdin( buf );
	} else {
	    closeStdin();
	    emit launchFinished();
	}
	return TRUE;
    } else {
	emit launchFinished();
	return FALSE;
    }
}

/*!
  This private slot is used by the launch() functions to close standard input.
*/
void QProcess::closeStdinLaunch()
{
    disconnect( this, SIGNAL(wroteToStdin()),
	    this, SLOT(closeStdinLaunch()) );
    closeStdin();
    emit launchFinished();
}


/*!
  \fn void QProcess::readyReadStdout()

  This signal is emitted when the process has written data to standard output.
  You can read the data with readStdout().

  Note that this signal is only emitted when there is new data and not
  when there is old, but unread data. In the slot connected to this signal, you
  should always read everything that is available at that moment to make sure
  that you don't lose any data.

  \sa readStdout() readLineStdout() readyReadStderr()
*/
/*!
  \fn void QProcess::readyReadStderr()

  This signal is emitted when the process has written data to standard error.
  You can read the data with readStderr().

  Note that this signal is only emitted when there is new data and not
  when there is old, but unread data. In the slot connected to this signal, you
  should always read everything that is available at that moment to make sure
  that you don't lose any data.

  \sa readStderr() readLineStderr() readyReadStdout()
*/
/*!
  \fn void QProcess::processExited()

  This signal is emitted when the process has exited.

  \sa isRunning() normalExit() exitStatus() start() launch()
*/
/*!
  \fn void QProcess::wroteToStdin()

  This signal is emitted if the data sent to standard input (via
  writeToStdin()) was actually written to the process. This does not
  imply that the process really read the data, since this class only detects
  when it was able to write the data to the operating system. But it is now
  safe to close standard input without losing pending data.

  \sa writeToStdin() closeStdin()
*/


/*! \overload

  The string \a buf is handled as text using
  the QString::local8Bit() representation.
*/
void QProcess::writeToStdin( const QString& buf )
{
    QByteArray tmp = buf.local8Bit();
    tmp.resize( buf.length() );
    writeToStdin( tmp );
}


/*
 * Under Windows the implementation is not so nice: it is not that easy to
 * detect when one of the signals should be emitted; therefore there are some
 * timers that query the information.
 * To keep it a little efficient, use the timers only when they are needed.
 * They are needed, if you are interested in the signals. So use
 * connectNotify() and disconnectNotify() to keep track of your interest.
 */
/*!  \reimp
*/
void QProcess::connectNotify( const char * signal )
{
#if defined(QT_QPROCESS_DEBUG)
    qDebug( "QProcess::connectNotify(): signal %s has been connected", signal );
#endif
    if ( !ioRedirection )
	if ( qstrcmp( signal, SIGNAL(readyReadStdout()) )==0 ||
		qstrcmp( signal, SIGNAL(readyReadStderr()) )==0
	   ) {
#if defined(QT_QPROCESS_DEBUG)
	    qDebug( "QProcess::connectNotify(): set ioRedirection to TRUE" );
#endif
	    setIoRedirection( TRUE );
	    return;
	}
    if ( !notifyOnExit && qstrcmp( signal, SIGNAL(processExited()) )==0 ) {
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "QProcess::connectNotify(): set notifyOnExit to TRUE" );
#endif
	setNotifyOnExit( TRUE );
	return;
    }
    if ( !wroteToStdinConnected && qstrcmp( signal, SIGNAL(wroteToStdin()) )==0 ) {
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "QProcess::connectNotify(): set wroteToStdinConnected to TRUE" );
#endif
	setWroteStdinConnected( TRUE );
	return;
    }
}

/*!  \reimp
*/
void QProcess::disconnectNotify( const char * )
{
    if ( ioRedirection &&
	    receivers( SIGNAL(readyReadStdout()) ) ==0 &&
	    receivers( SIGNAL(readyReadStderr()) ) ==0
	    ) {
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "QProcess::disconnectNotify(): set ioRedirection to FALSE" );
#endif
	setIoRedirection( FALSE );
    }
    if ( notifyOnExit && receivers( SIGNAL(processExited()) ) == 0 ) {
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "QProcess::disconnectNotify(): set notifyOnExit to FALSE" );
#endif
	setNotifyOnExit( FALSE );
    }
    if ( wroteToStdinConnected && receivers( SIGNAL(wroteToStdin()) ) == 0 ) {
#if defined(QT_QPROCESS_DEBUG)
	qDebug( "QProcess::disconnectNotify(): set wroteToStdinConnected to FALSE" );
#endif
	setWroteStdinConnected( FALSE );
    }
}

#endif // QT_NO_PROCESS
