/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2012 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "technology.h"

// Qt DBUS includes
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusvariant.h>

#include <opie2/odebug.h>

TechnologyListener::TechnologyListener( const QDBusObjectPath &path, const QDBusDataMap<QString> &props ): QObject( 0, 0 )
{
    QDBusDataMap<QString>::ConstIterator it = props.begin();
    for (; it != props.end(); ++it) {
        if( it.key() == "Name" )
            m_name = it.data().toVariant().value.toString();
        else if( it.key() == "Type" )
            m_type = it.data().toVariant().value.toString();
        else if( it.key() == "Powered" )
            m_powered = it.data().toVariant().value.toBool();
    }

    QDBusConnection connection = QDBusConnection::systemBus();
    m_proxy = new QDBusProxy(this);
    m_proxy->setService("net.connman");
    m_proxy->setPath(path);
    m_proxy->setInterface("net.connman.Technology");
    m_proxy->setConnection(connection);

    QObject::connect(m_proxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                    this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
    QObject::connect(m_proxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));
}

TechnologyListener::~TechnologyListener()
{
}

void TechnologyListener::slotAsyncReply( int callId, const QDBusMessage& reply )
{
}

void TechnologyListener::slotDBusSignal( const QDBusMessage& message )
{
    if( message.member() == "PropertyChanged" ) {
        QString prop = message[0].toString();
        if( prop == "Powered" ) {
            m_powered = message[1].toVariant().value.toBool();
            odebug << m_name << " powered changed to " << m_powered << oendl;
        }
    }
}

QDBusProxy *TechnologyListener::proxy()
{
    return m_proxy;
}

bool TechnologyListener::isPowered()
{
    return m_powered;
}

void TechnologyListener::setPowered( bool powered )
{
    QValueList<QDBusData> params;
    params << QDBusData::fromString("Powered");
    params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(powered)));
    m_proxy->sendWithAsyncReply("SetProperty", params);
}

QString TechnologyListener::techType()
{
    return m_type;
}

QString TechnologyListener::techName()
{
    return m_name;
}
