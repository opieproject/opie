#include <qapplication.h>
#include <resources.h>
#include <OTDevice.h>
#include <OTGateway.h>
#include "bluetoothRFCOMMrun.h"

using Opietooth2::OTGateway;
using Opietooth2::OTDevice;
using Opietooth2::OTDeviceAddress;

BluetoothRFCOMMRun::~BluetoothRFCOMMRun( void ) {
      if( OT ) {
        OTGateway::releaseOTGateway();
      }
}

State_t BluetoothRFCOMMRun::detectState( void ) {

      if( ! OT ) {
        OT = OTGateway::getOTGateway();
      }

      if( deviceNrOfConnection() >= 0 ) {
        return Available;
      }

      odebug << "Bluetooth " 
            << OT->isEnabled()
            << oendl;

      return ( OT->isEnabled() ) ? Off : Unavailable;
}

QString BluetoothRFCOMMRun::setMyState( NodeCollection *, 
                                      Action_t A, 
                                      bool ) {

      if( OT ) {
        OTGateway::getOTGateway();
      }

      if( A == Activate ) {
        // from OFF to Available
        RFCOMMChannel * Ch = getChannel( );
        System & Sys = NSResources->system();
 
        if( Ch ) {
          // connect to this peer
          DeviceNr = OT->getFreeRFCommDevice();
          QStringList S;

          S << "rfcomm" 
            << "bind"
            << QString().setNum( DeviceNr )
            << Ch->BDAddress
            << QString().setNum( Ch->Channel );
 
          if( Sys.runAsRoot( S ) ) {
            return QString( "Error starting %1").arg(S.join(" "));
          }

          // here rfcomm should be running -> we will detect state later
          return QString();
        } else {
          Log(( "No channel selected -> cancel\n" ));
          return QString( "No channel selected. Operation cancelled" );
        }
      }

      if( A == Deactivate ) {
        if( DeviceNr >= 0 ) {
          if( OT->releaseRFCommDevice( DeviceNr ) ) {
           return QString( "Cannot release RFCOMM connection" );
          }
          DeviceNr = -1;
        }
      }
      return QString();
}

#include <qlistbox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qdialog.h>

RFCOMMChannel * BluetoothRFCOMMRun::getChannel( void ) {

    if( Data->Devices.count() == 1 ) {
      // only one device -> return channel
      return Data->Devices[0];
    }

    RFCOMMChannel * Ch = 0;
    QDialog * Dlg = new QDialog( qApp->mainWidget(), 0, TRUE );
    QVBoxLayout * V = new QVBoxLayout( Dlg );

    QLabel * L = new QLabel( 
      qApp->translate( "BluetoothRFCOMMRun", 
                       "Select device to connect to"),
                       Dlg );
    QListBox * LB = new QListBox( Dlg );

    for( unsigned int i = 0; i < Data->Devices.count(); i ++ ) {
      LB->insertItem( QString( "%1 (%2 Chnl %3)" ).
                        arg( Data->Devices[i]->Name ).
                        arg( Data->Devices[i]->BDAddress ).
                        arg( Data->Devices[i]->Channel ) );
    }

    V->addWidget( L );
    V->addWidget( LB );

    Dlg->resize( 100, 100 );
    Dlg->move( 20, 
               (qApp->desktop()->height()-100)/2 );

    if( Dlg->exec() == QDialog::Accepted ) {
      unsigned int i = 0;
      for( i = 0; i < Data->Devices.count(); i ++ ) {
        if( LB->isSelected(i) ) {
          odebug << "Selected " << Data->Devices[i]->Name << oendl;
          Ch = Data->Devices[i];
          break;
        }
      }
    }

    delete Dlg;
    return Ch;
}

QString BluetoothRFCOMMRun::deviceFile( void ) {
    if( deviceNrOfConnection() >= 0 ) {
      OTDevice * OTD = OT->getOTDevice();
      // there is a connection
      return OTD->getRFCommDevicePattern().arg(DeviceNr);
    }
    return QString();
}

int BluetoothRFCOMMRun::deviceNrOfConnection( void ) {

    if( ! OT ) {
      OT = OTGateway::getOTGateway();
    }

    DeviceNr = -1;
    for( unsigned int i = 0; i < Data->Devices.count(); i ++ ) {
      odebug << "Check for rfcomm on " 
            << Data->Devices[i]->BDAddress
            << " "
            << Data->Devices[i]->Channel
            << oendl;
      if( ( DeviceNr = OT->connectedToRFCommChannel( 
              OTDeviceAddress( Data->Devices[i]->BDAddress ),
              Data->Devices[i]->Channel ) ) >= 0 ) {
        odebug << "Up " 
              << oendl;
        break;
      }
    }
    return DeviceNr;
}
