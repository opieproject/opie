
#ifndef OpieToothManager_Base_H
#define OpieToothManager_Base_H

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qdir.h>

#include "connection.h"
#include "remotedevice.h"
#include "services.h"


class OProcess;
namespace OpieTooth {
	class Device;
	/** This ressource manager manages a bluetooth protocol stack.
	 */
	class Manager_Base : public QObject {
	public:		
		
		/** Set the manager to control a new device
		 *  @param device the new device to control (hci0 )
		 */
		virtual void setDevice( const QString& mac ) = 0;
		
		/**
		 * Convience functions for setting a new device
		 */
		virtual void setDevice( Device* dev ) = 0;
		
		/**
		 * Wether or not a device is connected. The function
		 * is asynchron
		 * If device is empty it will take the currently managed
		 * device and see if it's up
		 * for Remote devices it will ping and see.
		 * @param either mac or hciX
		 */
		virtual void isAvailable(const QString& mac= QString::null ) = 0;
		
		/**
		 * same as above
		 */
		virtual void isAvailable(Device* dev ) = 0;
		
		/** this searchs for devices reachable from the
		 *  currently managed device
		 *  or from device if @param device is not empty
		 */
		virtual void searchDevices(const QString& dev= QString::null ) = 0;
		/** same as above
		 *
		 */
		virtual void searchDevices(Device* d ) = 0;
		
		/**
		 * This will add the service @param name
		 * to the sdpd daemon
		 * It will start the daemon if necessary
		 */
		virtual void addService(const QString& name ) = 0;
		
		/**
		 * This will add the services @param names
		 * to the sdpd daemon
		 * It will start the daemon if necessary
		 */
		virtual void addServices( const QStringList& names ) = 0;
		
		/**
		 * This removes a service from the sdps
		 */
		virtual void removeService(const QString& name ) = 0;
		
		/**
		 * Removes a list from the sdpd
		 */
		virtual void removeServices(const QStringList& names) = 0;
		
		/**
		 * search for services on a remote device
		 *
		 */
		virtual void searchServices( const QString& remDeviceMac ) = 0;
		
		/**
		 * search for services on a remote device
		 */
		virtual void searchServices( const RemoteDevice& ) = 0;
		
		/**
		 * Starts to connect to the device
		 * in @param
		 */
		virtual void connectTo(const QString& ) = 0;
		
		/**
		 * Searches for active connections
		 * the result is emitted with the
		 * connections signal
		 */
		virtual void searchConnections() = 0;
		
		virtual void signalStrength( const QString &mac ) = 0;
		//// not implemented yet
		virtual /*static*/ QString toDevice( const QString& mac ) = 0;
		virtual /*static*/ QString toMac( const QString &device ) = 0;
		//// not implemented yet over
		
	signals:
		// device either mac or dev name
		// the first device is the device which you access
		virtual void available( const QString& device, bool connected ) = 0;
		virtual void addedService(  const QString& service, bool added ) = 0;
		virtual void removedService( const QString& service, bool removed ) = 0;
		virtual void foundServices( const QString& device, Services::ValueList ) = 0;
		virtual void foundDevices( const QString& device, RemoteDevice::ValueList ) = 0;
		virtual void connections( ConnectionState::ValueList ) = 0;
		virtual void signalStrength( const QString& mac, const QString& strengh ) = 0;
		
	};
	
}
#endif
