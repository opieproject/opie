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

#include "service.h"

// Qt DBUS includes
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusvariant.h>

#include <qtimer.h>
#include <opie2/odebug.h>

ServiceListener::ServiceListener( const QDBusObjectPath &path, const QDBusDataMap<QString> &props )
    : QObject( 0, 0 )
{
    m_strength = -1;
    m_secured = false;

    QDBusDataMap<QString>::ConstIterator it = props.begin();
    for (; it != props.end(); ++it) {
        if( it.key() == "Name" ) {
            m_serviceName = it.data().toVariant().value.toString();
        }
        else if( it.key() == "State" ) {
            m_state = it.data().toVariant().value.toString();
        }
        else if( it.key() == "Type" ) {
            m_type = it.data().toVariant().value.toString();
        }
        else if( it.key() == "Security" ) {
            QString security = QStringList(it.data().toVariant().value.toList().toStringList()).join(", ");
            if( security != "" && security != "none" )
                m_secured = true;
        }
        else if( it.key() == "Strength" ) {
            m_strength = it.data().toVariant().value.toByte();
        }
    }

    QDBusConnection connection = QDBusConnection::systemBus();
    m_proxy = new QDBusProxy(this);
    m_proxy->setService("net.connman");
    m_proxy->setPath(path);
    m_proxy->setInterface("net.connman.Service");
    m_proxy->setConnection(connection);

    QObject::connect(m_proxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                    this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
    QObject::connect(m_proxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));

    QTimer::singleShot( 0, this, SLOT( sendSignalStrength() ) );
}

ServiceListener::~ServiceListener()
{
}

void ServiceListener::sendSignalStrength()
{
    if( ( m_state == "ready" || m_state == "online" ) && m_strength != -1 )
        emit signalStrength( m_strength );
}

void ServiceListener::slotAsyncReply( int callId, const QDBusMessage& reply )
{
}

void ServiceListener::slotDBusSignal( const QDBusMessage& message )
{
    odebug << "ConnMan: " << message.member() << oendl;
    if( message.member() == "PropertyChanged" ) {
        QString prop = message[0].toString();
        if( prop == "State" ) {
            QString oldState = m_state;
            QString newState = message[1].toVariant().value.toString();
            if( oldState != newState ) {
                emit serviceStateChanged( QDBusObjectPath( QCString( m_proxy->path() )), oldState, newState );
                m_state = newState;
                sendSignalStrength();
            }
        }
        else if( prop == "Strength" ) {
            m_strength = message[1].toVariant().value.toByte();
            sendSignalStrength();
        }
    }
}

QDBusProxy *ServiceListener::proxy()
{
    return m_proxy;
}

QString ServiceListener::serviceName()
{
    return m_serviceName;
}

QString ServiceListener::serviceType()
{
    return m_type;
}

QString ServiceListener::state()
{
    return m_state;
}

bool ServiceListener::isSecured()
{
    return m_secured;
}
