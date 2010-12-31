/* qdbusserver.cpp
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

#include "dbus/qdbusserver.h"
#include "qdbusconnection_p.h"

QDBusServer::QDBusServer(const QString &addr, QObject *parent)
    : QObject(parent)
{
    d = new QDBusConnectionPrivate(this);

    if (addr.isEmpty())
        return;

    d->setServer(dbus_server_listen(addr.utf8().data(), &d->error));
}

bool QDBusServer::isConnected() const
{
    return d->server && dbus_server_get_is_connected(d->server);
}

QDBusError QDBusServer::lastError() const
{
    return d->lastError;
}

QString QDBusServer::address() const
{
    QString addr;
    if (d->server) {
        char *c = dbus_server_get_address(d->server);
        addr = QString::fromUtf8(c);
        dbus_free(c);
    }

    return addr;
}

#include "qdbusserver.moc"
