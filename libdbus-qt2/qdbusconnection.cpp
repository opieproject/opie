/* qdbusconnection.cpp
 *
 * Copyright (C) 2005 Harald Fernengel <harry@kdevelop.org>
 * Copyright (C) 2005-2007 Kevin Krammer <kevin.krammer@gmx.at>
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

#include "dbus/qdbusconnection.h"
#include "dbus/qdbuserror.h"
#include "dbus/qdbusmessage.h"
#include "qdbusconnection_p.h"

QT_STATIC_CONST_IMPL char *QDBusConnection::default_connection_name = "qt_dbus_default_connection";

class QDBusConnectionManager
{
public:
    QDBusConnectionManager(): default_connection(0) {}
    ~QDBusConnectionManager();
    void bindToApplication();
    QDBusConnectionPrivate *connection(const QString &name) const;
    void removeConnection(const QString &name);
    void setConnection(const QString &name, QDBusConnectionPrivate *c);

    static QDBusConnectionManager* instance() {
        if (managerInstance == 0) managerInstance = new QDBusConnectionManager();
        return managerInstance;
    }

private:
    QDBusConnectionPrivate *default_connection;
    // FIXME-QT4 QHash<QString, QDBusConnectionPrivate *> connectionHash;
    typedef QMap<QString, QDBusConnectionPrivate*> ConnectionHash;
    ConnectionHash connectionHash;

    static QDBusConnectionManager* managerInstance;
};

// FIXME-QT4 Q_GLOBAL_STATIC(QDBusConnectionManager, manager);
QDBusConnectionManager* QDBusConnectionManager::managerInstance = 0;
QDBusConnectionManager* manager() {
    return QDBusConnectionManager::instance();
}

QDBusConnectionPrivate *QDBusConnectionManager::connection(const QString &name) const
{
    if (name == QString::fromLatin1(QDBusConnection::default_connection_name))
        return default_connection;

    ConnectionHash::const_iterator it = connectionHash.find(name);

    return (it != connectionHash.end() ? it.data() : 0);
}

void QDBusConnectionManager::removeConnection(const QString &name)
{
    QDBusConnectionPrivate *d = 0;
    if (name == QString::fromLatin1(QDBusConnection::default_connection_name)) {
        d = default_connection;
        default_connection = 0;
    } else {
        ConnectionHash::iterator it = connectionHash.find(name);
        if (it == connectionHash.end())
            return;

        d = it.data();
        connectionHash.erase(it);
    }
    if (!d->ref.deref())
        delete d;
}

QDBusConnectionManager::~QDBusConnectionManager()
{
    if (default_connection) {
        delete default_connection;
        default_connection = 0;
    }
/* FIXME-QT4
    for (QHash<QString, QDBusConnectionPrivate *>::const_iterator it = connectionHash.constBegin();
         it != connectionHash.constEnd(); ++it) {
             delete it.value();
    }*/
    for (ConnectionHash::const_iterator it = connectionHash.constBegin();
         it != connectionHash.constEnd(); ++it)
    {
        delete it.data();
    }
    connectionHash.clear();
}

void QDBusConnectionManager::bindToApplication()
{
    if (default_connection) {
        default_connection->bindToApplication();
    }
/* FIXME-QT4
    for (QHash<QString, QDBusConnectionPrivate *>::const_iterator it = connectionHash.constBegin();
         it != connectionHash.constEnd(); ++it) {
             (*it)->bindToApplication();
    }*/
    for (ConnectionHash::const_iterator it = connectionHash.constBegin();
         it != connectionHash.constEnd(); ++it)
    {
        it.data()->bindToApplication();
    }
}

void qDBusBindToApplication()
{
    manager()->bindToApplication();
}

void QDBusConnectionManager::setConnection(const QString &name, QDBusConnectionPrivate *c)
{
    if (name == QString::fromLatin1(QDBusConnection::default_connection_name))
        default_connection = c;
    else
        connectionHash[name] = c;
}


QDBusConnection::QDBusConnection() : d(0)
{
}

QDBusConnection::QDBusConnection(const QString &name)
{
    d = manager()->connection(name);
    if (d)
        d->ref.ref();
}

QDBusConnection::QDBusConnection(const QDBusConnection &other)
{
    d = other.d;
    if (d)
        d->ref.ref();
}

QDBusConnection::~QDBusConnection()
{
    if (d && !d->ref.deref())
        delete d;
}

QDBusConnection &QDBusConnection::operator=(const QDBusConnection &other)
{
    if (other.d)
        other.d->ref.ref();
/* FIXME-QT4
    QDBusConnectionPrivate *old = static_cast<QDBusConnectionPrivate *>(
            q_atomic_set_ptr(&d, other.d));*/
    QDBusConnectionPrivate* old = d;
    d = other.d;
    if (old && !old->ref.deref())
        delete old;

    return *this;
}

QDBusConnection QDBusConnection::sessionBus()
{
    return addConnection(QDBusConnection::SessionBus);
}

QDBusConnection QDBusConnection::systemBus()
{
    return addConnection(QDBusConnection::SystemBus);
}

QDBusConnection QDBusConnection::addConnection(BusType type, const QString &name)
{
//    Q_ASSERT_X(QCoreApplication::instance(), "QDBusConnection::addConnection",
//               "Cannot create connection without a Q[Core]Application instance");

    QDBusConnectionPrivate *d = manager()->connection(name);
    if (d)
        return QDBusConnection(name);

    d = new QDBusConnectionPrivate;
    DBusConnection *c = 0;
    switch (type) {
        case SystemBus:
            c = dbus_bus_get(DBUS_BUS_SYSTEM, &d->error);
            break;
        case SessionBus:
            c = dbus_bus_get(DBUS_BUS_SESSION, &d->error);
            break;
        case ActivationBus:
            c = dbus_bus_get(DBUS_BUS_STARTER, &d->error);
            break;
    }
    d->setConnection(c); //setConnection does the error handling for us

    manager()->setConnection(name, d);

    return QDBusConnection(name);
}

QDBusConnection QDBusConnection::addConnection(const QString &address,
                    const QString &name)
{
//    Q_ASSERT_X(QCoreApplication::instance(), "QDBusConnection::addConnection",
//               "Cannot create connection without a Q[Core]Application instance");

    QDBusConnectionPrivate *d = manager()->connection(name);
    if (d)
        return QDBusConnection(name);

    d = new QDBusConnectionPrivate;
    // setConnection does the error handling for us
    d->setConnection(dbus_connection_open(address.utf8().data(), &d->error));

    manager()->setConnection(name, d);

    return QDBusConnection(name);
}

void QDBusConnection::closeConnection(const QString &name)
{
    manager()->removeConnection(name);
}

void QDBusConnectionPrivate::timerEvent(QTimerEvent *e)
{
    DBusTimeout *timeout = timeouts[e->timerId()];
    dbus_timeout_handle(timeout);
}

bool QDBusConnection::send(const QDBusMessage &message) const
{
    if (!d || !d->connection)
        return false;

    DBusMessage *msg = message.toDBusMessage();
    if (!msg)
        return false;

    bool isOk = dbus_connection_send(d->connection, msg, 0);
    dbus_message_unref(msg);
    return isOk;
}

int QDBusConnection::sendWithAsyncReply(const QDBusMessage &message, QObject *receiver,
        const char *method) const
{
    if (!d || !d->connection)
        return 0;

    return d->sendWithReplyAsync(message, receiver, method);
}

QDBusMessage QDBusConnection::sendWithReply(const QDBusMessage &message, QDBusError *error) const
{
    if (!d || !d->connection)
        return QDBusMessage::fromDBusMessage(0);

    DBusMessage *msg = message.toDBusMessage();
    if (!msg)
        return QDBusMessage::fromDBusMessage(0);
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(d->connection, msg,
                                                -1, &d->error);
    if (d->handleError() && error)
        *error = d->lastError;

    dbus_message_unref(msg);

    return QDBusMessage::fromDBusMessage(reply);
}

void QDBusConnection::flush() const
{
    if (!d || !d->connection) return;

    d->flush();
}

void QDBusConnection::dispatch() const
{
    if (!d || !d->connection) return;

    d->dispatch();
}

void QDBusConnection::scheduleDispatch() const
{
    if (!d || !d->connection) return;

    d->scheduleDispatch();
}

bool QDBusConnection::connect(QObject* object, const char* slot)
{
    if (!d || !d->connection || !object || !slot)
        return false;

    bool ok = object->connect(d, SIGNAL(dbusSignal(const QDBusMessage&)), slot);

    return ok;
}

bool QDBusConnection::disconnect(QObject* object, const char* slot)
{
    if (!d || !d->connection || !object || !slot)
        return false;

    bool ok = d->disconnect(object, slot);

    return ok;
}

bool QDBusConnection::registerObject(const QString& path, QDBusObjectBase* object)
{
    if (!d || !d->connection || !object || path.isEmpty())
        return false;

    QDBusConnectionPrivate::ObjectMap::const_iterator it = d->registeredObjects.find(path);
    if (it != d->registeredObjects.end())
        return false;

    d->registeredObjects.insert(path, object);

    return true;
}

void QDBusConnection::unregisterObject(const QString &path)
{
    if (!d || !d->connection || path.isEmpty())
        return;

    QDBusConnectionPrivate::ObjectMap::iterator it = d->registeredObjects.find(path);
    if (it == d->registeredObjects.end())
        return ;

    d->registeredObjects.erase(it);
}

bool QDBusConnection::isConnected( ) const
{
    return d && d->connection && dbus_connection_get_is_connected(d->connection);
}

QDBusError QDBusConnection::lastError() const
{
    return d ? d->lastError : QDBusError();
}

QString QDBusConnection::uniqueName() const
{
    return d && d->connection ?
            QString::fromUtf8(dbus_bus_get_unique_name(d->connection))
            : QString();
}

bool QDBusConnection::requestName(const QString &name, int modeFlags)
{
    Q_ASSERT(modeFlags >= 0);

    if (!d || !d->connection)
        return false;

    if (modeFlags < 0)
        return false;

    int dbusFlags = 0;
    if (modeFlags & AllowReplace)
        dbusFlags |= DBUS_NAME_FLAG_ALLOW_REPLACEMENT;
    if (modeFlags & ReplaceExisting)
        dbusFlags |= DBUS_NAME_FLAG_REPLACE_EXISTING;

    dbus_bus_request_name(d->connection, name.utf8(), dbusFlags, &d->error);

    return !d->handleError();
}

#include "qdbusconnection.moc"
