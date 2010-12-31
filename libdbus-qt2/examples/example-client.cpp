/* example-client.cpp Qt3 D-Bus bindings client example
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

// Qt includes
#include <qapplication.h>
#include <qtimer.h>

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>

// local includes
#include "example-client.h"

ExampleClient::ExampleClient() : QObject(0, "ExampleClient"), m_proxy(0)
{
    // schedule connection to time when eventloop is operational
    QTimer::singleShot(0, this, SLOT(slotConnect()));
}

ExampleClient::~ExampleClient()
{
}

void ExampleClient::slotConnect()
{
    // get a connection to the session bus
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected())
    {
        qFatal("Cannot connect to session bus");
    }

    // create a proxy for the main bus object
    m_proxy = new QDBusProxy(this);
    m_proxy->setService("org.freedesktop.DBus");
    m_proxy->setPath("/org/freedesktop/DBus");
    m_proxy->setInterface("org.freedesktop.DBus");

    // let it use our connection
    m_proxy->setConnection(connection);

    // we want to be notified about D-BUS signals originating from the peer
    QObject::connect(m_proxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                     this, SLOT(slotDBusSignal(const QDBusMessage&)));

    // ask the bus for all service names
    qDebug("Calling method 'ListNames' synchronously. Our name: '%s'",
           connection.uniqueName().local8Bit().data());

    QDBusMessage reply = m_proxy->sendWithReply("ListNames", QValueList<QDBusData>());

    // we expect a method reply with a list of strings as its only value
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1)
    {
        if (reply[0].type() == QDBusData::List)
        {
            QDBusDataList list = reply[0].toList();
            if (list.type() != QDBusData::String)
            {
                qWarning("Reply for 'ListNames' carried unexpected value of"
                         " type %s-List", QDBusData::typeName(list.type()));
            }

            QStringList nameList = list.toQStringList();
            qDebug("Bus knows %d names:", nameList.count());

            QStringList::iterator it = nameList.begin();
            for (; it != nameList.end(); ++it)
            {
                qDebug("%s", (*it).local8Bit().data());
            }
        }
        else
            qWarning("Reply for 'ListNames' carried unexpected value of type %s",
                     reply[0].typeName());
    }
    else
    {
        qWarning("Method call 'ListNames' failed");

        QDBusError error = m_proxy->lastError();
        if (error.isValid())
        {
            qDebug("Error '%s' says:\n\"%s\"", error.name().local8Bit().data(),
                   error.message().local8Bit().data());
        }
    }
    qDebug("\n");

    // check for the Unix user ID our connection is registered with
    // do not block for the answer, perform an asynchronous call instead
    QObject::connect(m_proxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                     this, SLOT(slotAsyncReply(int, const QDBusMessage&)));

    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString(connection.uniqueName());

    int id = m_proxy->sendWithAsyncReply("GetConnectionUnixUser", parameters);
    if (id > 0)
    {
        qDebug("Calling method 'GetConnectionUnixUser' asynchronously. Call ID %d\n", id);
    }
    else
        qWarning("Method call 'GetConnectionUnixUser' failed");
}

void ExampleClient::slotDBusSignal(const QDBusMessage& message)
{
    qDebug("Received signal '%s' from peer: %d values",
           message.member().local8Bit().data(), message.count());

    for (QDBusMessage::const_iterator it = message.begin(); it != message.end(); ++it)
    {
        qDebug("Value of type %s: '%s'", (*it).typeName(), (*it).toString().local8Bit().data());
    }
    qDebug("\n");
}

void ExampleClient::slotAsyncReply(int callID, const QDBusMessage& reply)
{
    qDebug("Received asynchronous reply from peer. Call ID %d", callID);
    switch (reply.type())
    {
        case QDBusMessage::ErrorMessage:
        {
            qDebug("Reply is an error message");
            QDBusError error = reply.error();
            if (error.isValid())
            {
                qDebug("Error '%s' says:\n\"%s\"\n",
                       error.name().local8Bit().data(),
                       error.message().local8Bit().data());
            }
        }
        break;

        case QDBusMessage::ReplyMessage:
        {
            qDebug("Reply from peer: %d values", reply.count());

            for (QDBusMessage::const_iterator it = reply.begin(); it != reply.end(); ++it)
            {
                qDebug("Value of type %s: '%d'",
                       (*it).typeName(), (*it).toUInt32());
            }
            qDebug("\n");
        }
        break;

        default:
            break;
    }
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv, false);

    ExampleClient client;

    return app.exec();
}

// End of File
