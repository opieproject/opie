/*
                             This file is part of the Opie Project
                             Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the license.
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

#include "obluetooth.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* Qt */
#include <qapplication.h>

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>
#include <dbus/qdbusdatamap.h>

namespace Opie {
namespace Bluez {

/*======================================================================================
 * OBluetooth
 *======================================================================================*/

OBluetooth* OBluetooth::_instance = 0;

OBluetooth::OBluetooth()
    : m_bluezManagerProxy(0), d( 0 )
{
    synchronize();
}

OBluetooth::~OBluetooth()
{
    delete m_bluezManagerProxy;
}

OBluetooth* OBluetooth::instance()
{
    if ( !_instance ) _instance = new OBluetooth();
    return _instance;
}

OBluetooth::InterfaceIterator OBluetooth::iterator() const
{
    return OBluetooth::InterfaceIterator( _interfaces );
}

int OBluetooth::count() const
{
    return _interfaces.count();
}

OBluetoothInterface* OBluetooth::interface( const QString& iface ) const
{
    return _interfaces[iface];
}

void OBluetooth::synchronize()
{
    odebug << "OBluetooth::synchronize() - gathering available HCI devices" << oendl;
    _interfaces.clear();

    // get a connection to the session bus
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        oerr << "Cannot connect to session bus" << oendl;
        return;
    }

    if( !m_bluezManagerProxy ) {
        // create a proxy for the bluez object
        QDBusConnection connection = QDBusConnection::systemBus();
        m_bluezManagerProxy = new QDBusProxy(this);
        m_bluezManagerProxy->setService("org.bluez");
        m_bluezManagerProxy->setPath("/");
        m_bluezManagerProxy->setInterface("org.bluez.Manager");
        m_bluezManagerProxy->setConnection(connection);
        //QObject::connect(m_bluezManagerProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
        //                this, SLOT(slotDBusSignal(const QDBusMessage&)));
        //QObject::connect(m_bluezManagerProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
        //                this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
    }

    QDBusMessage reply = m_bluezManagerProxy->sendWithReply("GetProperties", QValueList<QDBusData>());
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
        const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
        QDBusDataMap<QString>::ConstIterator it = map.begin();
        for (; it != map.end(); ++it) {
            if( it.key() == "Adapters" ) {
                // Get info for each adapter
                const QValueList<QDBusObjectPath> list = it.data().toVariant().value.toList().toObjectPathList();
                QValueList<QDBusObjectPath>::ConstIterator it2 = list.begin();
                for (; it2 != list.end(); ++it2) {
                    odebug << "OBluetooth::synchronize() - found device " << (*it2) << oendl;
                    OBluetoothInterface *iface  = new OBluetoothInterface( this, 0, (*it2) );
                    _interfaces.insert( iface->name(), iface );
                }
            }
        }
    }
}

/*======================================================================================
 * OBluetoothInterface
 *======================================================================================*/

class OBluetoothInterface::Private
{
  public:
    Private( const QDBusObjectPath &path )
    {
        discovering = false;
        QDBusConnection connection = QDBusConnection::systemBus();
        adapterProxy = new QDBusProxy(0);
        adapterProxy->setService("org.bluez");
        adapterProxy->setPath(path);
        adapterProxy->setInterface("org.bluez.Adapter");
        adapterProxy->setConnection(connection);
        reloadInfo();
    }

    ~Private() {
        delete adapterProxy;
    }

    void reloadInfo()
    {
        adapterName = "";
        adapterClass = 0;
        bdaddr = "";
        powered = false;
        QDBusMessage reply = adapterProxy->sendWithReply("GetProperties", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            const QDBusDataMap<QString> adapterProps = reply[0].toStringKeyMap();
            QDBusDataMap<QString>::ConstIterator it = adapterProps.begin();
            for (; it != adapterProps.end(); ++it) {
                if( it.key() == "Name" ) {
                    adapterName = it.data().toVariant().value.toString();
                }
                else if( it.key() == "Class" ) {
                    adapterClass = it.data().toVariant().value.toUInt32();
                }
                else if( it.key() == "Address" ) {
                    bdaddr = it.data().toVariant().value.toString();
                }
                else if( it.key() == "Powered" ) {
                    powered = it.data().toVariant().value.toBool();
                }
            }
        }
    }

    QDBusProxy *adapterProxy;
    QString adapterName;
    QString bdaddr;
    Q_UINT32 adapterClass;
    bool powered;
    bool discovering;
};

OBluetoothInterface::OBluetoothInterface( QObject* parent, const char* name, const QDBusObjectPath &path )
                    :QObject( parent, name )
{
    d = new OBluetoothInterface::Private( path );
    setName( d->adapterName );
    QObject::connect(d->adapterProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));
}

OBluetoothInterface::~OBluetoothInterface()
{
    delete d;
}

QString OBluetoothInterface::macAddress() const
{
    return d->bdaddr;
}

bool OBluetoothInterface::setUp( bool b )
{
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString("Powered");
    parameters << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(b)));
    QDBusMessage reply = d->adapterProxy->sendWithReply("SetProperty", parameters);
    if (reply.type() == QDBusMessage::ReplyMessage)
        return true;
    else
        return false;
}

bool OBluetoothInterface::isUp() const
{
    return d->powered;
}

OBluetoothInterface::DeviceIterator OBluetoothInterface::neighbourhood()
{
    QDBusConnection connection = QDBusConnection::systemBus();

    _devices.clear();
    d->discovering = true;
    d->adapterProxy->send("StartDiscovery", QValueList<QDBusData>());
    while( d->discovering )
        qApp->processEvents();

    return DeviceIterator( _devices );
}

void OBluetoothInterface::slotDBusSignal(const QDBusMessage& message)
{
    if( message.member() == "PropertyChanged" ) {
        if( message[0].toString() == "Discovering" ) {
            d->discovering = message[1].toVariant().value.toBool();
        }
    }
    else if( message.member() == "DeviceFound" ) {
        const QDBusDataMap<QString> adapterProps = message[1].toStringKeyMap();
        OBluetoothDevice *dev = new OBluetoothDevice( this, adapterProps );
        _devices.insert( message[0].toString(), dev );
    }
}

/*======================================================================================
 * OBluetoothDevice
 *======================================================================================*/

class OBluetoothDevice::Private
{
  public:
    Private( const QDBusObjectPath &path )
    {
        setupProxy(path);
        reloadInfo();
    }

    Private( const QDBusDataMap<QString> &props )
    {
        devProxy = NULL;
        loadInfo(props);
    }

    void setupProxy( const QDBusObjectPath &path )
    {
        QDBusConnection connection = QDBusConnection::systemBus();
        devProxy = new QDBusProxy(0);
        devProxy->setService("org.bluez");
        devProxy->setPath(path);
        devProxy->setInterface("org.bluez.Device");
        devProxy->setConnection(connection);
    }

    ~Private()
    {
        delete devProxy;
    }

    void reloadInfo()
    {
        devName = "";
        devClass = 0;
        bdaddr = "";
        QDBusMessage reply = devProxy->sendWithReply("GetProperties", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            const QDBusDataMap<QString> adapterProps = reply[0].toStringKeyMap();
            loadInfo( adapterProps );
        }
    }

    void loadInfo(const QDBusDataMap<QString> &props)
    {
        QDBusDataMap<QString>::ConstIterator it = props.begin();
        for (; it != props.end(); ++it) {
            if( it.key() == "Name" ) {
                devName = it.data().toVariant().value.toString();
            }
            else if( it.key() == "Class" ) {
                devClass = it.data().toVariant().value.toUInt32();
            }
            else if( it.key() == "Address" ) {
                bdaddr = it.data().toVariant().value.toString();
            }
        }
    }

    QDBusProxy *devProxy;
    QString devName;
    QString bdaddr;
    Q_UINT32 devClass;
};

OBluetoothDevice::OBluetoothDevice( QObject* parent, const QDBusDataMap<QString> &props )
                 :QObject( parent )
{
//    odebug << "OBluetoothDevice::OBluetoothDevice() - '" << name << "'" << oendl;
    d = new OBluetoothDevice::Private( props );
    setName( d->devName );
}

OBluetoothDevice::OBluetoothDevice( QObject* parent, const QDBusObjectPath &path )
                 :QObject( parent )
{
//    odebug << "OBluetoothDevice::OBluetoothDevice() - '" << path << "'" << oendl;
    d = new OBluetoothDevice::Private( path );
    setName( d->devName );
}

OBluetoothDevice::~OBluetoothDevice()
{
//    odebug << "OBluetoothDevice::~OBluetoothDevice()" << oendl;
    delete d;
}

QString OBluetoothDevice::macAddress() const
{
    return d->bdaddr;
}

QString OBluetoothDevice::deviceClass() const
{
    int devClass = d->devClass & 0xfff;
    int maj = devClass >> 8;
    int min = (devClass & 0x1f) >> 2;

    QString major,minor;
    major.sprintf( "Unknown(%d)", maj );
    minor.sprintf( "Unknown(%d)", min );

    switch ( maj )
    {
        case 0: major = "Miscellaneous";
        break;

        case 1: major = "Computer";
        switch ( min )
        {
            case 0: minor = "Uncategorized"; break;
            case 1: minor = "Desktop workstation"; break;
            case 2: minor = "Server"; break;
            case 3: minor = "Laptop"; break;
            case 4: minor = "Handheld"; break;
            case 5: minor = "Palm"; break;
            case 6: minor = "Wearable"; break;
        }
        break;

        case 2: major = "Phone";
        switch ( min )
        {
            case 0: minor = "Uncategorized"; break;
            case 1: minor = "Cellular"; break;
            case 2: minor = "Cordless"; break;
            case 3: minor = "Smart phone"; break;
            case 4: minor = "Wired modem or voice gateway"; break;
            case 5: minor = "Common ISDN Access"; break;
            case 6: minor = "Sim Card Reader"; break;
        }
        break;

        case 3: major = "LAN Access";
        break;

        case 4: major = "Audio/Video";
        switch ( min )
        {
            case 0: minor = "Uncategorized"; break;
            case 1: minor = "Device conforms to the Headset profile"; break;
            case 2: minor = "Hands-free"; break;
            case 3: minor = "Reserved(3)"; break;
            case 4: minor = "Microphone"; break;
            case 5: minor = "Loudspeaker"; break;
            case 6: minor = "Headphones"; break;
            case 7: minor = "Portable Audio"; break;
            case 8: minor = "Car Audio"; break;
            case 9: minor = "Set-top box"; break;
            case 10: minor = "HiFi Audio Device"; break;
            case 11: minor = "VCR"; break;
            case 12: minor = "Video Camera"; break;
            case 13: minor = "Camcorder"; break;
            case 14: minor = "Video Monitor"; break;
            case 15: minor = "Video Display and Loudspeaker"; break;
            case 16: minor = "Video Conferencing"; break;
            case 17: minor = "Reserved(17)"; break;
            case 18: minor = "Gaming/Toy"; break;
        }
        break;

        case 5: major = "Peripheral";
        switch ( min )
        {
            case 16: minor = "Keyboard"; break;
            case 32: minor = "Pointing Device"; break;
            case 48: minor = "Keyboard and Pointing Device"; break;
        }
        break;

        case 6: major = "Imaging";
        if (min & 4) minor = "Display";
        else if (min & 8) minor = "Camera";
        else if (min & 16) minor = "Scanner";
        else if (min & 32) minor = "Printer";
        break;

        case 63: major = "Uncategorized";
        break;
    }

    return QString( "%1:%2" ).arg( major ).arg( minor );
}

}
}
