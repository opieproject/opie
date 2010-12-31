/* qdbusintegrator.cpp QDBusConnection private implementation
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

#include <qapplication.h>
#include <qevent.h>
#include <qmetaobject.h>
#include <qsocketnotifier.h>
#include <qtimer.h>

#include "qdbusconnection_p.h"
#include "dbus/qdbusmessage.h"

Atomic::Atomic(int value) : m_value(value)
{
}

void Atomic::ref()
{
    m_value++;
}

bool Atomic::deref()
{
    m_value--;
    return m_value > 0;
}

int QDBusConnectionPrivate::messageMetaType = 0;

static dbus_bool_t qDBusAddTimeout(DBusTimeout *timeout, void *data)
{
    Q_ASSERT(timeout);
    Q_ASSERT(data);

  //  qDebug("addTimeout %d", dbus_timeout_get_interval(timeout));

    QDBusConnectionPrivate *d = static_cast<QDBusConnectionPrivate *>(data);

    if (!dbus_timeout_get_enabled(timeout))
        return true;

    if (!qApp) {
        d->pendingTimeouts.append(timeout);
        return true;
    }
    int timerId = d->startTimer(dbus_timeout_get_interval(timeout));
    if (!timerId)
        return false;

    d->timeouts[timerId] = timeout;
    return true;
}

static void qDBusRemoveTimeout(DBusTimeout *timeout, void *data)
{
    Q_ASSERT(timeout);
    Q_ASSERT(data);

  //  qDebug("removeTimeout");

    QDBusConnectionPrivate *d = static_cast<QDBusConnectionPrivate *>(data);
    for (QValueList<DBusTimeout*>::iterator it = d->pendingTimeouts.begin();
         it != d->pendingTimeouts.end();) {
        if ((*it) == timeout) {
            it = d->pendingTimeouts.erase(it);
        }
      else
        ++it;
    }

    QDBusConnectionPrivate::TimeoutHash::iterator it = d->timeouts.begin();
    while (it != d->timeouts.end()) {
        if (it.data() == timeout) {
            d->killTimer(it.key());
            QDBusConnectionPrivate::TimeoutHash::iterator copyIt = it;
            ++it;
            d->timeouts.erase(copyIt);
        } else {
            ++it;
        }
    }
}

static void qDBusToggleTimeout(DBusTimeout *timeout, void *data)
{
    Q_ASSERT(timeout);
    Q_ASSERT(data);

    //qDebug("ToggleTimeout");

    qDBusRemoveTimeout(timeout, data);
    qDBusAddTimeout(timeout, data);
}

static dbus_bool_t qDBusAddWatch(DBusWatch *watch, void *data)
{
    Q_ASSERT(watch);
    Q_ASSERT(data);

    QDBusConnectionPrivate *d = static_cast<QDBusConnectionPrivate *>(data);

    int flags = dbus_watch_get_flags(watch);
    int fd = dbus_watch_get_unix_fd(watch);

    QDBusConnectionPrivate::Watcher watcher;
    if (flags & DBUS_WATCH_READABLE) {
        bool enabled = dbus_watch_get_enabled(watch);
        //qDebug("addReadWatch %d %s", fd, (enabled ? "enabled" : "disabled"));
        watcher.watch = watch;
        if (qApp) {
            watcher.read = new QSocketNotifier(fd, QSocketNotifier::Read, d);
            if (!enabled) watcher.read->setEnabled(false);
            d->connect(watcher.read, SIGNAL(activated(int)), SLOT(socketRead(int)));
        }
    }
    if (flags & DBUS_WATCH_WRITABLE) {
        bool enabled = dbus_watch_get_enabled(watch);
        //qDebug("addWriteWatch %d %s", fd, (enabled ? "enabled" : "disabled"));
        watcher.watch = watch;
        if (qApp) {
            watcher.write = new QSocketNotifier(fd, QSocketNotifier::Write, d);
            if (!enabled) watcher.write->setEnabled(false);
            d->connect(watcher.write, SIGNAL(activated(int)), SLOT(socketWrite(int)));
        }
    }
    // FIXME-QT4 d->watchers.insertMulti(fd, watcher);
    QDBusConnectionPrivate::WatcherHash::iterator it = d->watchers.find(fd);
    if (it == d->watchers.end())
    {
        it = d->watchers.insert(fd, QDBusConnectionPrivate::WatcherList());
    }
    it.data().append(watcher);

    return true;
}

static void qDBusRemoveWatch(DBusWatch *watch, void *data)
{
    Q_ASSERT(watch);
    Q_ASSERT(data);

    //qDebug("remove watch");

    QDBusConnectionPrivate *d = static_cast<QDBusConnectionPrivate *>(data);
    int fd = dbus_watch_get_unix_fd(watch);

    QDBusConnectionPrivate::WatcherHash::iterator it = d->watchers.find(fd);
    if (it != d->watchers.end())
    {
        QDBusConnectionPrivate::WatcherList& list = *it;
        for (QDBusConnectionPrivate::WatcherList::iterator wit = list.begin();
             wit != list.end(); ++wit)
        {
            if ((*wit).watch == watch)
            {
                // migth be called from a function triggered by a socket listener
                // so just disconnect them and schedule their delayed deletion.

                d->removedWatches.append(*wit);
                if ((*wit).read)
                {
                    (*wit).read->disconnect(d);
                    (*wit).read = 0;
                }
                if ((*wit).write)
                {
                    (*wit).write->disconnect(d);
                    (*wit).write = 0;
                }
                (*wit).watch = 0;
            }
        }
    }

    if (d->removedWatches.count() > 0)
        QTimer::singleShot(0, d, SLOT(purgeRemovedWatches()));
}

static void qDBusToggleWatch(DBusWatch *watch, void *data)
{
    Q_ASSERT(watch);
    Q_ASSERT(data);

    //qDebug("toggle watch");

    QDBusConnectionPrivate *d = static_cast<QDBusConnectionPrivate *>(data);
    int fd = dbus_watch_get_unix_fd(watch);

    QDBusConnectionPrivate::WatcherHash::iterator it = d->watchers.find(fd);
    if (it != d->watchers.end()) {
        QDBusConnectionPrivate::WatcherList& list = *it;
        for (QDBusConnectionPrivate::WatcherList::iterator wit = list.begin(); wit != list.end();
             ++wit)
        {
            if ((*wit).watch == watch) {
                bool enabled = dbus_watch_get_enabled(watch);
                int flags = dbus_watch_get_flags(watch);

//                 qDebug("toggle watch %d to %d (write: %d, read: %d)",
//                         dbus_watch_get_unix_fd(watch), enabled,
//                         flags & DBUS_WATCH_WRITABLE, flags & DBUS_WATCH_READABLE);

                if (flags & DBUS_WATCH_READABLE && (*wit).read)
                    (*wit).read->setEnabled(enabled);
                if (flags & DBUS_WATCH_WRITABLE && (*wit).write)
                    (*wit).write->setEnabled(enabled);
                return;
            }
        }
    }
}

static void qDBusNewConnection(DBusServer *server, DBusConnection *c, void *data)
{
    Q_ASSERT(data); Q_ASSERT(server); Q_ASSERT(c);

    qDebug("SERVER: GOT A NEW CONNECTION"); // TODO
}

static DBusHandlerResult qDBusSignalFilter(DBusConnection *connection,
                                           DBusMessage *message, void *data)
{
    Q_ASSERT(data);
    Q_UNUSED(connection);

    QDBusConnectionPrivate *d = static_cast<QDBusConnectionPrivate *>(data);
    if (d->mode == QDBusConnectionPrivate::InvalidMode)
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    int msgType = dbus_message_get_type(message);
    bool handled = false;

    //QDBusMessage amsg = QDBusMessage::fromDBusMessage(message);
    //qDebug() << "got message: " << dbus_message_get_type(message) << amsg;

    if (msgType == DBUS_MESSAGE_TYPE_SIGNAL) {
        handled = d->handleSignal(message);
    } else if (msgType == DBUS_MESSAGE_TYPE_METHOD_CALL) {
        handled = d->handleObjectCall(message);
    }

    return handled ? DBUS_HANDLER_RESULT_HANDLED :
            DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int QDBusConnectionPrivate::registerMessageMetaType()
{
    // FIXME-QT4 int tp = messageMetaType = qRegisterMetaType<QDBusMessage>("QDBusMessage");
    int tp = 0;
    return tp;
}

QDBusConnectionPrivate::QDBusConnectionPrivate(QObject *parent)
    : QObject(parent), ref(1), mode(InvalidMode), connection(0), server(0),
      dispatcher(0)
{
    static const int msgType = registerMessageMetaType();
    Q_UNUSED(msgType);

    dbus_error_init(&error);

    dispatcher = new QTimer(this);
    QObject::connect(dispatcher, SIGNAL(timeout()), this, SLOT(dispatch()));
}

QDBusConnectionPrivate::~QDBusConnectionPrivate()
{
    if (dbus_error_is_set(&error))
        dbus_error_free(&error);

    closeConnection();
}

void QDBusConnectionPrivate::closeConnection()
{
    ConnectionMode oldMode = mode;
    mode = InvalidMode; // prevent reentrancy
    if (oldMode == ServerMode) {
        if (server) {
            dbus_server_disconnect(server);
            dbus_server_unref(server);
            server = 0;
        }
    } else if (oldMode == ClientMode) {
        if (connection) {
            // closing shared connections is forbidden
#if 0
            dbus_connection_close(connection);
            // send the "close" message
            while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS);
#endif
            dbus_connection_unref(connection);
            connection = 0;
        }
    }
}

bool QDBusConnectionPrivate::handleError()
{
    lastError = QDBusError(&error);
    if (dbus_error_is_set(&error))
        dbus_error_free(&error);
    return lastError.isValid();
}

void QDBusConnectionPrivate::emitPendingCallReply(const QDBusMessage& message)
{
    emit dbusPendingCallReply(message);
}

void QDBusConnectionPrivate::bindToApplication()
{
    // Yay, now that we have an application we are in business
    // Re-add all watchers
    WatcherHash oldWatchers = watchers;
    watchers.clear();
    // FIXME-QT4 QHashIterator<int, QDBusConnectionPrivate::Watcher> it(oldWatchers);
    for (WatcherHash::const_iterator it = oldWatchers.begin(); it != oldWatchers.end(); ++it)
    {
        const WatcherList& list = *it;
        for (WatcherList::const_iterator wit = list.begin(); wit != list.end(); ++wit)
        {
            if (!(*wit).read && !(*wit).write) {
                qDBusAddWatch((*wit).watch, this);
            }
        }
    }

    // Re-add all timeouts
    while (!pendingTimeouts.isEmpty()) {
       qDBusAddTimeout(pendingTimeouts.first(), this);
       pendingTimeouts.pop_front();
    }
}

void QDBusConnectionPrivate::socketRead(int fd)
{
    // FIXME-QT4 QHashIterator<int, QDBusConnectionPrivate::Watcher> it(watchers);
    WatcherHash::const_iterator it = watchers.find(fd);
    if (it != watchers.end()) {
        const WatcherList& list = *it;
        for (WatcherList::const_iterator wit = list.begin(); wit != list.end(); ++wit) {
            if ((*wit).read && (*wit).read->isEnabled()) {
                if (!dbus_watch_handle((*wit).watch, DBUS_WATCH_READABLE))
                    qDebug("OUT OF MEM");
            }
        }
    }
    if (mode == ClientMode)
        scheduleDispatch();
}

void QDBusConnectionPrivate::socketWrite(int fd)
{
    // FIXME-QT4 QHashIterator<int, QDBusConnectionPrivate::Watcher> it(watchers);
    WatcherHash::const_iterator it = watchers.find(fd);
    if (it != watchers.end()) {
        const WatcherList& list = *it;
        for (WatcherList::const_iterator wit = list.begin(); wit != list.end(); ++wit) {
            if ((*wit).write && (*wit).write->isEnabled()) {
                if (!dbus_watch_handle((*wit).watch, DBUS_WATCH_WRITABLE))
                    qDebug("OUT OF MEM");
            }
        }
    }
}

void QDBusConnectionPrivate::objectDestroyed(QObject* object)
{
    //qDebug("Object destroyed");
    for (PendingCallMap::iterator it = pendingCalls.begin(); it != pendingCalls.end();)
    {
        QObject* receiver = (QObject*) it.data()->receiver;
        if (receiver == object || receiver == 0)
        {
            PendingCallMap::iterator copyIt = it;
            ++it;

            dbus_pending_call_cancel(copyIt.key());
            dbus_pending_call_unref(copyIt.key());
            delete copyIt.data();
            pendingCalls.erase(copyIt);
        }
        else
            ++it;
    }
}

void QDBusConnectionPrivate::purgeRemovedWatches()
{
    if (removedWatches.isEmpty()) return;

    WatcherList::iterator listIt = removedWatches.begin();
    for (; listIt != removedWatches.end(); ++listIt)
    {
        delete (*listIt).read;
        delete (*listIt).write;
    }
    removedWatches.clear();

    uint count = 0;
    WatcherHash::iterator it = watchers.begin();
    while (it != watchers.end())
    {
        WatcherList& list = *it;
        listIt = list.begin();
        while (listIt != list.end())
        {
            if (!((*listIt).read) && !((*listIt).write))
            {
                listIt = list.erase(listIt);
                ++count;
            }
        }

        if (list.isEmpty())
        {
            WatcherHash::iterator copyIt = it;
            ++it;
            watchers.erase(copyIt);
        }
        else
            ++it;
    }
}

void QDBusConnectionPrivate::scheduleDispatch()
{
    dispatcher->start(0);
}

void QDBusConnectionPrivate::dispatch()
{
    if (mode == ClientMode)
    {
        if (dbus_connection_dispatch(connection) != DBUS_DISPATCH_DATA_REMAINS)
        {
            // stop dispatch timer
            dispatcher->stop();
        }
    }
}

bool QDBusConnectionPrivate::handleObjectCall(DBusMessage *message)
{
    QDBusMessage msg = QDBusMessage::fromDBusMessage(message);

    ObjectMap::iterator it = registeredObjects.find(msg.path());
    if (it == registeredObjects.end())
        return false;

    return it.data()->handleMethodCall(msg);
}

bool QDBusConnectionPrivate::handleSignal(DBusMessage *message)
{
    QDBusMessage msg = QDBusMessage::fromDBusMessage(message);

    // yes, it is a single "|" below...
    // FIXME-QT4
    //return handleSignal(QString(), msg) | handleSignal(msg.path(), msg);

    dbusSignal(msg);
    return true;
}

static dbus_int32_t server_slot = -1;

void QDBusConnectionPrivate::setServer(DBusServer *s)
{
    if (!server) {
        handleError();
        return;
    }

    server = s;
    mode = ServerMode;

    dbus_server_allocate_data_slot(&server_slot);
    if (server_slot < 0)
        return;

    dbus_server_set_watch_functions(server, qDBusAddWatch, qDBusRemoveWatch,
                                    qDBusToggleWatch, this, 0); // ### check return type?
    dbus_server_set_timeout_functions(server, qDBusAddTimeout, qDBusRemoveTimeout,
                                      qDBusToggleTimeout, this, 0);
    dbus_server_set_new_connection_function(server, qDBusNewConnection, this, 0);

    dbus_server_set_data(server, server_slot, this, 0);
}

void QDBusConnectionPrivate::setConnection(DBusConnection *dbc)
{
    if (!dbc) {
        handleError();
        return;
    }

    connection = dbc;
    mode = ClientMode;

    dbus_connection_set_exit_on_disconnect(connection, false);
    dbus_connection_set_watch_functions(connection, qDBusAddWatch, qDBusRemoveWatch,
                                        qDBusToggleWatch, this, 0);
    dbus_connection_set_timeout_functions(connection, qDBusAddTimeout, qDBusRemoveTimeout,
                                          qDBusToggleTimeout, this, 0);
//    dbus_bus_add_match(connection, "type='signal',interface='com.trolltech.dbus.Signal'", &error);
//    dbus_bus_add_match(connection, "type='signal'", &error);

    dbus_bus_add_match(connection, "type='signal'", &error);
    if (handleError()) {
        closeConnection();
        return;
    }

    const char *service = dbus_bus_get_unique_name(connection);
    if (service) {
        QCString filter;
        filter += "destination='";
        filter += service;
        filter += "\'";

        dbus_bus_add_match(connection, filter.data(), &error);
        if (handleError()) {
            closeConnection();
            return;
        }
    } else {
        qWarning("QDBusConnectionPrivate::SetConnection: Unable to get unique name");
    }

    dbus_connection_add_filter(connection, qDBusSignalFilter, this, 0);

    //qDebug("unique name: %s", service);
}

static void qDBusResultReceived(DBusPendingCall *pending, void *user_data)
{
    //qDebug("Pending Call Result received");
    QDBusConnectionPrivate* d = reinterpret_cast<QDBusConnectionPrivate*>(user_data);
    QDBusConnectionPrivate::PendingCallMap::iterator it = d->pendingCalls.find(pending);

    DBusMessage *dbusReply = dbus_pending_call_steal_reply(pending);

    dbus_set_error_from_message(&d->error, dbusReply);
    d->handleError();

    if (it != d->pendingCalls.end())
    {
        QDBusMessage reply = QDBusMessage::fromDBusMessage(dbusReply);

        QObject::connect(d, SIGNAL(dbusPendingCallReply(const QDBusMessage&)),
                         it.data()->receiver, it.data()->method.data());

        d->emitPendingCallReply(reply);

        QObject::disconnect(d, SIGNAL(dbusPendingCallReply(const QDBusMessage&)),
                            it.data()->receiver, it.data()->method.data());
    }

    dbus_message_unref(dbusReply);
    dbus_pending_call_unref(pending);
    delete it.data();

    d->pendingCalls.erase(it);
}

int QDBusConnectionPrivate::sendWithReplyAsync(const QDBusMessage &message, QObject *receiver,
        const char *method)
{
    if (!receiver || !method)
        return 0;

    if (!QObject::connect(receiver, SIGNAL(destroyed(QObject*)),
                          this, SLOT(objectDestroyed(QObject*))))
        return false;

    DBusMessage *msg = message.toDBusMessage();
    if (!msg)
        return 0;

    DBusPendingCall *pending = 0;
    if (dbus_connection_send_with_reply(connection, msg, &pending, message.timeout())) {
        QDBusPendingCall *pcall = new QDBusPendingCall;
        pcall->receiver = receiver;
        pcall->method = method;
        pcall->pending = dbus_pending_call_ref(pending);
        pendingCalls.insert(pcall->pending, pcall);

        dbus_pending_call_set_notify(pending, qDBusResultReceived, this, 0);

        return dbus_message_get_serial(msg);
    }

    return 0;
}

void QDBusConnectionPrivate::flush()
{
    if (!connection) return;

    dbus_connection_flush(connection);
}
