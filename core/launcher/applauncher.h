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

#ifndef APP_LAUNCHER_H
#define APP_LAUNCHER_H

#include <qtopia/qpeglobal.h>
#include <qobject.h>
#include <qmap.h>
#ifdef Q_OS_WIN32
#include <qtopia/qprocess.h>
#include <qlist.h>
#endif

class QMessageBox;

class AppLauncher : public QObject
{
    Q_OBJECT
public:
    AppLauncher(QObject *parent = 0, const char *name = 0);
    ~AppLauncher();

    bool isRunning(const QString &app);

    static const int RAISE_TIMEOUT_MS;

signals:
    void launched(int pid, const QString &app);
    void terminated(int pid, const QString &app);
    void connected(const QString &app);
    
protected slots:
    void sigStopped(int sigPid, int sigStatus);
    void received(const QCString& msg, const QByteArray& data);
    void newQcopChannel(const QString& channel);
    void removedQcopChannel(const QString& channel);
    void createQuickLauncher();
    void processExited();

protected:
    bool event(QEvent *);
    void timerEvent( QTimerEvent * );

private:
    static void signalHandler(int sig);
    bool executeBuiltin(const QString &c, const QString &document);
    bool execute(const QString &c, const QString &document, bool noRaise = FALSE);
    void kill( int pid );
    int pidForName( const QString & );
    
private:    
    QMap<int,QString> runningApps;
    QMap<QString,int> waitingHeartbeat;
#ifdef Q_OS_WIN32
    QList<QProcess> runningAppsProc; 
#endif
    int qlPid;
    bool qlReady;
    QMessageBox *appKillerBox;
    QString appKillerName;
};

#endif

