/* qdbusobjectpath.cpp DBUS object path data type
 *
 * Copyright (C) 2007 Kevin Krammer <kevin.krammer@gmx.at>
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

#include "dbus/qdbusobjectpath.h"

QDBusObjectPath::QDBusObjectPath() : QCString()
{
}

QDBusObjectPath::QDBusObjectPath(const QDBusObjectPath& other) : QCString(other)
{
}

QDBusObjectPath::QDBusObjectPath(const QCString& other) : QCString(other)
{
}

QDBusObjectPath::QDBusObjectPath(const QDBusObjectPath& parentNode,
                                 const QCString& nodeName)
    : QCString(parentNode)
{
    if (parentNode.length() != 1) append("/");

    append(nodeName);
}

bool QDBusObjectPath::isValid() const
{
    return (validate(*this) == -1);
}

QDBusObjectPath QDBusObjectPath::parentNode() const
{
    if (length() == 1) return QDBusObjectPath();

    int index = findRev('/');

    if (index == -1) return QDBusObjectPath();

    if (index == 0) return left(1);

    return left(index);
}

int QDBusObjectPath::validate(const QCString& path)
{
    if (path.isEmpty()) return 0;

    if (path[0] != '/') return 0;

    // TODO add additional checks

    uint len = path.length();

    // only root node allowed to end in slash
    if (path[len - 1] == '/' && len > 1) return (len - 1);

    return -1;
}
