/* qdbusproxy.cpp DBUS Object proxy
 *
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

#include "dbus/qdbuserror.h"
#include "dbus/qdbusconnection.h"
#include "dbus/qdbusmessage.h"
#include "dbus/qdbusproxy.h"

class QDBusProxy::Private
{
public:
    Private() : canSend(false) {}
    ~Private() {}

    void checkCanSend()
    {
        canSend = !path.isEmpty() && !service.isEmpty() && !interface.isEmpty();
    }

public:
    QDBusConnection connection;

    QString service;
    QString path;
    QString interface;
    bool canSend;

    QDBusError error;
};

QDBusProxy::QDBusProxy(QObject* parent, const char* name)
    : QObject(parent, (name ? name : "QDBusProxy")),
      d(new Private())
{
}

QDBusProxy::QDBusProxy(const QDBusConnection& connection,
                       QObject* parent, const char* name)
    : QObject(parent, (name ? name : "QDBusProxy")),
      d(new Private())
{
    setConnection(connection);
}

QDBusProxy::QDBusProxy(const QString& service, const QString& path,
                       const QString& interface, const QDBusConnection& connection,
                       QObject* parent, const char* name)
    : QObject(parent, (name ? name : "QDBusProxy")),
      d(new Private())
{
    setConnection(connection);

    d->service = service;
    d->path = path;
    d->interface = interface;
    d->checkCanSend();
}

QDBusProxy::~QDBusProxy()
{
    delete d;
}

bool QDBusProxy::setConnection(const QDBusConnection& connection)
{
    d->connection.disconnect(this, SLOT(handleDBusSignal(const QDBusMessage&)));

    d->connection = connection;

    return d->connection.connect(this, SLOT(handleDBusSignal(const QDBusMessage&)));
}

const QDBusConnection& QDBusProxy::connection() const
{
    return d->connection;
}

void QDBusProxy::setService(const QString& service)
{
    d->service = service;
    d->checkCanSend();
}

QString QDBusProxy::service() const
{
    return d->service;
}

void QDBusProxy::setPath(const QString& path)
{
    d->path = path;
    d->checkCanSend();
}

QString QDBusProxy::path() const
{
    return d->path;
}

void QDBusProxy::setInterface(const QString& interface)
{
    d->interface = interface;
    d->checkCanSend();
}

QString QDBusProxy::interface() const
{
    return d->interface;
}

bool QDBusProxy::canSend() const
{
    return d->canSend && d->connection.isConnected();
}

bool QDBusProxy::send(const QString& method, const QValueList<QDBusData>& params) const
{
    if (!d->canSend || method.isEmpty() || !d->connection.isConnected())
        return false;

    QDBusMessage message = QDBusMessage::methodCall(d->service, d->path,
                                                    d->interface, method);
    message += params;

    return d->connection.send(message);
}

QDBusMessage QDBusProxy::sendWithReply(const QString& method,
                                       const QValueList<QDBusData>& params,
                                       QDBusError* error) const
{
    if (!d->canSend || method.isEmpty() || !d->connection.isConnected())
        return QDBusMessage();

    QDBusMessage message = QDBusMessage::methodCall(d->service, d->path,
                                                    d->interface, method);
    message += params;

    QDBusMessage reply = d->connection.sendWithReply(message, &d->error);

    if (error)
        *error = d->error;

    return reply;
}

int QDBusProxy::sendWithAsyncReply(const QString& method, const QValueList<QDBusData>& params)
{
    if (!d->canSend || method.isEmpty() || !d->connection.isConnected())
        return 0;

    QDBusMessage message = QDBusMessage::methodCall(d->service, d->path,
                                                    d->interface, method);
    message += params;

    return d->connection.sendWithAsyncReply(message, this,
                   SLOT(handleAsyncReply(const QDBusMessage&)));
}

QDBusError QDBusProxy::lastError() const
{
    return d->error;
}

void QDBusProxy::handleDBusSignal(const QDBusMessage& message)
{
    if (!d->path.isEmpty() && d->path != message.path())
        return;

    // only filter by service name if the name is a unique name
    // because signals are always coming from a connection's unique name
    // and filtering by a generic name would reject all signals
    if (d->service.startsWith(":") && d->service != message.sender())
        return;

    if (!d->interface.isEmpty() && d->interface != message.interface())
        return;

    emit dbusSignal(message);
}

void QDBusProxy::handleAsyncReply(const QDBusMessage& message)
{
    d->error = message.error();

    emit asyncReply(message.replySerialNumber(), message);
}

#include "qdbusproxy.moc"
