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
#ifndef DESKTOP_H
#define DESKTOP_H

#include <qwidget.h>
#include <qdatetime.h>

//#include "qcopbridge.h"

class QCopBridge;
class QHostAddress;
class TransferServer;
class PackageHandler;
class ServiceRequest;
class TempScreenSaverMonitor;
class AppLauncher;
class AppLnkSet;
class StorageInfo;
class SyncDialog;
class DocumentList;
class ServerInterface;
namespace Opie {
    class ODeviceButton;
}

class Server : public QWidget {
    Q_OBJECT
public:
    Server();
    ~Server();

    static bool mkdir(const QString &path);

    void show();

    static bool setKeyboardLayout( const QString &kb );

public slots:
    void systemMsg(const QCString &, const QByteArray &);
    void receiveTaskBar(const QCString &msg, const QByteArray &data);
    void terminateServers();
    void pokeTimeMonitors();

private slots:
    void activate(const Opie::ODeviceButton*,bool);
    void syncConnectionClosed( const QHostAddress & );
    void applicationLaunched(int pid, const QString &app);
    void applicationTerminated(int pid, const QString &app);
    void applicationConnected(const QString &app);
    void storageChanged();
    void cancelSync();

protected:
    void styleChange( QStyle & );
    void timerEvent( QTimerEvent *e );

private:
    void layout();
    void startTransferServer();
    void preloadApps();

    QCopBridge *qcopBridge;
    TransferServer *transferServer;
    PackageHandler *packageHandler;
    QDate last_today_show;
    int tid_xfer;
    /* ### FIXME two below### */
//    int tid_today;
//    TempScreenSaverMonitor *tsmMonitor;
    StorageInfo *storage;
    SyncDialog *syncDialog;
    AppLauncher *appLauncher;
    DocumentList *docList;
    ServerInterface *serverGui;
};


#endif // DESKTOP_H

