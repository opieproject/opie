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

#ifndef OBLUETOOTH_H
#define OBLUETOOTH_H

#include <qobject.h>
#include <qdict.h>
#include <qmap.h>

#include <opie2/obluetoothservices.h>

class QDBusProxy;
class QDBusMessage;
class QDBusObjectPath;
class QDBusVariant;
template <typename T> class QDBusDataMap;

namespace Opie {
namespace Bluez {

class OBluetoothInterface;
class OBluetoothDevice;

/**
 * @brief A container class for all bluetooth interfaces
 *
 * This class provides access to all available bluetooth interfaces.
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class OBluetooth : public QObject
{
  Q_OBJECT

  public:
    typedef QDict<OBluetoothInterface> InterfaceMap;
    typedef QDictIterator<OBluetoothInterface> InterfaceIterator;

  public:
    /**
     * @returns the number of available interfaces
     */
    int count() const;
    /**
     * @returns a pointer to the (one and only) @ref OBluetooth instance.
     */
    static OBluetooth* instance();
    /**
     * @returns an iterator usable for iterating through all network interfaces.
     */
    InterfaceIterator iterator() const;
    /**
     * @returns true, if the @a interface is present.
     */
    bool isPresent( const char* interface ) const;
    /**
     * @returns a pointer to the @ref OBluetoothInterface object for the specified @a interface or 0, if not found.
     * @see OBluetoothInterface
     */
    OBluetoothInterface* interface( const QString& interface ) const;
    /**
     * @returns a pointer to the @ref OBluetoothInterface object for the default @a interface or 0, if none present.
     * @see OBluetoothInterface
     */
    OBluetoothInterface* defaultInterface() const;
    /**
     * @internal Rebuild the internal interface database
     * @note Sometimes it might be useful to call this from client code,
     * e.g. after issuing a cardctl insert
     */
    void synchronize();

  signals:
    void defaultInterfaceChanged( OBluetoothInterface *intf );
    void interfaceAdded( OBluetoothInterface *intf );
    void interfaceRemoved( OBluetoothInterface *intf );

  protected:
    OBluetooth();
    ~OBluetooth();

  protected slots:
    void slotDBusSignal(const QDBusMessage& message);

  private:
    static OBluetooth* _instance;
    InterfaceMap _interfaces;
    QDBusProxy *m_bluezManagerProxy;
    class Private;
    Private *d;
};

/*======================================================================================
 * OBluetoothInterface
 *======================================================================================*/

/**
 * @brief An bluetooth interface wrapper.
 *
 * This class provides a wrapper for a bluetooth HCI device. All the cumbersome details of
 * Linux ioctls are hidden under a convenient high-level interface.
 * @warning Most of the setting methods contained in this class require the appropriate
 * process permissions to work.
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class OBluetoothInterface : public QObject
{
  Q_OBJECT

  friend class OBluetoothDevice;

  public:
    typedef QDict<OBluetoothDevice> DeviceMap;
    typedef QDictIterator<OBluetoothDevice> DeviceIterator;

  public:
    /**
     * Constructor. Normally you don't create @ref OBluetoothInterface objects yourself,
     * but access them via @ref OBluetooth::interface().
     */
    OBluetoothInterface( QObject* parent, const QDBusObjectPath &path );
    /**
     * Destructor.
     */
    virtual ~OBluetoothInterface();
    /**
     * @return the MAC address of the interface.
     */
    QString macAddress() const;
    /**
     * @returns the name used when making the interface visible to other devices.
     */
    QString publicName() const;
    /**
     * @returns the class of device.
     */
    QString deviceClass() const;
    /**
     * @returns the discoverable status of the interface
     */
    bool discoverable() const;
    /**
     * @returns true if discovery is in progress
     */
    bool discovering() const;
    /**
     * @returns the dbus path to the adapter, for extensions
     */
    const QString &adapterPath();
    /**
     * Setting an interface to up enables it to receive packets.
     */
    bool setUp( bool );
    /**
     * @returns true if the interface is up.
     */
    bool isUp() const;
    /**
     * Set discoverable status of the interface
     */
    bool setDiscoverable( bool );
    /**
     * Set the used when making the interface visible to other devices.
     */
    bool setPublicName( const QString & );
    /**
     * Starts discovery, enumerates devices and then returns when discovery times out.
     * @returns an iterator usable for iterating through the devices in range.
     */
    DeviceIterator neighbourhood();

    /**
     * Find a specific device, if we know about it
     */
    OBluetoothDevice *findDevice(const QString &bdaddr);
    /**
     * Find a specific device asynchronously, adding it if we don't already know about it
     * Emits deviceFound when device is found
     */
    void findDeviceCreate(const QString &bdaddr);
    /**
     * Remove a device node, including pairing information. 
     */
    void removeDevice( const QString &bdaddr );

  public slots:
    /**
     * Starts discovery
     */
    void startDiscovery();
    /**
     * Stops discovery
     */
    void stopDiscovery();

  signals:
    /**
     * Triggered when a device is found during discovery.
     */
    void deviceDiscovered(OBluetoothDevice *dev);
    /**
     * Triggered when a property of the interface changes.
     */
    void propertyChanged(const QString &prop);
    /**
     * Triggered when a device is found/added (during discovery, or in response to findDeviceCreate).
     */
    void deviceFound( OBluetoothDevice *dev, bool newDevice);

  protected slots:
    void slotDBusSignal(const QDBusMessage& message);
    void slotAsyncReply(int callID, const QDBusMessage& reply);
  protected:
    void addDevice( const QString &bdaddr );
  private:
    DeviceMap _devices;
    class Private;
    Private *d;
};

/*======================================================================================
 * OBluetoothDevice
 *======================================================================================*/

/**
 * @brief An bluetooth (remote) device abstraction.
 *
 * This class resembles a (remote) bluetooth device.
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class OBluetoothDevice : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor for manually added device
     */
    OBluetoothDevice( OBluetoothInterface *parent, const QDBusObjectPath &path );
    /**
     * Constructor for device added through discovery
     */
    OBluetoothDevice( OBluetoothInterface *parent, const QMap<QString,QDBusVariant> &props );
    /**
     * Destructor.
     */
    virtual ~OBluetoothDevice();
    /**
      * @returns the MAC address of the device's interface.
      */
    QString macAddress() const;
    /**
     * @returns the class of device.
     */
    QString deviceClass() const;
    /**
     * @returns the dbus path to the device, for extensions
     */
    const QString &devicePath() const;
    /**
     * @internal sets the dbus path to the device
     */
    void setDevicePath( const QString &path );
    /**
     * Discover the device's available services
     * servicesFound signal will be emitted when discovery is complete
     */
    void discoverServices();
    /**
     * The device's available services
     * available after calling discoverServices()
     */
    OBluetoothServices::ValueList services();
    /**
     * Set whether the device is trusted or not
     */
    bool setTrusted(bool);
    /**
     * @internal Convert a device class number to a string
     */
    static QString deviceClassString(Q_UINT32 dev_class);

  signals:
    void servicesFound( OBluetoothDevice *dev ); 

  protected slots:
    void slotAsyncReply(int, const QDBusMessage&);

  private:
    class Private;
    Private *d;
};

}
}
#endif

