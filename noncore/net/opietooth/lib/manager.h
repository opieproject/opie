
#ifndef OpieToothManager_H
#define OpieToothManager_H

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>

namespace OpieTooth {
  class Device;
  class Manager : public QObject {
Q_OBJECT
  public:
    // Manage MAC device or by name (hci0)
    Manager( const QString &device );
    // Manage a with Device attached device
    Manager( Device* dev );
    // manage nothing ;)
    Manager();
    ~Manager();

    // setDevice either MAC or hci0
    // ups it
    void setDevice( const QString& device );
    void setDevice( Device *dev );
    // asynchron connect to signal
    void isConnected(const QString& device );
    void isConnected(Device *dev );

    // if c'tor with device or setDevice everythning is ok
    void searchDevices();
    // use an alternate device
    void searchDevices(const QString& device);
    void searchDevices(Device *d );

    void addService(const QString &name );
    void addServices( const QStringList& names );
    void removeService(const QString &name );
    void removeServices(const QStringList& );

    void searchServices( const QString& remDevice );
    /*static*/ QString toDevice( const QString& mac );
    /*static*/ QString toMac( const QString &device );

  signals:
    // device either mac or dev name
    // the first device is the device which you access
    void connected( const QString& device, bool connected );
    void addedService( const QString& device, const QString& service, bool added );
    void removedService( const QString& device, const QString& service, bool removed );
    void foundServices( const QString& device, Services::ValueList );
    void foundDevices( const QString& device, RemoteDevices::ValueList );
  };
};

#endif
