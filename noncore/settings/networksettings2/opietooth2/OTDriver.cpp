/***************************************************************************
 *   Copyright (C) 2003 by Mattia Merzi                                    *
 *   ottobit@ferrara.linux.it                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <opie2/odebug.h>
#include <qtimer.h>

#include <sys/poll.h>
#include <bluezlib.h>

#include <OTGateway.h>
#include <OTDriver.h>
#include <OTHCISocket.h>

using namespace Opietooth2;

static struct {
    const char  *str;
    unsigned short rev;
} csr_map[] = {
    { "HCI 11.2 (bc01b)",   114 },
    { "HCI 11.3 (bc01b)",   115 },
    { "HCI 12.1 (bc01b)",   119 },
    { "HCI 12.3 (bc01b)",   134 },
    { "HCI 12.7 (bc01b)",   188 },
    { "HCI 12.8 (bc01b)",   218 },
    { "HCI 12.9 (bc01b)",   283 },
    { "HCI 13.10 (bc01b)",  309 },
    { "HCI 13.11 (bc01b)",  351 },
    { "HCI 16.4 (bc01b)",   523 },
    { "HCI 14.3 (bc02x)",   272 },
    { "HCI 14.6 (bc02x)",   336 },
    { "HCI 14.7 (bc02x)",   373 },
    { "HCI 14.8 (bc02x)",   487 },
    { "HCI 15.3 (bc02x)",   443 },
    { "HCI 16.4 (bc02x)",   525 },
    { NULL, 0}
};

static char *services[] = { "Positioning",
                            "Networking",
                            "Rendering",
                            "Capturing",
                            "Object Transfer",
                            "Audio",
                            "Telephony",
                            "Information" };

static char *major_devices[] = { "Miscellaneous",
                                 "Computer",
                                 "Phone",
                                 "LAN Access",
                                 "Audio/Video",
                                 "Peripheral",
                                 "Imaging",
                                 "Uncategorized" };


typedef struct {
    short Minor;
    const char * Description;
} ClassMap_t;

static ClassMap_t MapMiscClass[] = {
    { -1, "" }
};

static ClassMap_t MapUnclassifiedClass[] = {
    { -1, "" }
};

static ClassMap_t MapComputerClass[] = {
    { 0, "Uncategorized" } ,
    { 1, "Desktop workstation" } ,
    { 2, "Server" } ,
    { 3, "Laptop" } ,
    { 4, "Handheld" } ,
    { 5, "Palm" } ,
    { 6, "Wearable" },
    { -1, 0 }
};

static ClassMap_t MapPhoneClass[] = {
    { 0, "Uncategorized" },
    { 1, "Cellular" },
    { 2, "Cordless" },
    { 3, "Smart phone" },
    { 4, "Wired modem or voice gateway" },
    { 5, "Common ISDN Access" },
    { 6, "Sim Card Reader" },
    { -1, 0 }
};

static ClassMap_t MapAVClass[] = {
    { 0, "Uncategorized" },
    { 1, "Device conforms to the Headset profile" },
    { 2, "Hands-free" },
    { 3, 0 },
    { 4, "Microphone" },
    { 5, "Loudspeaker" },
    { 6, "Headphones" },
    { 7, "Portable Audio" },
    { 8, "Car Audio" },
    { 9, "Set-top box" },
    { 10, "HiFi Audio Device" },
    { 11, "VCR" },
    { 12, "Video Camera" },
    { 13, "Camcorder" },
    { 14, "Video Monitor" },
    { 15, "Video Display and Loudspeaker" },
    { 16, "Video Conferencing" },
    { 17, 0 },
    { 18, "Gaming/Toy" },
    { -1, 0 }
};

static ClassMap_t MapPeripheralClass[] = {
    { 16, "Keyboard" },
    { 32, "Pointing device" },
    { 48, "Combo keyboard/pointing device" },
    { -1, 0 }
};

typedef struct {
    int Major;
    ClassMap_t * Map;
} MainClassMap_t;

static MainClassMap_t MainClasses[] = {
    { 0, MapMiscClass },
    { 1, MapComputerClass },
    { 2, MapPhoneClass },
    { 3, 0 }, // special case
    { 4, MapAVClass },
    { 5, MapPeripheralClass },
    { 6, 0 }, // special case
    { 63, MapUnclassifiedClass },
    { -1, 0 }
};

OTDriver::OTDriver( OTGateway * _OT, struct hci_dev_info* di) : QObject( _OT ), Address() {
    OT = _OT;
    IsUp = 0;
    Socket = 0;

    init(di);
    owarn << "Driver " << devname() << oendl;

    AutoClose = new QTimer( this );
    connect( AutoClose,
             SIGNAL( timeout() ),
             this,
             SLOT( SLOT_CloseFd() )
           );
}

OTDriver::~OTDriver() {
    closeSocket();
    SLOT_CloseFd();
}

void OTDriver::SLOT_CloseFd( void ){
    if ( isOpen() ) {
      AutoClose->stop();
      ::close( fd() );
      setfd( -1 );
    }
}

void OTDriver::init(struct hci_dev_info* di) {

    Dev = di->name;

    setDevId(di->dev_id);
    setType(di->type);
    setFlags(di->flags);
    Address.setBDAddr( di->bdaddr );
    setFeatures(di->features);
    setfd( -1 ); // not open

    Manufacturer = "";
}

// internal reinitialize
void OTDriver::reinit() {
    bool Old;
    Old = IsUp;

    if( currentState() < 0 )
      return;

    if( Old != IsUp ) {
      // state changes
      emit stateChange( this, IsUp );
    }
}

// requested by application
int OTDriver::currentState() {
    struct hci_dev_info di;

    //  uint16_t tmp_dev_id = device_info.dev_id;
    //  bzero(&device_info,sizeof(struct hci_dev_info));
    //  device_info.dev_id = tmp_dev_id;

    memset( &di, 0, sizeof( di ) );
    di.dev_id = Dev_id;
    if( ioctl( OT->getSocket(), HCIGETDEVINFO, (void*)&di) < 0 ) {
      SLOT_CloseFd();
      return -1;
    } else {
      // load new info
      init(&di);
    }

    return IsUp;
}

bool OTDriver::open() {

    // (re)start single shot close
    AutoClose->start( 30000, TRUE );

    if( isOpen() )
      // is open
      return 1;

    setfd(hci_open_dev(devId()));

    if (fd() < 0) {
      emit error( tr( "Can't open device %1. %2 : %3" ).
                      arg( devname() ).
                      arg( errno ).
                      arg( strerror(errno) )
                );
      return 0;
    }

    return 1;
}

QString OTDriver::name() {
    char name[1000];

    if( ! open() ) {
      return tr("Cannot open");
    }

    if (hci_read_local_name( fd(), sizeof(name), name, 1000) < 0) {
      if (errno != ETIMEDOUT) {
        emit error( tr("Can't read local name on %1. %2 : %3.  Default to %4" ).
            arg( devname() ).
            arg( errno  ).
            arg( strerror(errno) ).
            arg( devname() )
          );
      } // ETIMEDOUT error is quite normal, device is down ... I think ! :)
      strcpy(name,devname().latin1());
    }
    return QString(name);
}

void OTDriver::setFlags(unsigned long flags) {

    //  kdDebug() << "Setting OTDriver Values ..." << endl;
    IsUp = BTVALUE(hci_test_bit(HCI_UP, &flags));

    if (isUp()) {
        setIScan(BTVALUE(hci_test_bit(HCI_ISCAN, &flags)));
        setPScan(BTVALUE(hci_test_bit(HCI_PSCAN, &flags)));
        setAuthentication(BTVALUE(hci_test_bit(HCI_AUTH, &flags)));
        setEncryption(BTVALUE(hci_test_bit(HCI_ENCRYPT, &flags)));
    } else {
        setIScan(BT_UNKNOWN);
        setPScan(BT_UNKNOWN);
        setAuthentication(BT_UNKNOWN);
        setEncryption(BT_UNKNOWN);
    }
}

QString OTDriver::revision() {

    struct hci_version ver;

    if( ! open() ) {
      return tr("Cannot open");
    }

    if (hci_read_local_version(fd(), &ver, 1000) < 0) {
      emit error( tr( "Can't read revision info on %1. %2 : %3" ).
          arg( devname() ).
          arg( errno ).
          arg( strerror(errno) ) );
      return QString();
    }

    setManufacturer(ver.manufacturer);

    switch (ver.manufacturer) {
      case 0:
        return getRevEricsson();
        break;
      case 10:
        return getRevCsr(ver.hci_rev);
        break;
      default:
        return tr( "Unsupported manufacturer" );
        break;
    }
}

QString OTDriver::getRevEricsson() {

    char revision[102];
    struct hci_request rq;

    if( ! open() ) {
      return QString( "Cannot open" );
    }

    memset(&rq, 0, sizeof(rq));
    rq.ogf = 0x3f;
    rq.ocf = 0x000f;
    rq.cparam = NULL;
    rq.clen = 0;
    rq.rparam = &revision;
    rq.rlen = sizeof(revision);

    if (hci_send_req(fd(), &rq, 1000) < 0) {
      emit error( tr( "Can't read revision info on %1. %2 : %3" ).
          arg( devname() ).
          arg( errno ).
          arg( strerror(errno) ) );
      return QString();
    }

    return QString( revision+1 );
}

QString OTDriver::getRevCsr( unsigned short rev) {

    int i;

    for (i = 0; csr_map[i].str; i++)
      if (csr_map[i].rev == rev) {
        return QString( csr_map[i].str );
      }

    return tr( "Unknown firmware" );
}

int OTDriver::reset() {

    if( ! open() ) {
      return 0;
    }

    if( ioctl(fd(), HCIDEVRESET, devId()) < 0 ) {
      if( errno != EALREADY ) {
        emit error( tr( "Reset failed for %1. %2 : %3" ).
            arg( devname() ).
            arg( errno ).
            arg( strerror(errno) ) );
        if (errno == EACCES) {
          return EACCES;
        }
      }
    }
    return 0;
}

void OTDriver::setUp( bool M )  {
    if( M && ! isUp() ) {
      bringUp();
    } else if( ! M && isUp() ) {
      bringDown();
    }
}

void OTDriver::bringUp() {

    owarn << "bringUp : " << Dev << oendl;

    if( ! open() ) {
      return;
    }

    if (! isUp()) {
      if( ioctl(fd(), HCIDEVUP, devId()) < 0 ) {
        if( errno != EALREADY ) {
          emit error( tr( "Cannot bring interface %1 up. %2 : %3" ).
              arg( devname() ).
              arg( errno ).
              arg( strerror(errno) ) );
        }
        return;
      }
      // have to wait a bit for the 'up' to become active
      QTimer::singleShot( 3000, this, SLOT( reinit() ) );
    }
}

void OTDriver::bringDown() {

    owarn << "bringDown : " << Dev << oendl;

    if( ! open() ) {
      return;
    }

    if ( isUp() ) {
      if( ioctl(fd(), HCIDEVDOWN, devId()) < 0 ) {
        if( errno != EALREADY ) {
          emit error( tr( "Cannot bring interface %1 down. %2 : %3" ).
              arg( devname() ).
              arg( errno ).
              arg( strerror(errno) ) );
        }
        return;
      }
      reinit();
    }
}

void OTDriver::setScanMode(bool iscan, bool pscan) {

    struct hci_dev_req dr;

    if( ! open() ) {
      return;
    }

    dr.dev_id  = devId();
    dr.dev_opt = SCAN_DISABLED;

    if( iscan&&(!pscan) )
      dr.dev_opt = SCAN_INQUIRY;
    else if( pscan&&(!iscan) )
      dr.dev_opt = SCAN_PAGE;
    else if( pscan&&iscan )
      dr.dev_opt = SCAN_PAGE | SCAN_INQUIRY;

    if( ioctl(fd(), HCISETSCAN, (unsigned long)&dr) < 0 ) {
      if( errno != EALREADY ) {
        emit error( tr( "Can't set scan mode on %1. %2 : %3" ).
            arg( devname() ).
            arg( errno ).
            arg( strerror(errno) ) );
      }
      return;
    }

    reinit();
}

void OTDriver::changeDevName(const char* name) {

    if( ! open() ) {
      return;
    }

    if (hci_write_local_name(fd(), name, 1000) < 0) {
      emit error( tr( "Can't change local name on %1. %2 : %3" ).
          arg( devname() ).
          arg( errno ).
          arg( strerror(errno) ) );
    }
}

void OTDriver::changeAuthentication(bool _auth) {
    struct hci_dev_req dr;

    if( ! open() ) {
      return;
    }

    dr.dev_id  = devId();
    dr.dev_opt = _auth?AUTH_ENABLED:AUTH_DISABLED;
   
    if (ioctl(fd(),HCISETAUTH,(unsigned long)&dr) < 0) {
      if( errno != EALREADY ) {
        emit error( tr( "Can't change authentication on %1. %2 : %3" ).
            arg( devname() ).
            arg( errno ).
            arg( strerror(errno) ) );
      }
      return;
    }
    reinit();
}

void OTDriver::changeEncryption(bool _encrypt) {
    struct hci_dev_req dr;

    if( ! open() ) {
      return;
    }

    dr.dev_id  = devId();
    dr.dev_opt = _encrypt?ENCRYPT_P2P:ENCRYPT_DISABLED;
        
    if (ioctl(fd(),HCISETENCRYPT,(unsigned long)&dr) < 0) {
      if( errno != EALREADY ) {
        emit error( tr( "Can't change encryption on %1. %2 : %3" ).
            arg( devname() ).
            arg( errno ).
            arg( strerror(errno) ) );
      }
      return;
    }

    reinit();
}

void OTDriver::changeClass ( unsigned char service,  
                             unsigned char major,
                             unsigned char minor ) {
    unsigned long cod = 0;
    cod = (service << 16) |
          (major   <<  8) |
          (minor        ) ;

    if( ! open() ) {
      return;
    }

    if ( hci_write_class_of_dev(fd(),cod,1000) < 0 ) {
      emit error( tr( "Can't change class informations for %1. %2 : %3" ).
          arg( devname() ).
          arg( errno ).
          arg( strerror(errno) ) );
      return;
    }
}

void OTDriver::getClass( QString & service,  
                         QString & device ) {
    unsigned char cls[3];

    if( ! open() ) {
      return;
    }

    if ( hci_read_class_of_dev(fd(),cls,1000) < 0 ) {
      emit error( tr( "Can't read class information for %1. %2 : %3" ).
          arg( devname() ).
          arg( errno ).
          arg( strerror(errno) ) );
      return;
    }

    if( cls[2] ) {
      int first = 1;
      for ( unsigned int s = 0; s < sizeof(*services); s++) {
        if (cls[2] & (1 << s)) {
          if( !first )
            service += ", ";
          service += services[s];
          first = 0;
        }
      }
    } else {
      service = "unspecified";
    }

    MainClassMap_t * MCM = MainClasses;
    int major = cls[1] & 0x1f;
    int minor = cls[0] >> 2;

    if( (unsigned)(cls[1] & 0x1f) > sizeof(*major_devices)) {
      device = tr("Invalid Device Class");
      return;
    }

    device = major_devices[cls[1] & 0x1f];

    while( MCM->Major != -1 ) {
      if( major == MCM->Major ) {
        // this class
        ClassMap_t * CM = MCM->Map;
        if( MCM->Map ) {
          while( CM->Minor != -1 ) {
            if( minor == CM->Minor ) {
              break;
            }
            CM ++;
          }
          device = CM->Description;
        } else {
          // special case
          if( major == 3 ) {
            /* lan access */
            if( minor == 0 ) {
              device = "Uncategorized";
            } else {
              switch( minor / 8 ) {
                case 0:
                  device = "Fully available";
                  break;
                case 1:
                  device = "1-17% utilized";
                  break;
                case 2:
                  device = "17-33% utilized";
                  break;
                case 3:
                  device = "33-50% utilized";
                  break;
                case 4:
                  device = "50-67% utilized";
                  break;
                case 5:
                  device = "67-83% utilized";
                  break;
                case 6:
                  device = "83-99% utilized";
                  break;
                case 7:
                  device = "No service available";
                  break;
              }
            }
          } else if( major == 6 ) { /* imaging */
            if (minor & 4)
              device = "Display";
            if (minor & 8)
              device = "Camera";
            if (minor & 16)
              device = "Scanner";
            if (minor & 32)
              device = "Printer";
          }
        }
        break;
      }
      MCM ++;
    }

    if( MCM->Major == -1 ) {
      device = "Unknown (reserved) minor device class";
    }
}

QString OTDriver::strType() {
    return QString( hci_dtypetostr(Type) );
}

void OTDriver::setFeatures( unsigned char * _f) {
    Features = lmp_featurestostr(_f, NULL, 255);
}

void OTDriver::setManufacturer(int compid) {
    Manufacturer = bt_compidtostr(compid);
}

OTHCISocket * OTDriver::openSocket( void ) {
    if( ! Socket ) {
      owarn << "Open HCI socket to " << devname() << oendl;
      Socket = new OTHCISocket( this );
    }
    return Socket;
}

void OTDriver::closeSocket( void ) {
    if( Socket ) {
      owarn << "Close HCI socket to " << devname() << oendl;
      delete Socket;
      Socket = 0;
    }
}

QString OTDriver::getPeerName( const OTDeviceAddress & PAddr ) {
      QString S;
      char name[100 ];

      if( ! open() ) {
        return QString("N/A");
      }

      if( hci_read_remote_name( fd(), 
                                &(PAddr.getBDAddr()), 
                                sizeof(name), 
                                name, 
                                100000 ) < 0 ) {
        return QString( "N/A" );
      }

      return QString( name );
}

long OTDriver::getLinkQuality( const OTDeviceAddress & Addr ) {
      struct hci_conn_info_req *cr;
      struct hci_request rq;
      read_rssi_rp rp;
      uint16_t handle;

      if( ! open() ) {
        return 0;
      }

      cr = (struct hci_conn_info_req *)malloc(
                sizeof(*cr) + sizeof(struct hci_conn_info));
      if (!cr)
        return 0;

      bacpy( &(cr->bdaddr), &(Addr.getBDAddr()) );
      cr->type = ACL_LINK;

      if (ioctl( fd(), HCIGETCONNINFO, (unsigned long) cr) < 0) {
        owarn << "Get connection info failed" << oendl;
        free(cr);
        return 0;
      }

      handle = htobs(cr->conn_info->handle);

      free(cr);

      memset(&rq, 0, sizeof(rq));
      rq.ogf    = OGF_STATUS_PARAM;
      rq.ocf    = OCF_READ_RSSI;
      rq.cparam = &handle;
      rq.clen   = 2;
      rq.rparam = &rp;
      rq.rlen   = GET_LINK_QUALITY_RP_SIZE;

      if (hci_send_req( fd(), &rq, 100) < 0) {
         owarn << "Get connection info failed" << oendl;
         return 0;
      }

      if( rp.status ) {
        owarn << QString().sprintf("HCI get_link_quality cmd failed (0x%2.2X)", rp.status) << oendl;
        return 0;
      }

      return rp.rssi+50;
}
