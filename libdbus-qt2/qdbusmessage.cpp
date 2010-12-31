/* qdbusmessage.cpp
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

#include "dbus/qdbusmessage.h"

#include <qstringlist.h>

#include <dbus/dbus.h>

#include "qdbusmarshall.h"
#include "qdbusmessage_p.h"

QDBusMessagePrivate::QDBusMessagePrivate(QDBusMessage *qq)
    : msg(0), reply(0), q(qq), type(DBUS_MESSAGE_TYPE_INVALID), timeout(-1), ref(1)
{
}

QDBusMessagePrivate::~QDBusMessagePrivate()
{
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);
}

///////////////


QDBusMessage QDBusMessage::signal(const QString &path, const QString &interface,
                                  const QString &member)
{
    QDBusMessage message;
    message.d->type = DBUS_MESSAGE_TYPE_SIGNAL;
    message.d->path = path;
    message.d->interface = interface;
    message.d->member = member;

    return message;
}

QDBusMessage QDBusMessage::methodCall(const QString &service, const QString &path,
                                      const QString &interface, const QString &method)
{
    QDBusMessage message;
    message.d->type = DBUS_MESSAGE_TYPE_METHOD_CALL;
    message.d->service = service;
    message.d->path = path;
    message.d->interface = interface;
    message.d->member = method;

    return message;
}

QDBusMessage QDBusMessage::methodReply(const QDBusMessage &other)
{
    Q_ASSERT(other.d->msg);

    QDBusMessage message;
    message.d->type = DBUS_MESSAGE_TYPE_METHOD_RETURN;
    message.d->reply = dbus_message_ref(other.d->msg);

    return message;
}

QDBusMessage QDBusMessage::methodError(const QDBusMessage &other, const QDBusError& error)
{
    Q_ASSERT(other.d->msg);

    QDBusMessage message;
    if (!error.isValid())
    {
        qWarning("QDBusMessage: error passed to methodError() is not valid!");
        return message;
    }

    message.d->type = DBUS_MESSAGE_TYPE_ERROR;
    message.d->reply = dbus_message_ref(other.d->msg);
    message.d->error = error;

    return message;
}

QDBusMessage::QDBusMessage()
{
    d = new QDBusMessagePrivate(this);
}

QDBusMessage::QDBusMessage(const QDBusMessage &other)
    : QValueList<QDBusData>(other)
{
    d = other.d;
    d->ref.ref();
}

QDBusMessage::~QDBusMessage()
{
    if (!d->ref.deref())
        delete d;
}

QDBusMessage &QDBusMessage::operator=(const QDBusMessage &other)
{
    QValueList<QDBusData>::operator=(other);
    // FIXME-QT4 qAtomicAssign(d, other.d);
    if (other.d) other.d->ref.ref();
    QDBusMessagePrivate* old = d;
    d = other.d;
    if (old && !old->ref.deref())
        delete old;
    return *this;
}

DBusMessage *QDBusMessage::toDBusMessage() const
{
    DBusMessage *msg = 0;
    switch (d->type) {
    case DBUS_MESSAGE_TYPE_METHOD_CALL:
        msg = dbus_message_new_method_call(d->service.utf8().data(),
                d->path.utf8().data(), d->interface.utf8().data(),
                d->member.utf8().data());
        break;
    case DBUS_MESSAGE_TYPE_SIGNAL:
        msg = dbus_message_new_signal(d->path.utf8().data(),
                d->interface.utf8().data(), d->member.utf8().data());
        break;
    case DBUS_MESSAGE_TYPE_METHOD_RETURN:
        msg = dbus_message_new_method_return(d->reply);
        break;
    case DBUS_MESSAGE_TYPE_ERROR:
        msg = dbus_message_new_error(d->reply, d->error.name().utf8().data(),
                d->error.message().utf8().data());
        break;
    }
    if (!msg)
        return 0;

    QDBusMarshall::listToMessage(*this, msg);
    return msg;
}

QDBusMessage QDBusMessage::fromDBusMessage(DBusMessage *dmsg)
{
    QDBusMessage message;
    if (!dmsg)
        return message;

    message.d->type = dbus_message_get_type(dmsg);
    message.d->path = QString::fromUtf8(dbus_message_get_path(dmsg));
    message.d->interface = QString::fromUtf8(dbus_message_get_interface(dmsg));
    message.d->member = QString::fromUtf8(dbus_message_get_member(dmsg));
    message.d->sender = QString::fromUtf8(dbus_message_get_sender(dmsg));
    message.d->msg = dbus_message_ref(dmsg);

    DBusError dbusError;
    dbus_error_init(&dbusError);
    if (dbus_set_error_from_message(&dbusError, dmsg))
    {
        message.d->error = QDBusError(&dbusError);
    }

    QDBusMarshall::messageToList(message, dmsg);

    return message;
}

QString QDBusMessage::path() const
{
    return d->path;
}

QString QDBusMessage::interface() const
{
    return d->interface;
}

QString QDBusMessage::member() const
{
    return d->member;
}

QString QDBusMessage::sender() const
{
    return d->sender;
}

QDBusError QDBusMessage::error() const
{
    return d->error;
}

int QDBusMessage::timeout() const
{
    return d->timeout;
}

void QDBusMessage::setTimeout(int ms)
{
    d->timeout = ms;
}

/*!
    Returns the unique serial number assigned to this message
    or 0 if the message was not sent yet.
 */
int QDBusMessage::serialNumber() const
{
    if (!d->msg)
        return 0;
    return dbus_message_get_serial(d->msg);
}

/*!
    Returns the unique serial number assigned to the message
    that triggered this reply message.

    If this message is not a reply to another message, 0
    is returned.

 */
int QDBusMessage::replySerialNumber() const
{
    if (!d->msg)
        return 0;
    return dbus_message_get_reply_serial(d->msg);
}

QDBusMessage::MessageType QDBusMessage::type() const
{
    switch (d->type) {
    case DBUS_MESSAGE_TYPE_METHOD_CALL:
        return MethodCallMessage;
    case DBUS_MESSAGE_TYPE_METHOD_RETURN:
        return ReplyMessage;
    case DBUS_MESSAGE_TYPE_ERROR:
        return ErrorMessage;
    case DBUS_MESSAGE_TYPE_SIGNAL:
        return SignalMessage;
    default:
        return InvalidMessage;
    }
}

