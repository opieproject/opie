
#ifndef OpieToothManager_H
#define OpieToothManager_H

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qvaluelist.h>

#include "remotedevice.h"
#include "services.h"

class OProcess;
namespace OpieTooth {
  class Device;
  /** Manager manages a blueZ device (hci0 for example)
   *  with Manager you can control the things you
   *  could do from command line in a OO and asynchronus
   *  way.
   */
  class Manager : public QObject {
Q_OBJECT
  public:
    /** c'tor whichs create a new Manager
     *  @param device is the device to use. Either a mac or blueZ device name
     *
     */
    Manager( const QString &device );
    /** c'tor
     * @param dev The Device to be managed
     * We don't care of Device so you need to delete it
     */
    Manager( Device* dev );
    /**
     * c'tor
     */
    Manager();
    ~Manager();

    /** Set the manager to control a new device
     *  @param device the new device to control (hci0 )
     */
    void setDevice( const QString& device );
    /**
     * Convience functions for setting a new device
     */
    void setDevice( Device *dev );
    /**
     * Wether or not a device is connected. The function
     * is asynchron
     * If device is empty it will take the currently managed
     * device and see if it's up
     * for Remote devices it will ping and see.
     * @param either mac or hciX
     */
    void isConnected(const QString& device= QString::null );
    /**
     * same as above
     */
    void isConnected(Device *dev );

    /** this searchs for devices reachable from the
     *  currently managed device
     *  or from device if @param device is not empty
     */
    void searchDevices(const QString& device= QString::null );
    /** same as above
     *
     */
    void searchDevices(Device *d );

    /**
     * This will add the service @param name
     * to the sdpd daemon
     * It will start the daemon if necessary
     */
    void addService(const QString &name );
    /**
     * This will add the services @param names
     * to the sdpd daemon
     * It will start the daemon if necessary
     */
    void addServices( const QStringList& names );
    /**
     * This removes a service from the sdps
     */
    void removeService(const QString &name );
    /**
     * Removes a list from the sdpd
     */
    void removeServices(const QStringList& );

    /**
     * search for services on a remote device
     *
     */
    void searchServices( const QString& remDevice );
    /**
     * search for services on a remote device
     */
    void searchServices( const RemoteDevice& );
    /*static*/ QString toDevice( const QString& mac );
    /*static*/ QString toMac( const QString &device );

  signals:
    // device either mac or dev name
    // the first device is the device which you access
    void connected( const QString& device, bool connected );
    void addedService(  const QString& service, bool added );
    void removedService( const QString& service, bool removed );
    void foundServices( const QString& device, Services::ValueList );
    void foundDevices( const QString& device, RemoteDevice::ValueList );

private slots:
    void slotProcessExited(OProcess* );
    void slotSDPExited(OProcess*);
    void slotSDPOut(OProcess*, char*, int);
    void slotHCIExited(OProcess* );
    void slotHCIOut(OProcess*, char*, int );
  private:
      Services::ValueList parseSDPOutput( const QString& );
      RemoteDevice::ValueList parseHCIOutput( const QString& );
      OProcess *m_hcitool;
      OProcess *m_sdp; // not only one
      QString m_device;
      QMap<QString, QString> m_out;
      QMap<QString, QString> m_devices;
  };
};

#endif
