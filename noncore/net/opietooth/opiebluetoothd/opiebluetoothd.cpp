/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
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

#include "opiebluetoothd.h"
#include "bluetoothagent.h"

/* OPIE */
#include <opie2/odebug.h>

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>
#include <dbus/qdbusdatamap.h>

/* QT */
#include <qapplication.h>
#include <qtimer.h>
#include <qpe/qcopenvelope_qws.h>

/* STD */
#include <stdlib.h>

using namespace Opie::Core;

OBluetoothDaemon::OBluetoothDaemon()
{
    QCopChannel* chan = new QCopChannel("QPE/Bluetooth", this );
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(slotMessage(const QCString&,const QByteArray&) ) );

    m_bluezAdapterProxy = NULL;
    m_agent = NULL;
    m_tempEnabled = false;
    m_receiveEnabled = false;
    m_propCallID = -1;
    QTimer::singleShot(0, this, SLOT(slotDBusConnect()));

    odebug << "opiebluetoothd started" << oendl;
}

OBluetoothDaemon::~OBluetoothDaemon()
{
    odebug << "opiebluetoothd ended" << oendl;
    delete m_agent;
    delete m_bluezAdapterProxy;
}

void OBluetoothDaemon::slotDBusConnect()
{
    // get a connection to the session bus
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected())
    {
        oerr << "Cannot connect to session bus" << oendl;
        return;
    }

    // create a proxy for the bluez object
    m_bluezManagerProxy = new QDBusProxy(this);
    m_bluezManagerProxy->setService("org.bluez");
    m_bluezManagerProxy->setPath("/");
    m_bluezManagerProxy->setInterface("org.bluez.Manager");

    m_bluezManagerProxy->setConnection(connection);
    QObject::connect(m_bluezManagerProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));

    QObject::connect(m_bluezManagerProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                    this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
    odebug << "bluez connected" << oendl;

    QDBusMessage reply = m_bluezManagerProxy->sendWithReply("DefaultAdapter", QValueList<QDBusData>());
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1)
    {
        m_adapterPath = reply[0].toObjectPath();
        slotAdapterChange();
    }
}

void OBluetoothDaemon::slotAdapterChange()
{
    QDBusConnection connection = QDBusConnection::systemBus();

    if( m_bluezAdapterProxy ) {
        delete m_bluezAdapterProxy;
        delete m_agent;
        m_bluezAdapterProxy = NULL;
        m_agent = NULL;
    }

    if( m_adapterPath != "" ) {
        m_bluezAdapterProxy = new QDBusProxy(this);
        m_bluezAdapterProxy->setService("org.bluez");
        m_bluezAdapterProxy->setPath(m_adapterPath);
        m_bluezAdapterProxy->setInterface("org.bluez.Adapter");

        m_bluezAdapterProxy->setConnection(connection);
        QObject::connect(m_bluezAdapterProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                        this, SLOT(slotDBusSignal(const QDBusMessage&)));

        QObject::connect(m_bluezAdapterProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                        this, SLOT(slotAsyncReply(int, const QDBusMessage&)));

        odebug << "adapter " << m_adapterPath << " connected" << oendl;

        // Reset state variables
        m_tempEnabled = false;
        m_propCallID = m_bluezAdapterProxy->sendWithAsyncReply( "GetProperties", QValueList<QDBusData>() );

        m_agent = new OBluetoothAgent(connection, m_bluezAdapterProxy);
    }
    else {
        sendStatus(true);
        odebug << "adapter disconnected" << oendl;
    }
}

void OBluetoothDaemon::slotDBusSignal(const QDBusMessage& message)
{
    odebug << "Received signal '" << message.member() << "' from peer: " << message.count() << " values" << oendl;
    for (QDBusMessage::ConstIterator it = message.begin(); it != message.end(); ++it)
    {
        odebug << " value of type " << (*it).typeName() << ": '" << dataToString(*it) << "'" << oendl;
    }

    if( message.member() == "DefaultAdapterChanged" ) {
        m_adapterPath = message[0].toObjectPath();
        QTimer::singleShot(0, this, SLOT(slotAdapterChange()));
    }
    else if( message.member() == "AdapterRemoved" ) {
        if( m_adapterPath == QString(message[0].toObjectPath()) ) {
            m_adapterPath = "";
            QTimer::singleShot(0, this, SLOT(slotAdapterChange()));
        }
    }
    else if( message.member() == "PropertyChanged" ) {
        if( message[0].toString() == "Powered" ) {
            m_powered = message[1].toVariant().value.toBool();
            odebug << "chg powered is " << m_powered << oendl;
            sendStatus(true);
        }
        else if( message[0].toString() == "Discoverable" ) {
            m_discoverEnabled = message[1].toVariant().value.toBool();
            sendStatus(true);
        }
    }
}

void OBluetoothDaemon::slotAsyncReply(int callID, const QDBusMessage& reply)
{
    odebug << "Received asynchronous reply from peer. Call ID " << callID << oendl;
    switch (reply.type())
    {
        case QDBusMessage::ErrorMessage:
        {
            odebug << "Reply is an error message" << oendl;
            QDBusError error = reply.error();
            if (error.isValid())
            {
                owarn << "Error '" << error.name() << "' says:\n\"" << error.message() << "\"" << oendl;
            }
        }
        break;

        case QDBusMessage::ReplyMessage:
        {
/*            if (reply[0].type() == QDBusData::List)
            {
                QDBusDataList list = reply[0].toList();
                if (list.type() != QDBusData::String)
                {
                    owarn << "Reply for 'GetProperties' carried unexpected value of"
                            " type " << QDBusData::typeName(list.type()) << "-List" << oendl;
                }

                QStringList nameList = list.toQStringList();
                odebug << "Bus knows " << nameList.count() << " names:";

                QStringList::Iterator it = nameList.begin();
                for (; it != nameList.end(); ++it)
                {
                    odebug << (*it) << oendl;
                }
            }
            else*/ if( callID == m_propCallID ) {
                if (reply[0].type() == QDBusData::Map) {
                    if( reply[0].keyType() == QDBusData::String ) {
                        const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
                        QDBusDataMap<QString>::ConstIterator it = map.begin();
                        for (; it != map.end(); ++it) {
                            if( it.key() == "Discoverable" )
                                m_discoverEnabled = it.data().toVariant().value.toBool();
                            else if( it.key() == "Powered" ) {
                                m_powered = it.data().toVariant().value.toBool();
                                odebug << "powered is " << m_powered << oendl;
                            }
                        }
                        sendStatus(true);
                    }
                }
            }
            else {
                odebug << "Reply from peer: " << reply.count() << " values" << oendl;

                for (QDBusMessage::ConstIterator it = reply.begin(); it != reply.end(); ++it)
                {
                    odebug << " value of type " << (*it).typeName() << ": '" << dataToString(*it) << "'" << oendl;
                }
            }
        }
        break;

        default:
            break;
    }
}

void OBluetoothDaemon::slotMessage( const QCString& msg, const QByteArray& data )
{
    if ( msg == "quitDaemon()" )
        qApp->quit();
    else if ( msg == "enableBluetooth()" ) {
        m_tempEnabled = false;
        startBluetooth();
    }
    else if ( msg == "disableBluetooth()" ) {
        m_tempEnabled = false;
        stopBluetooth();
    }
    else if ( msg == "enableBluetoothTemp()" ) {
        if( !m_powered ) {
            m_tempEnabled = true;
            startBluetooth();
        }
    }
    else if ( msg == "disableBluetoothTemp()" ) {
        if( m_tempEnabled )
            stopBluetooth();
    }
    else if ( msg == "sendStatus()" )
        sendStatus(false);
}

void OBluetoothDaemon::startBluetooth()
{
    if( m_bluezAdapterProxy ) {
        odebug << "starting bluetooth" << oendl;
        QValueList<QDBusData> parameters;
        parameters << QDBusData::fromString("Powered");
        parameters << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(true)));
        if( ! m_bluezAdapterProxy->sendWithAsyncReply( "SetProperty", parameters ) ) {
            odebug << "startBluetooth: SetProperty(Powered,true) failed" << oendl;
            QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
            e << tr("Failed to power on adapter");
        }
    }
    else {
        odebug << "startBluetooth: no adapter" << oendl;
        QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
        e << tr("No Bluetooth adapter available");
    }
}

void OBluetoothDaemon::stopBluetooth()
{
    if( m_bluezAdapterProxy ) {
        odebug << "stopping bluetooth" << oendl;
        QValueList<QDBusData> parameters;
        parameters << QDBusData::fromString("Powered");
        parameters << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(false)));
        if( ! m_bluezAdapterProxy->sendWithAsyncReply( "SetProperty", parameters ) ) {
            odebug << "stopBluetooth: SetProperty(Powered,false) failed" << oendl;
            QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
            e << tr("Failed to power off adapter");
        }
    }
    else {
        odebug << "stopBluetooth: no adapter" << oendl;
        QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
        e << tr("No Bluetooth adapter available");
    }
}

void OBluetoothDaemon::sendStatus(bool change)
{
    int status = 0;
    if( m_bluezAdapterProxy ) {
        status |= BT_STATUS_ADAPTER;
        if( m_powered ) {
            status |= BT_STATUS_ENABLED;
            if( m_tempEnabled )
                status |= BT_STATUS_ENABLED_TEMP;
            if( m_discoverEnabled )
                status |= BT_STATUS_DISCOVERABLE;
            if( m_receiveEnabled )
                status |= BT_STATUS_RECEIVE;
        }
    }

    QCString msg;
    if( change )
        msg = "statusChange(int)";
    else
        msg = "status(int)";
        
    QCopEnvelope e( "QPE/Bluetooth", msg );
    e << status;

    odebug << "sent " << msg << " " << status << oendl;
}

// mostly for debugging
QString OBluetoothDaemon::dataToString(const QDBusData &data)
{
    switch (data.type())
    {
        case QDBusData::Invalid:
            return QString::null;

        case QDBusData::Bool:
            if( data.toBool() )
                return "True";
            else
                return "False";

        case QDBusData::Byte: {
            QString ret;
            ret.setNum(data.toByte());
            return ret;
        }
        case QDBusData::Int16: {
            QString ret;
            ret.setNum(data.toInt16());
            return ret;
        }
        case QDBusData::UInt16: {
            QString ret;
            ret.setNum(data.toUInt16());
            return ret;
        }
        case QDBusData::Int32: {
            QString ret;
            ret.setNum(data.toInt32());
            return ret;
        }
        case QDBusData::UInt32: {
            QString ret;
            ret.setNum(data.toUInt32());
            return ret;
        }
        case QDBusData::Int64: {
            QString ret;
            ret.setNum(data.toInt64());
            return ret;
        }
        case QDBusData::UInt64: {
            QString ret;
            ret.setNum(data.toUInt64());
            return ret;
        }
        case QDBusData::Double: {
            QString ret;
            ret.setNum(data.toDouble());
            return ret;
        }
        case QDBusData::String:
            return data.toString();

        case QDBusData::ObjectPath:
            return QString(data.toObjectPath());

        case QDBusData::List: {
            QString ret;
            QValueList<QDBusData> list = data.toList().toQValueList();
            QValueList<QDBusData>::Iterator it = list.begin();
            for (; it != list.end(); ++it)
            {
                ret += dataToString(*it) + ", ";
            }
            return ret;
        }
        case QDBusData::Struct: {
            QString ret;
            QValueList<QDBusData> list = data.toStruct();
            QValueList<QDBusData>::Iterator it = list.begin();
            for (; it != list.end(); ++it)
            {
                ret += dataToString(*it) + ", ";
            }
            return ret;
        }
        case QDBusData::Variant:
            return QString("variant(" + dataToString(data.toVariant().value) + ")");

        case QDBusData::Map:
            QString ret;
            switch (data.keyType())
            {
                case QDBusData::Byte: {
                    const QDBusDataMap<Q_UINT8> map = data.toByteKeyMap();
                    QDBusDataMap<Q_UINT8>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::Int16: {
                    const QDBusDataMap<Q_INT16> map = data.toInt16KeyMap();
                    QDBusDataMap<Q_INT16>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::UInt16: {
                    const QDBusDataMap<Q_UINT16> map = data.toUInt16KeyMap();
                    QDBusDataMap<Q_UINT16>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::Int32: {
                    const QDBusDataMap<Q_INT32> map = data.toInt32KeyMap();
                    QDBusDataMap<Q_INT32>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::UInt32: {
                    const QDBusDataMap<Q_UINT32> map = data.toUInt32KeyMap();
                    QDBusDataMap<Q_UINT32>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::Int64: {
                    const QDBusDataMap<Q_INT64> map = data.toInt64KeyMap();
                    QDBusDataMap<Q_INT64>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::UInt64: {
                    const QDBusDataMap<Q_UINT64> map = data.toUInt64KeyMap();
                    QDBusDataMap<Q_UINT64>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::String: {
                    const QDBusDataMap<QString> map = data.toStringKeyMap();
                    QDBusDataMap<QString>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += it.key() + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                case QDBusData::ObjectPath: {
                    const QDBusDataMap<QDBusObjectPath> map = data.toObjectPathKeyMap();
                    QDBusDataMap<QDBusObjectPath>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it)
                        ret += QString(it.key()) + "=" + dataToString(it.data()) + "\n";
                    break;
                }
                default:
                    qFatal("dataToString unhandled map key type %d(%s)",
                            data.keyType(), QDBusData::typeName(data.keyType()));
            }
            return ret;
    }
}
