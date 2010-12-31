/* example-client.h Qt3 D-Bus bindings client example
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

#if !defined(EXAMPLE_CLIENT_H)
#define EXAMPLE_CLIENT_H

// Qt includes
#include <qobject.h>

// forward declarations
class QDBusMessage;
class QDBusProxy;

class ExampleClient : public QObject
{
    Q_OBJECT
public:
    ExampleClient();
    virtual ~ExampleClient();

private slots:
    void slotConnect();

    void slotDBusSignal(const QDBusMessage& message);
    void slotAsyncReply(int callID, const QDBusMessage& reply);

private:
    QDBusProxy* m_proxy;
};

#endif

// End of file
