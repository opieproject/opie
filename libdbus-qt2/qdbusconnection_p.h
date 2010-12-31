/* qdbusconnection_p.h QDBusConnection private object
 *
 * Copyright (C) 2005 Harald Fernengel <harry@kdevelop.org>
 * Copyright (C) 2005 Kevin Krammer <kevin.krammer@gmx.at>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 */

//
//  W A R N I N G
//  -------------
//
// This file is not part of the public API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#ifndef QDBUSCONNECTION_P_H
#define QDBUSCONNECTION_P_H

#include <qguardedptr.h>
#include <qmap.h>
#include <qobject.h>
#include <qvaluelist.h>

#include <dbus/dbus.h>

#include "qdbusatomic.h"
#include "dbus/qdbuserror.h"
#include "dbus/qdbusobject.h"

class QDBusMessage;
class QSocketNotifier;
class QTimer;
class QTimerEvent;

struct DBusConnection;
struct DBusServer;

class QDBusConnectionPrivate: public QObject
{
    Q_OBJECT
public:
    QDBusConnectionPrivate(QObject *parent = 0);
    ~QDBusConnectionPrivate();

    void bindToApplication();

    void setConnection(DBusConnection *connection);
    void setServer(DBusServer *server);
    void closeConnection();
    void timerEvent(QTimerEvent *e);

    bool handleSignal(DBusMessage *msg);
    bool handleObjectCall(DBusMessage *message);
    bool handleError();

    void emitPendingCallReply(const QDBusMessage& message);

signals:
    void dbusSignal(const QDBusMessage& message);

    void dbusPendingCallReply(const QDBusMessage& message);

public slots:
    void socketRead(int);
    void socketWrite(int);

    void objectDestroyed(QObject* object);

    void purgeRemovedWatches();

    void scheduleDispatch();
    void dispatch();

public:
    DBusError error;
    QDBusError lastError;

    enum ConnectionMode { InvalidMode, ServerMode, ClientMode };

    // FIXME QAtomic ref;
    Atomic ref;
    ConnectionMode mode;
    DBusConnection *connection;
    DBusServer *server;

    QTimer* dispatcher;

    static int messageMetaType;
    static int registerMessageMetaType();
    int sendWithReplyAsync(const QDBusMessage &message, QObject *receiver,
                           const char *method);
    void flush();

    struct Watcher
    {
        Watcher(): watch(0), read(0), write(0) {}
        DBusWatch *watch;
        QSocketNotifier *read;
        QSocketNotifier *write;
    };
    // FIXME typedef QMultiHash<int, Watcher> WatcherHash;
    typedef QValueList<Watcher> WatcherList;
    WatcherList removedWatches;
    typedef QMap<int, WatcherList> WatcherHash;
    WatcherHash watchers;

    // FIXME typedef QHash<int, DBusTimeout *> TimeoutHash;
    typedef QMap<int, DBusTimeout*> TimeoutHash;
    TimeoutHash timeouts;

    typedef QMap<QString, QDBusObjectBase*> ObjectMap;
    ObjectMap registeredObjects;

    QValueList<DBusTimeout *> pendingTimeouts;

    struct QDBusPendingCall
    {
        QGuardedPtr<QObject> receiver;
        QCString method;
        DBusPendingCall *pending;
    };
    typedef QMap<DBusPendingCall*, QDBusPendingCall*> PendingCallMap;
    PendingCallMap pendingCalls;
};

#endif
