/****************************************************************************
** $Id: qprocess.h,v 1.1 2002-01-31 17:04:17 kergoth Exp $
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

#ifndef QPROCESS_H
#define QPROCESS_H

#ifndef QT_H
#include "qobject.h"
#include "qstringlist.h"
#include "qdir.h"
#endif // QT_H

#ifndef QT_NO_PROCESS

class QProcessPrivate;


class Q_EXPORT QProcess : public QObject
{
    Q_OBJECT
public:
    QProcess( QObject *parent=0, const char *name=0 );
    QProcess( const QString& arg0, QObject *parent=0, const char *name=0 );
    QProcess( const QStringList& args, QObject *parent=0, const char *name=0 );
    ~QProcess();

    // set and get the arguments and working directory
    QStringList arguments() const;
    void clearArguments();
    virtual void setArguments( const QStringList& args );
    virtual void addArgument( const QString& arg );
#ifndef QT_NO_DIR
    QDir workingDirectory() const;
    virtual void setWorkingDirectory( const QDir& dir );
#endif

    // set and get the comms wanted
    enum Communication { Stdin=0x01, Stdout=0x02, Stderr=0x04, DupStderr=0x08 };
    void setCommunication( int c );
    int communication() const;

    // start the execution
    virtual bool start( QStringList *env=0 );
    virtual bool launch( const QString& buf, QStringList *env=0  );
    virtual bool launch( const QByteArray& buf, QStringList *env=0  );

    // inquire the status
    bool isRunning() const;
    bool normalExit() const;
    int exitStatus() const;

    // reading
    virtual QByteArray readStdout();
    virtual QByteArray readStderr();
    bool canReadLineStdout() const;
    bool canReadLineStderr() const;
    virtual QString readLineStdout();
    virtual QString readLineStderr();

    // get platform dependent process information
#if defined(Q_OS_WIN32)
    typedef void* PID;
#else
    typedef long Q_LONG;
    typedef Q_LONG PID;
#endif
    PID processIdentifier();

    void flushStdin();

signals:
    void readyReadStdout();
    void readyReadStderr();
    void processExited();
    void wroteToStdin();
    void launchFinished();

public slots:
    // end the execution
    void tryTerminate() const;
    void kill() const;

    // input
    virtual void writeToStdin( const QByteArray& buf );
    virtual void writeToStdin( const QString& buf );
    virtual void closeStdin();

protected: // ### or private?
    void connectNotify( const char * signal );
    void disconnectNotify( const char * signal );
private:
    void setIoRedirection( bool value );
    void setNotifyOnExit( bool value );
    void setWroteStdinConnected( bool value );

    void init();
    void reset();
#if defined(Q_OS_WIN32)
    uint readStddev( HANDLE dev, char *buf, uint bytes );
#endif
    bool scanNewline( bool stdOut, QByteArray *store );

    QByteArray* bufStdout();
    QByteArray* bufStderr();
    void consumeBufStdout( int consume );
    void consumeBufStderr( int consume );

private slots:
    void socketRead( int fd );
    void socketWrite( int fd );
    void timeout();
    void closeStdinLaunch();

private:
    QProcessPrivate *d;
#ifndef QT_NO_DIR
    QDir        workingDir;
#endif
    QStringList _arguments;

    int  exitStat; // exit status
    bool exitNormal; // normal exit?
    bool ioRedirection; // automatically set be (dis)connectNotify
    bool notifyOnExit; // automatically set be (dis)connectNotify
    bool wroteToStdinConnected; // automatically set be (dis)connectNotify

    bool readStdoutCalled;
    bool readStderrCalled;
    int comms;

    friend class QProcessPrivate;
#if defined(Q_OS_UNIX) || defined(_OS_UNIX) || defined(UNIX)
    friend class QProcessManager;
    friend class QProc;
#endif
};

#endif // QT_NO_PROCESS

#endif // QPROCESS_H
