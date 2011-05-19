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
#include <opie2/xmltree.h>
using namespace Opie::Core;

/* Qt */
#include <qapplication.h>
#include <qintdict.h>
#include <qxml.h>

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
//    odebug << "OBluetooth::synchronize() - gathering available HCI devices" << oendl;
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
//                    odebug << "OBluetooth::synchronize() - found device " << iface->name() << oendl;
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

class CallRecord
{
public:
    enum CallMode { MODE_FIND, MODE_CREATE, MODE_FINDREMOVE };

    CallRecord( const QString &bdaddr, CallMode mode ) {
        this->bdaddr = bdaddr;
        this->mode = mode;
    }

    QString bdaddr;
    CallMode mode;
};

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
        callRecs.setAutoDelete( true );
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
    QIntDict<CallRecord> callRecs;
};

OBluetoothInterface::OBluetoothInterface( QObject* parent, const QDBusObjectPath &path )
                    :QObject( parent )
{
    d = new OBluetoothInterface::Private( path );
    setName( d->interfaceName );
    QObject::connect(d->adapterProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));
    QObject::connect(d->adapterProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                    this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
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
    return d->adapterProxy->send("SetProperty", parameters);
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
    return d->adapterProxy->send("SetProperty", parameters);
}

bool OBluetoothInterface::setPublicName( const QString &name )
{
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString("Name");
    parameters << QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(name)));
    return d->adapterProxy->send("SetProperty", parameters);
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

OBluetoothDevice *OBluetoothInterface::findDevice( const QString &bdaddr )
{
    return _devices.find(bdaddr);
}

void OBluetoothInterface::findDeviceCreate( const QString &bdaddr )
{
    OBluetoothDevice *dev = _devices.find(bdaddr);
    if( dev )
        emit deviceFound( dev, false );
    else
        addDevice( bdaddr );
}

void OBluetoothInterface::insertDevice( OBluetoothDevice *dev )
{
    _devices.insert( dev->macAddress(), dev );
    connect( dev, SIGNAL( propertyChanged( OBluetoothDevice *, const QString& ) ), this, SIGNAL( devicePropertyChanged( OBluetoothDevice *, const QString& ) ) );
    connect( dev, SIGNAL( servicesFound( OBluetoothDevice * ) ), this, SIGNAL( deviceServicesFound( OBluetoothDevice * ) ) );
}

void OBluetoothInterface::addDevice( const QString &bdaddr )
{
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString(bdaddr);
    int callId = d->adapterProxy->sendWithAsyncReply("FindDevice", parameters);
    d->callRecs.insert( callId, new CallRecord( bdaddr, CallRecord::MODE_FIND ) );
}

void OBluetoothInterface::removeDevice( const QString &bdaddr )
{
    OBluetoothDevice *dev = findDevice( bdaddr );
    if( dev )
        _devices.remove( bdaddr );
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString(bdaddr);
    int callId = d->adapterProxy->sendWithAsyncReply("FindDevice", parameters);
    d->callRecs.insert( callId, new CallRecord( bdaddr, CallRecord::MODE_FINDREMOVE ) );
}

void OBluetoothInterface::slotAsyncReply( int callId, const QDBusMessage& reply )
{
    CallRecord *rec = d->callRecs[callId];
    if( rec ) {
        if (reply.type() == QDBusMessage::ReplyMessage) {
            // Either FindDevice or CreateDevice, both return the path on success
            QDBusObjectPath path = reply[0].toObjectPath();
            switch( rec->mode ) {
                case CallRecord::MODE_FINDREMOVE: {
                    QValueList<QDBusData> parameters;
                    parameters << QDBusData::fromObjectPath( path );
                    d->adapterProxy->send("RemoveDevice", parameters);
                    break;
                }
                case CallRecord::MODE_FIND: // fall through
                case CallRecord::MODE_CREATE: {
                    OBluetoothDevice *dev = _devices.find( rec->bdaddr );
                    if( dev ) {
                        dev->setDevicePath( path );
                        emit deviceFound( dev, false );
                    }
                    else {
                        OBluetoothDevice *dev = new OBluetoothDevice( this, path );
                        insertDevice( dev );
                        emit deviceFound( dev, true );
                    }
                    break;
                }
            }
        }
        else {
            if( rec->mode == CallRecord::MODE_CREATE ) {
                // Failed to create the device
                // FIXME raise an error here
                odebug << "CreateDevice " << rec->bdaddr << " failed: " << reply.error().name() << ": " << reply.error().message() << oendl;
            }
            else if( rec->mode == CallRecord::MODE_FIND ) {
                odebug << "FindDevice " << rec->bdaddr << " failed: " << reply.error().name() << ": " << reply.error().message() << oendl;
                // Device not found, let's create it (which will try to contact the device)
                QValueList<QDBusData> parameters;
                parameters << QDBusData::fromString(rec->bdaddr);
                int newCallId = d->adapterProxy->sendWithAsyncReply("CreateDevice", parameters);
                d->callRecs.insert( newCallId, new CallRecord( rec->bdaddr, CallRecord::MODE_CREATE ) );
            }
        }
        d->callRecs.remove( callId );
    }
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
        // Device discovered
        const QMap<QString,QDBusVariant> devProps = message[1].toStringKeyMap().toVariantMap();
        QString bdaddr = message[0].toString();
        OBluetoothDevice *dev = _devices.find( bdaddr );
        // FIXME update existing device props
        if( !dev ) {
            dev = new OBluetoothDevice( this, devProps );
            insertDevice( dev );
        }
        emit deviceDiscovered(dev);
    }
}

/*======================================================================================
 * OBluetoothDevice
 *======================================================================================*/

class OBluetoothDevice::Private
{
  public:
    Private()
    {
        devProxy = new QDBusProxy(0);
        autoDiscover = false;
    }

    ~Private()
    {
        delete devProxy;
    }

    void setupProxy( OBluetoothDevice *dev )
    {
        QDBusConnection connection = QDBusConnection::systemBus();
        devProxy->setService("org.bluez");
        devProxy->setPath(devPath);
        devProxy->setInterface("org.bluez.Device");
        devProxy->setConnection(connection);
        QObject::connect(devProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                        dev, SLOT(slotDBusSignal(const QDBusMessage&)));
        QObject::connect(devProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                        dev, SLOT(slotAsyncReply(int, const QDBusMessage&)));
    }

    void reloadInfo()
    {
        QDBusMessage reply = devProxy->sendWithReply("GetProperties", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            const QDBusDataMap<QString> props = reply[0].toStringKeyMap();
            devProps = props.toVariantMap();
        }
    }

    void parseServiceRecord( XMLElement *recelem )
    {
        OBluetoothServices rec;
        XMLElement *elem = recelem->firstChild();
        while( elem ) {
            if( elem->tagName() == "attribute" ) {
                QString id = elem->attributes()["id"];
                if( id == "0x0000" ) {
                    // ServiceRecordHandle
                    XMLElement *elem2 = elem->firstChild();
                    if( elem2 )
                        rec.setRecHandle(elem2->attributes()["value"].mid(2).toUInt(0, 16));
                }
                else if( id == "0x0001" ) {
                    // ServiceClassIDList
                    XMLElement *seqelem = elem->firstChild();
                    while( seqelem ) {
                        XMLElement *elem2 = seqelem->firstChild();
                        if( elem2 ) {
                            rec.insertClassId( elem2->attributes()["value"].mid(2).toUInt(0, 16), "");
                        }
                        seqelem = seqelem->nextChild();
                    }
                }
                else if( id == "0x0004" ) {
                    // ProtocolDescriptorList
                    XMLElement *seqelem = elem->firstChild();
                    if( seqelem ) // sequence
                        seqelem = seqelem->firstChild();
                    while( seqelem ) {
                        OBluetoothServices::ProtocolDescriptor prtdesc;
                        XMLElement *seq2elem = seqelem->firstChild();
                        while( seq2elem ) {
                            if( seq2elem->tagName() == "uuid" )
                                prtdesc.setId( seq2elem->attributes()["value"].mid(2).toUInt(0, 16) );
                            else if( seq2elem->tagName().startsWith( "uint" ) )
                                prtdesc.setPort( seq2elem->attributes()["value"].mid(2).toUInt(0, 16) );
                            seq2elem = seq2elem->nextChild();
                        }
                        if( prtdesc.id() > 0 && prtdesc.id() != 0x0100 ) // no L2CAP
                            rec.insertProtocolDescriptor( prtdesc );
                        seqelem = seqelem->nextChild();
                    }
                }
                else if( id == "0x0009" ) {
                    // BluetoothProfileDescriptorList
                    XMLElement *seqelem = elem->firstChild();
                    if( seqelem ) // sequence
                        seqelem = seqelem->firstChild();
                    while( seqelem ) {
                        OBluetoothServices::ProfileDescriptor prfdesc;
                        XMLElement *seq2elem = seqelem->firstChild();
                        while( seq2elem ) {
                            if( seq2elem->tagName() == "uuid" )
                                prfdesc.setId( seq2elem->attributes()["value"].mid(2).toUInt(0, 16) );
                            else if( seq2elem->tagName().startsWith( "uint" ) )
                                prfdesc.setVersion( seq2elem->attributes()["value"].mid(2).toUInt(0, 16) );
                            seq2elem = seq2elem->nextChild();
                        }
                        if( prfdesc.idInt() > 0 )
                            rec.insertProfileDescriptor( prfdesc );
                        seqelem = seqelem->nextChild();
                    }
                }
                else if( id == "0x0100" ) {
                    // ServiceName
                    XMLElement *elem2 = elem->firstChild();
                    if( elem2 )
                        rec.setServiceName( elem2->attributes()["value"] );
                }
            }

            elem = elem->nextChild();
        }

        if( rec.recHandle() && rec.classIdList().count() > 0 && !rec.classIdList().contains( 0x1200 ) ) { // PnP class ID
            services.append( rec );
        }
    }

    void parseServices( XMLElement *root )
    {
        XMLElement *elem = root->firstChild();
        while( elem ) {
            if( elem->tagName() == "record" ) 
                parseServiceRecord( elem );
            elem = elem->nextChild();
        }
    }

    QDBusProxy *devProxy;
    QMap<QString,QDBusVariant> devProps;
    QString devPath;
    OBluetoothServices::ValueList services;
    bool autoDiscover;
};

OBluetoothDevice::OBluetoothDevice( OBluetoothInterface *parent, const QDBusObjectPath &path )
                 :QObject( parent )
{
    d = new OBluetoothDevice::Private();
    d->devPath = path;
    d->setupProxy(this);
    d->reloadInfo();

    setName( d->devProps["Name"].value.toString() );
}

OBluetoothDevice::OBluetoothDevice( OBluetoothInterface *parent, const QMap<QString,QDBusVariant> &props )
                 :QObject( parent )
{
    d = new OBluetoothDevice::Private();
    d->devProps = props;

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

const QString &OBluetoothDevice::devicePath() const
{
    return d->devPath;
}

bool OBluetoothDevice::isConnected() const
{
    return d->devProps["Connected"].value.toBool();
}

void OBluetoothDevice::setDevicePath( const QString &path )
{
    if( d->devPath.isEmpty() ) {
        d->devPath = path;
        d->setupProxy(this);
        if( d->autoDiscover ) {
            d->autoDiscover = false;
            discoverServices();
        }
    }
}

void OBluetoothDevice::discoverServices()
{
    if( d->devPath.isEmpty() ) {
        d->autoDiscover = true;
        ((OBluetoothInterface *)parent())->addDevice( macAddress() );
    }
    else {
        d->services.clear();
        QValueList<QDBusData> parameters;
        parameters << QDBusData::fromString("");
        d->devProxy->sendWithAsyncReply("DiscoverServices", parameters);
    }
}

OBluetoothServices::ValueList OBluetoothDevice::services()
{
    return d->services;
}

bool OBluetoothDevice::setTrusted( bool b )
{
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString("Trusted");
    parameters << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(b)));
    return d->devProxy->send("SetProperty", parameters);
}

void OBluetoothDevice::slotAsyncReply( int, const QDBusMessage& msg )
{
    // At the moment we get away with not checking anything about the message
    // as we don't use async messages on devices for anything except service discovery
    d->services.clear();
    if( msg.type() == QDBusMessage::ReplyMessage && msg.count() == 1 ) {
        QMap<Q_UINT32,QString> map = msg[0].toUInt32KeyMap().toStringMap();

        d->services.clear();
        QMap<Q_UINT32,QString>::Iterator it = map.begin();
        for( ; it != map.end(); ++it ) {
            QXmlInputSource src;
            src.setData( it.data() );
            XMLElement *root = XMLElement::load( src );
            if( root )
                d->parseServices(root);
        }
        emit servicesFound( this );
    }
    else if (msg.type() == QDBusMessage::ErrorMessage) {
        odebug << "********* serv disc fail: " << msg.error().name() << ": " << msg.error().message() << oendl;
        if( msg.error().name() == "org.bluez.Error.InProgress" )
            discoverServices(); // just try again
        else
            emit servicesFound( this ); // so we get something back
    }
}

void OBluetoothDevice::slotDBusSignal(const QDBusMessage& message)
{
    if( message.member() == "PropertyChanged" ) {
        QString propName = message[0].toString();
        d->devProps[propName] = message[1].toVariant();
        emit propertyChanged( this, propName );
    }
}

QString OBluetoothDevice::deviceClassString(Q_UINT32 dev_class)
{
    int devClass = dev_class & 0xfff;
    int maj = devClass >> 8;
    int min = (devClass & 0x1f) >> 2;

    QString major,minor;
    major = tr("Unknown(%1)").arg(maj);
    minor = tr("Unknown(%1)").arg(min);

    switch ( maj )
    {
        case 0: major = tr("Miscellaneous");
        break;

        case 1: major = tr("Computer");
        switch ( min )
        {
            case 0: minor = tr("Uncategorized"); break;
            case 1: minor = tr("Desktop workstation"); break;
            case 2: minor = tr("Server"); break;
            case 3: minor = tr("Laptop"); break;
            case 4: minor = tr("Handheld"); break;
            case 5: minor = tr("Palm"); break;
            case 6: minor = tr("Wearable"); break;
        }
        break;

        case 2: major = tr("Phone");
        switch ( min )
        {
            case 0: minor = tr("Uncategorized"); break;
            case 1: minor = tr("Cellular"); break;
            case 2: minor = tr("Cordless"); break;
            case 3: minor = tr("Smart phone"); break;
            case 4: minor = tr("Wired modem or voice gateway"); break;
            case 5: minor = tr("Common ISDN Access"); break;
            case 6: minor = tr("Sim Card Reader"); break;
        }
        break;

        case 3: major = tr("LAN Access");
        break;

        case 4: major = tr("Audio/Video");
        switch ( min )
        {
            case 0: minor = tr("Uncategorized"); break;
            case 1: minor = tr("Device conforms to the Headset profile"); break;
            case 2: minor = tr("Hands-free"); break;
            case 3: minor = tr("Reserved(3)"); break;
            case 4: minor = tr("Microphone"); break;
            case 5: minor = tr("Loudspeaker"); break;
            case 6: minor = tr("Headphones"); break;
            case 7: minor = tr("Portable Audio"); break;
            case 8: minor = tr("Car Audio"); break;
            case 9: minor = tr("Set-top box"); break;
            case 10: minor = tr("HiFi Audio Device"); break;
            case 11: minor = tr("VCR"); break;
            case 12: minor = tr("Video Camera"); break;
            case 13: minor = tr("Camcorder"); break;
            case 14: minor = tr("Video Monitor"); break;
            case 15: minor = tr("Video Display and Loudspeaker"); break;
            case 16: minor = tr("Video Conferencing"); break;
            case 17: minor = tr("Reserved(17)"); break;
            case 18: minor = tr("Gaming/Toy"); break;
        }
        break;

        case 5: major = tr("Peripheral");
        switch ( min )
        {
            case 16: minor = tr("Keyboard"); break;
            case 32: minor = tr("Pointing Device"); break;
            case 48: minor = tr("Keyboard and Pointing Device"); break;
        }
        break;

        case 6: major = tr("Imaging");
        if (min & 4) minor = tr("Display");
        else if (min & 8) minor = tr("Camera");
        else if (min & 16) minor = tr("Scanner");
        else if (min & 32) minor = tr("Printer");
        break;

        case 63: major = tr("Uncategorized");
        break;
    }

    return QString( "%1:%2" ).arg( major ).arg( minor );
}

}
}
