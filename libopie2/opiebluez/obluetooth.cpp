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

class OBluetooth::Private
{
  public:
    Private()
    {
        defaultIntf = NULL;
    }
  public:
    OBluetoothInterface *defaultIntf;
};

OBluetooth* OBluetooth::_instance = 0;

OBluetooth::OBluetooth()
    : m_bluezManagerProxy(0)
{
    d = new OBluetooth::Private();
    synchronize();
}

OBluetooth::~OBluetooth()
{
    delete d;
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

bool OBluetooth::isPresent( const char* interface ) const
{
    return ( _interfaces.find( QString(interface) ) != 0 );
}

OBluetoothInterface* OBluetooth::interface( const QString& iface ) const
{
    return _interfaces[iface];
}

OBluetoothInterface* OBluetooth::defaultInterface() const
{
    return d->defaultIntf;
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
        QObject::connect(m_bluezManagerProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                        this, SLOT(slotDBusSignal(const QDBusMessage&)));
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
                    OBluetoothInterface *iface  = new OBluetoothInterface( this, (*it2) );
                    odebug << "OBluetooth::synchronize() - found device " << iface->name() << oendl;
                    _interfaces.insert( (*it2), iface );
                }
            }
        }
    }

    reply = m_bluezManagerProxy->sendWithReply("DefaultAdapter", QValueList<QDBusData>());
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
        d->defaultIntf = _interfaces.find( reply[0].toObjectPath() );
    }
}

void OBluetooth::slotDBusSignal(const QDBusMessage& message)
{
    if( message.member() == "DefaultAdapterChanged" ) {
        d->defaultIntf = _interfaces.find( message[0].toObjectPath() );
        emit defaultInterfaceChanged( d->defaultIntf );
    }
    else if( message.member() == "AdapterAdded" ) {
        OBluetoothInterface *intf = new OBluetoothInterface( this, message[0].toObjectPath() );
        _interfaces.insert( message[0].toObjectPath(), intf );
        emit interfaceAdded( intf );
    }
    else if( message.member() == "AdapterRemoved" ) {
        OBluetoothInterface *intf = _interfaces.find( message[0].toObjectPath() );
        if( intf ) {
            emit interfaceRemoved( intf );
            if( intf == d->defaultIntf ) {
                d->defaultIntf = NULL;
                emit defaultInterfaceChanged( NULL ); // for convenience
            }
            _interfaces.remove( message[0].toObjectPath() );
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
        interfaceName = path.mid(path.findRev('/')+1);
        adapterPath = path;
    }

    ~Private()
    {
        delete adapterProxy;
    }

    void reloadInfo()
    {
        QDBusMessage reply = adapterProxy->sendWithReply("GetProperties", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            adapterProps = reply[0].toStringKeyMap().toVariantMap();
        }
    }

    QDBusProxy *adapterProxy;
    QMap<QString,QDBusVariant> adapterProps;
    QString interfaceName;
    QString adapterPath;
    bool discovering;
};

OBluetoothInterface::OBluetoothInterface( QObject* parent, const QDBusObjectPath &path )
                    :QObject( parent )
{
    d = new OBluetoothInterface::Private( path );
    setName( d->interfaceName );
    QObject::connect(d->adapterProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));
}

OBluetoothInterface::~OBluetoothInterface()
{
    delete d;
}

QString OBluetoothInterface::macAddress() const
{
    return d->adapterProps["Address"].value.toString();
}

QString OBluetoothInterface::deviceClass() const
{
    return OBluetoothDevice::deviceClassString( d->adapterProps["Class"].value.toUInt32() );
}

QString OBluetoothInterface::publicName() const
{
    return d->adapterProps["Name"].value.toString();
}

bool OBluetoothInterface::discoverable() const
{
    return d->adapterProps["Discoverable"].value.toBool();
}

bool OBluetoothInterface::discovering() const
{
    return d->discovering;
}

const QString &OBluetoothInterface::adapterPath()
{
    return d->adapterPath;
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
    return d->adapterProps["Powered"].value.toBool();
}

bool OBluetoothInterface::setDiscoverable( bool b )
{
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString("Discoverable");
    parameters << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(b)));
    QDBusMessage reply = d->adapterProxy->sendWithReply("SetProperty", parameters);
    if (reply.type() == QDBusMessage::ReplyMessage)
        return true;
    else
        return false;
}

bool OBluetoothInterface::setPublicName( const QString &name )
{
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString("Name");
    parameters << QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(name)));
    QDBusMessage reply = d->adapterProxy->sendWithReply("SetProperty", parameters);
    if (reply.type() == QDBusMessage::ReplyMessage)
        return true;
    else
        return false;
}

OBluetoothInterface::DeviceIterator OBluetoothInterface::neighbourhood()
{
    startDiscovery();
    while( d->discovering )
        qApp->processEvents();

    return DeviceIterator( _devices );
}

void OBluetoothInterface::startDiscovery()
{
    if( ! d->discovering ) {
        _devices.clear();
        d->discovering = true;
        d->adapterProxy->send("StartDiscovery", QValueList<QDBusData>());
    }
}

void OBluetoothInterface::stopDiscovery()
{
    d->adapterProxy->send("StopDiscovery", QValueList<QDBusData>());
}

void OBluetoothInterface::slotDBusSignal(const QDBusMessage& message)
{
    if( message.member() == "PropertyChanged" ) {
        QString propName = message[0].toString();
        if( propName == "Discovering" ) {
            d->discovering = message[1].toVariant().value.toBool();
        }
        d->adapterProps[propName] = message[1].toVariant();
        emit propertyChanged( propName );
    }
    else if( message.member() == "DeviceFound" ) {
        const QDBusDataMap<QString> adapterProps = message[1].toStringKeyMap();
        OBluetoothDevice *dev = new OBluetoothDevice( this, adapterProps );
        _devices.insert( message[0].toString(), dev );
        emit deviceFound(dev);
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
        QDBusMessage reply = devProxy->sendWithReply("GetProperties", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            const QDBusDataMap<QString> props = reply[0].toStringKeyMap();
            loadInfo( props );
        }
    }

    void loadInfo(const QDBusDataMap<QString> &props)
    {
        devProps = props.toVariantMap();
    }

    QDBusProxy *devProxy;
    QMap<QString,QDBusVariant> devProps;
};

OBluetoothDevice::OBluetoothDevice( QObject* parent, const QDBusDataMap<QString> &props )
                 :QObject( parent )
{
//    odebug << "OBluetoothDevice::OBluetoothDevice() - '" << name << "'" << oendl;
    d = new OBluetoothDevice::Private( props );
    setName( d->devProps["Name"].value.toString() );
}

OBluetoothDevice::OBluetoothDevice( QObject* parent, const QDBusObjectPath &path )
                 :QObject( parent )
{
//    odebug << "OBluetoothDevice::OBluetoothDevice() - '" << path << "'" << oendl;
    d = new OBluetoothDevice::Private( path );
    setName( d->devProps["Name"].value.toString() );
}

OBluetoothDevice::~OBluetoothDevice()
{
//    odebug << "OBluetoothDevice::~OBluetoothDevice()" << oendl;
    delete d;
}

QString OBluetoothDevice::macAddress() const
{
    return d->devProps["Address"].value.toString();
}

QString OBluetoothDevice::deviceClass() const
{
    return OBluetoothDevice::deviceClassString( d->devProps["Class"].value.toUInt32() );
}

QString OBluetoothDevice::deviceClassString(Q_UINT32 dev_class)
{
    int devClass = dev_class & 0xfff;
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
