
#ifndef OpieToothManager_H
#define OpieToothManager_H

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qvaluelist.h>

#include "connection.h"
#include "remotedevice.h"
#include "services.h"
#include "manager_base.h"
#include "plugin_interface.h"

#include <affix/sdp.h>

class OProcess;
namespace OpieTooth {

    class Device;

    /** Manager manages a blueZ device (hci0 for example)
     *  without Manager you can control the things you
     *  could do from command line in a OO and asynchronus
     *  way.
     */
    class Manager : public Manager_Base {
        Q_OBJECT
    public:
        /** c'tor whichs create a new Manager
         *  @param device is the device to use. Either a mac or blueZ device name
         *
         */
        Manager( const QString& mac );

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
        void setDevice( const QString& mac );

        /**
         * Convience functions for setting a new device
         */
        void setDevice( Device* dev );

        /**
         * Wether or not a device is connected. The function
         * is asynchron
         * If device is empty it will take the currently managed
         * device and see if it's up
         * for Remote devices it will ping and see.
         * @param either mac or hciX
         */
        void isAvailable(const QString& mac= QString::null );

        /**
         * same as above
         */
        void isAvailable(Device* dev );

        /** this searchs for devices reachable from the
         *  currently managed device
         *  or from device if @param device is not empty
         */
        void searchDevices(const QString& dev= QString::null );
        /** same as above
         *
         */
        void searchDevices(Device* d );

        /**
         * This will add the service @param name
         * to the sdpd daemon
         * It will start the daemon if necessary
         */
        void addService(const QString& name );

        /**
         * This will add the services @param names
         * to the sdpd daemon
         * It will start the daemon if necessary
         */
        void addServices( const QStringList& names );

        /**
         * This removes a service from the sdps
         */
        void removeService(const QString& name );

        /**
         * Removes a list from the sdpd
         */
        void removeServices(const QStringList& names);

        /**
         * search for services on a remote device
         *
         */
        void searchServices( const QString& remDeviceMac );

        /**
         * search for services on a remote device
         */
        void searchServices( const RemoteDevice& );

        /**
         * Starts to connect to the device
         * in @param
         */
        void connectTo(const QString& );

        /**
         * Searches for active connections
         * the result is emitted with the
         * connections signal
         */
        void searchConnections();

	void signalStrength( const QString &mac );
//// not implemented yet
        /*static*/ QString toDevice( const QString& mac );
        /*static*/ QString toMac( const QString &device );
//// not implemented yet over

    signals:
        // device either mac or dev name
        // the first device is the device which you access
        void available( const QString& device, bool connected );
        void addedService(  const QString& service, bool added );
        void removedService( const QString& service, bool removed );
        void foundServices( const QString& device, Services::ValueList );
        void foundDevices( const QString& device, RemoteDevice::ValueList );
        void connections( ConnectionState::ValueList );
	void signalStrength( const QString& mac, const QString& strengh );

    private:
        int Manager::extractGroups(UUID *groupToExtract, SDPServerHandle srvHandle,  
				   Services::ValueList& listOfServices );
	ConnectionState::ValueList parseConnections( const QString& );

        OProcess *m_hcitool;
        OProcess *m_sdp; // not only one
        QString m_device;
	QMap<QString, int> m_signalStrength;
        QMap<QString, QString> m_out;
        QMap<QString, QString> m_devices;
	QMap<OProcess*, QString> m_signalMac;
        QString m_hcitoolCon;
    };


    class PluginInterface: public Manager_PluginInterface {

    public:
	    PluginInterface(){
		    qWarning("Affix_PluginInterface: c'tor");
		    affix_plugin = new Manager();
	    }

	    virtual ~PluginInterface(){
		    qWarning("Affix_PluginInterface: d'tor");
		    delete affix_plugin;
		    affix_plugin = 0l;
	    }

	    /** This MACRO implements a simple reference counter, which just 
	     * deletes this class if it is released and not referenced by anyone.
	     * It also implements the two abstract methods addRef() and release(),
	     * which are defined by QUnknownInterface (our base class).
	     */
	    Q_REFCOUNT

	    /** Returns the pointer to this class if the given QUuid is correct
	     */
	    QRESULT queryInterface( const QUuid& uuid, QUnknownInterface** iface ){
		    *iface = 0l;
		    if ( uuid == IID_QUnknown ) {
			    *iface = this, (*iface)->addRef();
			    return QS_OK;
		    }
		    return QS_FALSE;
	    }

	    // Return pointer to the manager class
	    Manager_Base* manager()
	    {
		    qWarning("Affix_PluginInterface: manager()");
		    return affix_plugin;    
	    }

    private:
	    Manager* affix_plugin;
	    // ulong ref;
    }; 

};

#endif
