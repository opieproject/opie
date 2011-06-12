/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/odebug.h>
#include <opie2/obluetooth.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Bluez;
using namespace Opie::Core;

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>
#include <dbus/qdbusdatamap.h>

#include <obluetoothdevicehandler.h>

DeviceHandler::DeviceHandler( OBluetoothDevice *dev, const QString &dbusInterface, DeviceHandlerPool *pool )
    : QObject( dev )
{
    QDBusConnection connection = QDBusConnection::systemBus();
    m_dev = dev;
    m_devProxy = new QDBusProxy(0);
    m_devProxy->setService( "org.bluez" );
    m_devProxy->setPath( m_dev->devicePath() );
    m_devProxy->setInterface( dbusInterface );
    m_devProxy->setConnection( connection );

    m_pool = pool;
    m_pool->addHandler( this );

    QObject::connect(m_devProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                     this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
}

DeviceHandler::~DeviceHandler()
{
    delete m_devProxy;
}

void DeviceHandler::sendAsync( const QString& method, const QValueList<QDBusData>& params )
{
    int callId = m_devProxy->sendWithAsyncReply( method, params );
    m_calls[callId] = method;
}

OBluetoothDevice *DeviceHandler::device()
{
    return m_dev;
}

QString DeviceHandler::dbusInterface()
{
    return m_devProxy->interface();
}

void DeviceHandler::slotAsyncReply( int callId, const QDBusMessage& reply )
{
    QString method = m_calls[callId];

    if (reply.type() == QDBusMessage::ReplyMessage) {
        odebug << "********* " << m_devProxy->interface() << " " << method << " success" << oendl;
        if( m_pool ) {
            if( reply.count() > 0 )
                m_pool->handlerSuccess( this, method, reply[0].toString() );
            else
                m_pool->handlerSuccess( this, method, QString::null );
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage) {
        odebug << "********* " << m_devProxy->interface() << " " << method << " fail: " << reply.error().name() << ": " << reply.error().message() << oendl;
        if( m_pool )
            m_pool->handlerError( this, method, reply.error().name(), reply.error().message() );
    }

    m_calls.remove(callId);
}

////////////////////////////////////////////////////////////////

DeviceHandlerPool::DeviceHandlerPool( QObject *parent )
    : QObject( parent )
{
    m_handlers.setAutoDelete(TRUE);
}

void DeviceHandlerPool::addHandler( DeviceHandler *handler )
{
    m_handlers.append( handler );
}

DeviceHandler *DeviceHandlerPool::getHandler( OBluetoothDevice *dev, const QString &dbusInterface )
{
    QListIterator<DeviceHandler> it( m_handlers );
    for ( ; it.current(); ++it ) {
        DeviceHandler *handler = it.current();
        if( handler->device() == dev && handler->dbusInterface() == dbusInterface )
            return handler;
    }

    if( dbusInterface == InputDeviceHandler::dbusInterface() )
        return new InputDeviceHandler( dev, this );
    else if( dbusInterface == SerialDeviceHandler::dbusInterface() )
        return new SerialDeviceHandler( dev, this );
    else if( dbusInterface == NetworkDeviceHandler::dbusInterface() )
        return new NetworkDeviceHandler( dev, this );

    return NULL;
}

void DeviceHandlerPool::handlerSuccess( DeviceHandler *handler, const QString &method, const QString &retval )
{
    emit success( handler, method, retval );
}

void DeviceHandlerPool::handlerError( DeviceHandler *handler, const QString &method, const QString &error, const QString &message )
{
    emit failure( handler, method, error, message );
}

////////////////////////////////////////////////////////////////

InputDeviceHandler::InputDeviceHandler( OBluetoothDevice *dev, DeviceHandlerPool *pool )
    : DeviceHandler(dev, "org.bluez.Input", pool)
{
    odebug << "InputDeviceHandler ctor" << oendl;
}

QString InputDeviceHandler::dbusInterface()
{
    return "org.bluez.Input";
}

void InputDeviceHandler::connect()
{
    sendAsync("Connect", QValueList<QDBusData>());
}

void InputDeviceHandler::disconnect()
{
    sendAsync("Disconnect", QValueList<QDBusData>());
}

////////////////////////////////////////////////////////////////

SerialDeviceHandler::SerialDeviceHandler( OBluetoothDevice *dev, DeviceHandlerPool *pool )
    : DeviceHandler(dev, "org.bluez.Serial", pool)
{
}

QString SerialDeviceHandler::dbusInterface()
{
    return "org.bluez.Serial";
}

void SerialDeviceHandler::connect( const QString &pattern )
{
    QValueList<QDBusData> params;
    params << QDBusData::fromString( pattern );
    sendAsync("Connect", params);
}

void SerialDeviceHandler::disconnect()
{
    sendAsync("Disconnect", QValueList<QDBusData>());
}

////////////////////////////////////////////////////////////////

NetworkDeviceHandler::NetworkDeviceHandler( OBluetoothDevice *dev, DeviceHandlerPool *pool )
    : DeviceHandler(dev, "org.bluez.Network", pool)
{
}

QString NetworkDeviceHandler::dbusInterface()
{
    return "org.bluez.Network";
}

void NetworkDeviceHandler::connect( const QString &pattern )
{
    QValueList<QDBusData> params;
    params << QDBusData::fromString( pattern );
    sendAsync("Connect", params);
}

void NetworkDeviceHandler::disconnect()
{
    sendAsync("Disconnect", QValueList<QDBusData>());
}

////////////////////////////////////////////////////////////////
