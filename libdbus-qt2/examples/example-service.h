/* example-service.h Qt2 D-Bus bindings service example
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

#if !defined(EXAMPLE_SERVICE_H)
#define EXAMPLE_SERVICE_H

// QDBUS includes
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusobject.h>

// forward declarations
class QStringList;

class ExampleService : public QDBusObjectBase
{
public:
    ExampleService(const QDBusConnection& connection);
    virtual ~ExampleService();

protected:
    virtual bool handleMethodCall(const QDBusMessage& message);

private:
    QDBusConnection m_connection;

private:
    QStringList sortStrings(const QStringList& list);
};

#endif

// End of file
