/* qdbusmarshall.h QDBusMarshall object
 *
 * Copyright (C) 2005 Harald Fernengel <harry@kdevelop.org>
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

#ifndef QDBUSMARSHALL_H
#define QDBUSMARSHALL_H

struct DBusMessage;

class QDBusData;

template <typename T> class QValueList;

class QDBusMarshall
{
public:
    static void listToMessage(const QValueList<QDBusData> &list, DBusMessage* message);
    static void messageToList(QValueList<QDBusData>& list, DBusMessage* message);
};

#endif
