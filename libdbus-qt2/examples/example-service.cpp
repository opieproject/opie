/* example-service.cpp Qt3 D-Bus bindings service example
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
#include <qstringlist.h>

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>

// local includes
#include "example-service.h"

ExampleService::ExampleService(const QDBusConnection& connection) : m_connection(connection)
{
    m_connection.registerObject("/ListSorter", this);

    qDebug("Object registered for path /ListSorter on unique name %s",
           m_connection.uniqueName().local8Bit().data());
}

ExampleService::~ExampleService()
{
    m_connection.unregisterObject("/ListSorter");
}

bool ExampleService::handleMethodCall(const QDBusMessage& message)
{
    qDebug("ExampleService::handleMethodCall: interface='%s', member='%s'",
           message.interface().latin1(), message.member().latin1());

    if (message.interface() != "org.example.Sort") return false;

    if (message.member() == "Strings")
    {
        // check parameters

        if (message.count() != 1 || message[0].type() != QDBusData::List)
        {
            // method signature not what we expected

            QDBusError error = QDBusError::stdInvalidArgs(
                    "Expected one argument of type array of string");

            QDBusMessage reply = QDBusMessage::methodError(message, error);

            // send error

            m_connection.send(reply);

            // tell D-BUS we did handle the call

            return true;
        }

        bool ok = false;
        QDBusDataList list = message[0].toList(&ok);
        if (!ok || list.count() < 1 || list.type() != QDBusData::String)
        {
            // method signature not what we expected

            QDBusError error = QDBusError::stdInvalidArgs(
                    "Expected one argument of type array of string");

            QDBusMessage reply = QDBusMessage::methodError(message, error);

            // send error

            m_connection.send(reply);

            // tell D-BUS we did handle the call

            return true;
        }

        // call implementation

        QStringList result = sortStrings(list.toQStringList());

        // prepare reply

        QDBusMessage reply = QDBusMessage::methodReply(message);

        list = result;

        // implicit conversion from QStringList to QDBusDataList through
        // QDBusDataList(const QStringList&)

        reply << QDBusData::fromList(list);

        // send reply

        m_connection.send(reply);

        // tell D-BUS we handled the call

        return true;
    }

    return false;
}

QStringList ExampleService::sortStrings(const QStringList& list)
{
    QStringList result = list;

    result.sort();

    return result;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv, false);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected())
        qFatal("Cannot connect to session bus");

    // try to get a specific service name
    if (!connection.requestName("org.example.SortService"))
    {
        qWarning("Requesting name 'org.example.SortService' failed. "
                 "Will only be addressable through unique name '%s'",
                 connection.uniqueName().local8Bit().data());
    }
    else
    {
        qDebug("Requesting name 'org.example.SortService' successfull");
    }

    ExampleService service(connection);

    return app.exec();
}

// End of File
