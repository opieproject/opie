#include <qmessagebox.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qpixmap.h>
#include <qvector.h>
#include <qpe/resource.h>

#include <opie2/odebug.h>

#include <bluezlib.h>

#include <OTDevice.h>
#include <OTDriver.h>
#include <OTInquiry.h>
#include <OTDriverList.h>
#include <OTDeviceAddress.h>
#include <OTGateway.h>

using namespace Opietooth2;

// single instance
OTGateway * OTGateway::SingleGateway = 0;
int OTGateway::UseCount = 0;

OTGateway * OTGateway::getOTGateway( void ) {
      if(SingleGateway == 0 ) {
        SingleGateway = new OTGateway();
      }

      UseCount ++;
      return SingleGateway;
}

void OTGateway::releaseOTGateway( void ) {
      UseCount --;
      if( UseCount == 0 ) {
        delete SingleGateway;
        SingleGateway = 0;
      }
}

// open bluetooth system
OTGateway::OTGateway( void ) : QObject( 0, "OTGateway" ),
                               AllDrivers( this ),
                               AllPeers() {

      ErrorConnectCount = 0;
      TheOTDevice = 0;
      Scanning = 0;
      AllPeersModified = 0;
      AllPeers.setAutoDelete( TRUE );

      if ( ( HciCtl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) {
        SLOT_ShowError( tr( "error opening hci socket" ) );
        return;
      }

      // load all known devices
      updateDrivers();

      // load all peers we have ever seen
      loadKnownPeers();

      // iterate over drivers and find active NetworkSetups
      // adding/updating peers
      loadActiveNetworkSetups();

      // check every 4 seconds the state of BT
      timerEvent(0);
      RefreshTimer = -1;
      setRefreshTimer( 4000 );

      // load known link keys
      readLinkKeys();
}

// close bluetooth system
OTGateway::~OTGateway( void ) {

      if( AllPeersModified ) {
        saveKnownPeers();
      }

      if( Scanning )
        delete Scanning;

      if( TheOTDevice )
        delete TheOTDevice;

      if( HciCtl >= 0 ) {
        ::close( HciCtl );
      }
}

void OTGateway::setRefreshTimer( int T ) {
      if( RefreshTimer != -1 ) {
        killTimer( RefreshTimer );
      }

      if( T == 0 ) 
        T = 4000;
      RefreshTimer = startTimer( T );
}

OTDevice * OTGateway::getOTDevice( ) {
      if( TheOTDevice == 0 ) {
        // load bluetooth device and check state
        TheOTDevice = new OTDevice( this );
        connect( TheOTDevice, 
                 SIGNAL( isEnabled( int, bool ) ),
                 this, 
                 SLOT( SLOT_Enabled( int, bool ) ) );

        connect( TheOTDevice, 
                 SIGNAL( error( const QString & ) ),
                 this, 
                 SLOT( SLOT_ShowError( const QString & ) ) );
      }

      return TheOTDevice;
}

// start bluetooth (if stopped)
// return TRUE if started
void OTGateway::SLOT_SetEnabled( bool Mode ) {
      if( Mode ) {
        SLOT_Enable();
        return;
      }
      SLOT_Disable();
}

void OTGateway::SLOT_Enable() {
      getOTDevice()->attach();
}

void OTGateway::SLOT_Disable() {
      getOTDevice()->detach();
}

bool OTGateway::needsEnabling() {
      return getOTDevice()->needsAttach();
}

bool OTGateway::isEnabled() {
      if( getOTDevice()->deviceNr() >= 0 &&
          AllDrivers.count() != 0 &&
          driver( getOTDevice()->deviceNr() )->isUp() )
        return TRUE;

      // else check system
      return getOTDevice()->isAttached();
}

void OTGateway::SLOT_ShowError( const QString & S ) {

      odebug << S << oendl;

      if( ErrorConnectCount > 0 ) {
        // pass error
        emit error( QString( "<p>" ) + S + "</p>" );
        return;
      }

      QMessageBox::warning( 0,
          tr("OTGateway error"),
          S );
}

void OTGateway::connectNotify( const char * S ) {
      if( S && strcmp( S, "error(const QString&)" ) == 0 ) {
        ErrorConnectCount ++;
      }
}

void OTGateway::disconnectNotify( const char * S ) {
      if( S && strcmp( S, "error(const QString&)" ) == 0 ) {
        ErrorConnectCount --;
      }
}

void OTGateway::timerEvent( QTimerEvent * ) {

      OTDriver * D;
      unsigned int oldc = AllDrivers.count();
      bool old;

      AllDrivers.update();

      if( oldc != AllDrivers.count() ) {
        updateDrivers();
      } else {
        for( unsigned int i = 0;
             i < AllDrivers.count();
             i ++ ) {
          D = AllDrivers[i];
          old = D->isUp();
          if( D->currentState() >= 0 ) {
            if( old != D->isUp() ) {
              emit stateChange( D, D->isUp() );
            }
          } else {
            // if one driver is unable to provide info
            // we refresh all devices
            updateDrivers();
            return;
          }
        }
      }
}

void OTGateway::SLOT_Enabled( int id, bool Up ) {
      odebug << "device " << id << " state " << Up << oendl;
      if( Up ) {
        // device is up -> detect it
        updateDrivers();
        if( (unsigned)id >= AllDrivers.count() ) {
          // to make sure that the driver really IS detected
          AllDrivers[id]->bringUp();
        }
      } // if DOWN device already down
      emit deviceEnabled( Up );
}

void OTGateway::updateDrivers( void ) {
      OTDriver * D;

      AllDrivers.update();

      odebug << "updated drivers. now " << AllDrivers.count() << oendl;

      // connect signals for each driver
      for( unsigned int i = 0;
           i < AllDrivers.count();
           i ++ ) {
        D = AllDrivers[i];

        connect( D, 
                 SIGNAL( error( const QString & ) ),
                 this, 
                 SLOT( SLOT_ShowError( const QString & ) )
               );

        connect( D, 
                 SIGNAL( stateChange( OTDriver *, bool ) ),
                 this, 
                 SIGNAL( stateChange( OTDriver *, bool ) )
               );

        connect( D, 
                 SIGNAL( driverDisappeared( OTDriver * ) ),
                 this, 
                 SLOT( SLOT_DriverDisappeared( OTDriver * ) )
               );
      }

      // verify main device too
      if( TheOTDevice )
        TheOTDevice->checkAttach();

      // set to default scanning hardware
      setScanWith( 0 );

      emit driverListChanged();
}

void OTGateway::SLOT_DriverDisappeared( OTDriver * D ) {
      odebug << "Driver " << D->devname() << " when offline" << oendl;
      updateDrivers();
}

void OTGateway::scanNeighbourhood( OTDriver * D ) {

      if( Scanning ) {
        stopScanOfNeighbourhood();
      }

      if( D ) {
        setScanWith( D );
      }

      Scanning = new OTInquiry( scanWith() );

      connect( Scanning,
               SIGNAL( peerFound( OTPeer *, bool )),
               this,
               SLOT( SLOT_PeerDetected( OTPeer *, bool ) )
             );
      connect( Scanning,
               SIGNAL( finished()),
               this,
               SLOT( SLOT_FinishedDetecting() )
             );

      // start scanning
      Scanning->inquire( 30.0 );
}

OTPeer* OTGateway::findPeer( const OTDeviceAddress & Addr ) {
      for( unsigned int i = 0 ; i < AllPeers.count(); i ++ ) {
        if( AllPeers[i]->address() == Addr ) {
          return AllPeers[i];
        }
      }
      return 0;
}

OTDriver* OTGateway::findDriver( const OTDeviceAddress & Addr ) {
      for( unsigned int i = 0 ; i < AllDrivers.count(); i ++ ) {
        if( AllDrivers[i]->address() == Addr ) {
          return AllDrivers[i];
        }
      }
      return 0;
}

void OTGateway::SLOT_PeerDetected( OTPeer * P, bool IsNew ) {

      if( IsNew ) {
        // new peer
        odebug << "New peer " << P->name() << oendl;
        addPeer( P );
      }

      emit detectedPeer( P, IsNew );
}

void OTGateway::addPeer( OTPeer * P ) {
      AllPeers.resize( AllPeers.size()+1);
      AllPeers.insert( AllPeers.size()-1, P );
      AllPeersModified = 1;
}

void OTGateway::removePeer( OTPeer * P ) {
      int i = AllPeers.find( P );
      if( i ) {
        AllPeers.remove( i );
        AllPeersModified = 1;
      }
}

void OTGateway::stopScanOfNeighbourhood( void ) {
      if( Scanning ) {
        delete Scanning;
        Scanning = 0;
      }
}

void OTGateway::SLOT_FinishedDetecting() {
      stopScanOfNeighbourhood();
      emit finishedDetecting();
}

const char * OTGateway::deviceTypeToName( int cls ) {
    switch ( (cls & 0x001F00) >> 8) {
      case 0x00:
        return "misc";
      case 0x01:
        return "computer";
      case 0x02:
        return "phone";
      case 0x03:
        return "lan";
      case 0x04:
        return "av";
      case 0x05:
        return "peripheral";
      case 0x06:
        return "imaging";
      case 0x07:
      default :
        break;
    }
    return "unknown";
}

PANNetworkSetupVector OTGateway::getPANNetworkSetups( void ) {
        PANNetworkSetupVector V;

        struct bnep_connlist_req req;
        struct bnep_conninfo ci[48];

        V.setAutoDelete(TRUE);

        int ctl = socket(PF_BLUETOOTH, SOCK_RAW, BTPROTO_BNEP);
        if (ctl < 0) {
          odebug << "Failed to open control socket" << oendl;
          return V;
        }

        req.cnum = 48;
        req.ci   = ci;
        if (ioctl(ctl, BNEPGETCONNLIST, &req)) {
          odebug << "Failed to get NetworkSetup list" << oendl;
          ::close( ctl );
          return V;
        }

        for ( unsigned i=0; i < req.cnum; i++) {
          V.resize( V.size() + 1 );
          if( ci[i].role == BNEP_SVC_PANU ) {
            // we are the client
            V.insert( V.size()-1, new OTPANNetworkSetup(
                            ci[i].device,
                            batostr((bdaddr_t *) ci[i].dst)
                          ) );
          }
        }

        ::close( ctl );
        return V;
}

struct link_key {
        bdaddr_t sba;
        bdaddr_t dba;
        uint8_t  key[16];
        uint8_t  type;
        time_t   time;
};

void OTGateway::readLinkKeys( void ) {

        struct link_key k;
        int rv;

        AllKeys.truncate(0);

        QFile F( "/etc/bluetooth/link_key" );

        if( ! F.open( IO_ReadOnly ) ) {
          emit error( tr("Cannot open link_key file") );
          return;
        }

        while( 1 ) {
          rv = F.readBlock( (char *)&k, sizeof( k ) );
          if( rv == 0 )
            // EOF
            break;

          if( rv < 0 ) {
            emit error( tr("Read error in link key file") );
          }

          AllKeys.resize( AllKeys.size()+1 );
          AllKeys[ AllKeys.size()-1 ].From.setBDAddr( k.sba );
          AllKeys[ AllKeys.size()-1 ].To.setBDAddr( k.dba );
        }
}

bool OTGateway::removeLinkKey( unsigned int Index ) {
        OTLinkKey & LK = AllKeys[Index];

        struct link_key k;
        int rv;

        QFile F( "/etc/bluetooth/link_key" );
        QFile OutF( "/etc/bluetooth/newlink_key" );

        if( ! F.open( IO_ReadOnly ) ) {
          emit error( tr("Cannot open link_key file") );
          return 0;
        }

        if( ! OutF.open( IO_WriteOnly | IO_Truncate ) ) {
          emit error( tr("Cannot open temporary link_key file") );
          return 0;
        }

        while( 1 ) {
          rv = F.readBlock( (char *)&k, sizeof( k ) );
          if( rv == 0 )
            // EOF
            break;

          if( rv < 0 ) {
            emit error( tr("Read error in link key file") );
            return 0;
          }

          if( LK.from() != OTDeviceAddress( k.sba ) ||
              LK.to() != OTDeviceAddress( k.dba ) ) {
            // copy
            OutF.writeBlock( (char *)&k, sizeof( k ) );
          } // else remove this key
        }

        // rename files
        QDir D( "/etc/bluetooth" );

        D.remove( "link_key" );
        D.rename( "newlink_key", "link_key" );

        // restart hcid
        system( "/etc/init.d/hcid stop" );
        system( "/etc/init.d/hcid start" );

        // remove from table
        if( Index < (AllKeys.size()-1) ) {
          // collapse array
          AllKeys[Index] = AllKeys[AllKeys.size()-1];
        }

        // remove last element
        AllKeys.resize( AllKeys.size()-1 );

        return 1;
}

#define MAXCONNECTIONS 10
void OTGateway::loadActiveNetworkSetups( void ) {

        struct hci_conn_list_req *cl;
        struct hci_conn_info *ci;
        OTDeviceAddress Addr;
        OTPeer * P;

        if (!(cl = (struct hci_conn_list_req *)malloc( 
                      MAXCONNECTIONS * sizeof(*ci) + sizeof(*cl)))) {
          emit error( tr("Can't allocate memory") );
          return;
        }
        memset( cl, 0, MAXCONNECTIONS * sizeof(*ci) + sizeof(*cl) );

        for( unsigned int i = 0;
             i < AllDrivers.count();
             i ++ ) {

          if( ! AllDrivers[i]->isUp() ) {
            continue;
          }

          // driver is up -> check NetworkSetups
          cl->dev_id = AllDrivers[i]->devId();
          cl->conn_num = MAXCONNECTIONS;
          ci = cl->conn_info;

          if (ioctl( getSocket(), HCIGETCONNLIST, (void *) cl)) {
            emit error( tr("Can't get NetworkSetup list") );
            break;
          }

          for ( int k = 0; k < cl->conn_num; k++, ci++) {

            if( ci->state != BT_CONNECTED ) {
              // not yet connected
              continue;
            }

            Addr.setBDAddr( ci->bdaddr );
            P = findPeer( Addr );
            if( ! P ) {
              // peer not yet known -> add
              P = new OTPeer( this );
              addPeer( P );
              P->setAddress( Addr );
              // infoQueue.push_back(info);
              P->setName( AllDrivers[i]->getPeerName( Addr ) ); 
            } 
            P->setState( OTPeer::Peer_Up );
            P->setConnectedTo( AllDrivers[i] );
          }
        }

        free( cl );
}

void OTGateway::loadKnownPeers( void ) {
    QDir SaveDir = QDir::home();

    if( ! SaveDir.exists( "Settings" ) ) {
      return;
    }
    SaveDir.cd( "Settings" );

    if( ! SaveDir.exists( "opietooth" ) ) {
      return;
    }
    SaveDir.cd( "opietooth" );

    QFile F( SaveDir.path() + "/SeenDevices.conf" );

    if( F.open( IO_ReadOnly ) ) {
      QTextStream TS(&F);
      long count;

      count = TS.readLine().toLong();

      while( count > 0 ) {
        addPeer( new OTPeer( TS, this ) );
        count --;
      }
    }

    AllPeersModified = 0;
}

void OTGateway::saveKnownPeers( void ) {
    QDir SaveDir = QDir::home();

    if( ! SaveDir.exists( "Settings" ) ) {
      SaveDir.mkdir( "Settings" );
    }
    SaveDir.cd( "Settings" );

    if( ! SaveDir.exists( "opietooth" ) ) {
      SaveDir.mkdir( "opietooth" );
    }
    SaveDir.cd( "opietooth" );

    QFile F( SaveDir.path() + "/SeenDevices.conf" );

    if( F.open( IO_WriteOnly | IO_Truncate ) ) {
      QTextStream TS(&F);
      QString S;

      TS << AllPeers.count() << endl;

      for( unsigned int i = 0;
           i < AllPeers.count();
           i ++ ) {
        AllPeers[i]->save( TS );
      }
      AllPeersModified = 0;
    }
    AllPeersModified = 0;
}

int OTGateway::connectedToRFCommChannel( const OTDeviceAddress & Addr, 
                                         int channel ) {

    int s;

    if( (s = ::socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_RFCOMM)) < 0 ) {
      emit error( tr("Can't open RFCOMM control socket") );
      return 0;
    }

    // get all rfcomm devices
    { struct rfcomm_dev_list_req *dl;
      struct rfcomm_dev_info *di, *dr;
      int i;

      dl = (struct rfcomm_dev_list_req *)alloca(
              sizeof(*dl) + RFCOMM_MAX_DEV * sizeof(*di));
      memset( dl, 0, sizeof(*dl) + RFCOMM_MAX_DEV * sizeof(*di) );
      dl->dev_num = RFCOMM_MAX_DEV;
      di = dl->dev_info;

      if( ::ioctl(s, RFCOMMGETDEVLIST, (void *) dl) < 0) {
        emit error( tr("Can't get device list") );
        ::close( s );
        return 0;
      }

      dr = di;
      for (i = 0; i < dl->dev_num; i++, dr++) {
        // connected to Peer
        if( Addr == OTDeviceAddress( dr->dst ) &&
            channel == dr->channel &&
            ( dr->state != 0 )
          ) {
          // return device ID
          return dr->id;
        }
      }
    }

    // no device
    return -1;
}

static int byID( struct rfcomm_dev_info * d1,
                 struct rfcomm_dev_info * d2 ) {
    return d1->id - d2->id;
}

int OTGateway::getFreeRFCommDevice( void ) {

    int s;

    if( (s = ::socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_RFCOMM)) < 0 ) {
      emit error( tr("Can't open RFCOMM control socket") );
      return 0;
    }

    // get all rfcomm devices
    { struct rfcomm_dev_list_req *dl;
      struct rfcomm_dev_info *di, *dr;
      int i;

      dl = (struct rfcomm_dev_list_req *)alloca(
              sizeof(*dl) + RFCOMM_MAX_DEV * sizeof(*di));

      dl->dev_num = RFCOMM_MAX_DEV;
      di = dl->dev_info;

      if( ::ioctl(s, RFCOMMGETDEVLIST, (void *) dl) < 0) {
        emit error( tr("Can't get device list") );
        ::close( s );
        return 0;
      }

      // s
      if( dl->dev_num ) {
        qsort( di, sizeof(struct rfcomm_dev_info), 
               dl->dev_num, (int(*)(const void*,const void*))byID );
        int id = 0;

        dr = di;
        // find lowest free device number
        for (i = 0; i < dl->dev_num; i++, dr++) {
          if( id != dr->id ) {
            return id;
          }
          id ++;
        }
        return id;
      } else {
        return 0;
      }
    }
}

int OTGateway::releaseRFCommDevice( int devnr ) {

    int s;

    if( (s = ::socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_RFCOMM)) < 0 ) {
      emit error( tr("Can't open RFCOMM control socket") );
      return 0;
    }

    // get all rfcomm devices
    { struct rfcomm_dev_list_req *dl;
      struct rfcomm_dev_info *di, *dr;
      int i;

      dl = (struct rfcomm_dev_list_req *)alloca(
              sizeof(*dl) + RFCOMM_MAX_DEV * sizeof(*di));
      memset( dl, 0, sizeof(*dl) + RFCOMM_MAX_DEV * sizeof(*di) );
      dl->dev_num = RFCOMM_MAX_DEV;
      di = dl->dev_info;

      if( ::ioctl(s, RFCOMMGETDEVLIST, (void *) dl) < 0) {
        emit error( tr("Can't get device list") );
        ::close( s );
        return 0;
      }

      dr = di;
      for (i = 0; i < dl->dev_num; i++, dr++) {
        if( dr->id == devnr ) {
          // still in NetworkSetup list 
          struct rfcomm_dev_req req;
          int err;

          memset(&req, 0, sizeof(req));
          req.dev_id = devnr;

          if ((err = ioctl(s, RFCOMMRELEASEDEV, &req)) < 0 ) {
            return err;
          }
          return 0;
        }
      }
    }

    // no device -> nothing to release eiterh
    return 0;
}

