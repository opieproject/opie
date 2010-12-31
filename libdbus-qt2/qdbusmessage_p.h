/* qdbusmessage.h QDBusMessage private object
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

#ifndef QDBUSMESSAGE_P_H
#define QDBUSMESSAGE_P_H

#include <qstring.h>

#include "qdbusatomic.h"
#include "dbus/qdbuserror.h"

struct DBusMessage;

class QDBusMessagePrivate
{
public:
    QDBusMessagePrivate(QDBusMessage *qq);
    ~QDBusMessagePrivate();

    QString path, interface, member, service, sender;
    QDBusError error;
    DBusMessage *msg;
    DBusMessage *reply;
    QDBusMessage *q;
    int type;
    int timeout;
    // FIXME-QT4 QAtomic ref;
    Atomic ref;
};

#endif
