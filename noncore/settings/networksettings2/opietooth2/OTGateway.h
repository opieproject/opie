#ifndef OTGATEWAY_H
#define OTGATEWAY_H

#include <qobject.h>
#include <qvector.h>
#include <qmap.h>

#include <OTDriverList.h>
#include <OTInquiry.h>

class QPixmap;

namespace Opietooth2 {

class OTDriverList;
class OTDriver;
class OTDevice;
class OTPeer;
class OTInquiry;
class OTPANConnection;
class OTLinkKey;

typedef QVector<OTPeer> PeerVector;
typedef QVector<OTPANConnection> PANConnectionVector;
typedef QArray<OTLinkKey> LinkKeyArray;

class OTLinkKey {

public :

    OTLinkKey( const OTDeviceAddress & F,
               const OTDeviceAddress & T ) {
      From = F;
      To = T;
    }

    const OTDeviceAddress & to()
      { return To; }
    const OTDeviceAddress & from()
      { return From; }

    OTDeviceAddress From;
    OTDeviceAddress To;
};

class OTPANConnection {

public :

    OTPANConnection( const QString & Dev, const QString & CT ) {
      Device = Dev;
      ConnectedTo = CT;
    }

    QString Device;
    QString ConnectedTo;
};


class OTGateway : public QObject {

      Q_OBJECT

public :

      // single instance
      static OTGateway * getOTGateway( void );
      static void releaseOTGateway( void );
      // convert device type as class to name for that class
      static const char * deviceTypeToName( int Cls );

      // open bluetooth system
      OTGateway( void );
      // close bluetooth system
      ~OTGateway( void );

      // get access to system device
      OTDevice * getOTDevice();

      // return true if this device needs enabling of bluetooth
      bool needsEnabling();
      // return true if system is running
      bool isEnabled();
      void setRefreshTimer( int MilleSecs );
      // return socket to HCI raw layer
      inline int getSocket()
        { return HciCtl; }

      OTDriverList & getDriverList()
        { return AllDrivers; }
      OTDriver * driver( int nr ) 
        { return AllDrivers.count() == 0 ? 0 : AllDrivers[nr]; }
      void updateDrivers();

      PANConnectionVector getPANConnections();

      // scan neighbourhood using device
      void scanNeighbourhood( OTDriver * D = 0 );
      void stopScanOfNeighbourhood(void );
      void setScanWith( OTDriver * D = 0 ) 
        { ScanWith = (D) ? D : 
            (AllDrivers.count() ) ? AllDrivers[0] : 0; }
      OTDriver * scanWith( void )
        { return ScanWith; }

      // get list of all detected peers
      inline const PeerVector & peers( void )
        { return AllPeers; }
      // ping peer to see if it is up
      bool isPeerUp( const OTDeviceAddress & PAddr,
                     int timeoutInSec = 1,
                     int timeoutInUSec = 0,
                     int retry = 1 );
      OTPeer * findPeer( const OTDeviceAddress & Addr );
      void removePeer( OTPeer * P );
      void addPeer( OTPeer * P );

      OTDriver * findDriver( const OTDeviceAddress & Addr );

      inline const LinkKeyArray & getLinkKeys() const
        { return AllKeys; }
      bool removeLinkKey( unsigned int index );

      // return device number if we are connected over any device
      // to the channel
      // else returns -1
      int connectedToRFCommChannel( const OTDeviceAddress & Addr, int channel );
      int getFreeRFCommDevice( void );
      // return 0 if properly released
      int releaseRFCommDevice( int DevNr );

public slots :

      // start bluetooth system
      void SLOT_SetEnabled( bool );
      void SLOT_Enable();
      void SLOT_Disable();

      // show error
      void SLOT_ShowError( const QString & );

      void SLOT_Enabled( int, bool );
      void SLOT_DriverDisappeared( OTDriver * );
      void SLOT_PeerDetected( OTPeer *, bool  );
      void SLOT_FinishedDetecting();

signals :

      // any error
      void error( const QString & );

      // signal state of bluetooth driver
      void stateChange( OTDriver * D, bool State );

      // sent when list of drivers changees
      void driverListChanged();

      // sent when bluetooth on device is enabled
      void deviceEnabled( bool );
      
      // sent when a (new if bool = TRUE) peer is detected
      void detectedPeer( OTPeer *, bool );

      // end of detection process
      void finishedDetecting();

protected :

      void connectNotify( const char * Signal );
      void disconnectNotify( const char * Signal );

      void timerEvent( QTimerEvent * );

private :

      void loadActiveConnections( void );
      void loadKnownPeers( void );
      void saveKnownPeers( void );
      bool isConnectedTo( int devid,
                          const OTDeviceAddress & Address );

      void readLinkKeys();

      static OTGateway * SingleGateway;
      static int UseCount;

      OTDriver *      ScanWith;
      OTDriverList    AllDrivers;
      OTDevice *      TheOTDevice;
      int             HciCtl;
      int             ErrorConnectCount;
      int             RefreshTimer;
      OTInquiry *     Scanning;
      bool            AllPeersModified;
      PeerVector      AllPeers;
      LinkKeyArray    AllKeys;
};
}

#endif
