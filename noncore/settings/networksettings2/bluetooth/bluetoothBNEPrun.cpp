#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <resources.h>
#include "bluetoothBNEPrun.h"

using Opietooth2::OTGateway;
using Opietooth2::OTPANNetworkSetup;
using Opietooth2::PANNetworkSetupVector;

BluetoothBNEPRun::BluetoothBNEPRun( ANetNodeInstance * NNI, 
                  BluetoothBNEPData & D ) : 
                  RuntimeInfo( NNI ),
                  Data( D),
                  Pat( "bnep[0-6]" ) {
    OT = 0;
}

BluetoothBNEPRun::~BluetoothBNEPRun( void ) {
    if( OT ) {
      OTGateway::releaseOTGateway();
    }
}

State_t BluetoothBNEPRun::detectState( void ) { 

    /*

        need to detect 

        1. for any PAN NetworkSetup that is found if that
           PAN is connected.

           if it is connected it is not available (since we do
           not manage IP settings and we are called to detect
           the state we knwo that we do not have an UP NetworkSetup)

        2. if it not connected and we allow any NetworkSetup we
           are available or if that PAN NetworkSetup is to a device
           with a correct address

        3. if it is not connected and the address do not match or
           we do not accept any address, we are Unavailable but
           not DOWN.  I.e a new NetworkSetup could perhaps be created

    */

    if( ! OT ) {
      OT = OTGateway::getOTGateway();
    }

    if( ! OT->isEnabled() ) {
      return Unavailable;
    }

    // if there is a PAN NetworkSetup that is UP but not
    // yet configured (no ifup) the we are available
    return ( hasFreePANNetworkSetup() ) ?  Available : Unknown;
}

QString BluetoothBNEPRun::setMyState( NetworkSetup * NC, Action_t A, bool  ) {

    if( A == Activate ) {
      if( hasFreePANNetworkSetup( 1 ) ) {
        // we have now an assignedinterface
      } else {
        return QString("TODO : Start PAND");
      }

      Log(( "Assigned interface" ));
      NC->setCurrentState( Available );

      return QString();
    }

    if( A == Deactivate ) {
      // nothing to do 
      NC->setCurrentState( Off );
      return QString();
    }
    return QString();
}

bool BluetoothBNEPRun::handlesInterface( const QString & S ) {
    return Pat.match( S ) >= 0;
}

bool BluetoothBNEPRun::handlesInterface( InterfaceInfo * I ) {
    return handlesInterface( I->Name );
}

bool BluetoothBNEPRun::hasFreePANNetworkSetup( bool Grab ) {

    if( ! OT ) {
      OT = OTGateway::getOTGateway();
    }

    // load PAN NetworkSetups
    OTPANNetworkSetup * C;
    InterfaceInfo * Run;
    InterfaceInfo * Candidate = 0; // reuse this interface
    PANNetworkSetupVector Conns = OT->getPANNetworkSetups();
    System & Sys = NSResources->system();
    bool IsValid;

    for( unsigned int i = 0;
         i < Conns.count();
         i ++ ) {
      C = Conns[i];

      if( Data.AllowAll ) {
        // we allow all
        IsValid = 1;
      } else {
        // is this PAN NetworkSetup connecting to a Peer
        // we allow ?
        IsValid = 0;
        for ( QStringList::Iterator it = Data.BDAddress.begin(); 
              it != Data.BDAddress.end(); 
              ++ it ) {
          if( C->ConnectedTo == (*it) ) {
            // this is a NetworkSetup we could accept
            IsValid = 1;
            break;
          }
        }
      }

      if( ! IsValid ) {
        Log(("%s to %s not acceptable\n",
            C->Device.latin1(),
            C->ConnectedTo.latin1() ));
        // don't bother checking this address
        // it is not acceptable
        continue;
      }

      // is this PAN NetworkSetup available to us ?
      Run = Sys.findInterface( C->Device );

      if( Run && Run->IsUp ) {
        // this PAN NetworkSetup is up
        Log(("%s acceptable but unavailable\n",
            C->Device.latin1() ));
        // find others
        continue;
      }

      // we at least have a possible interface
      if( ! Candidate ) {
        Candidate = Run;
      }
    }

    if( Candidate ) {
      if ( Grab ) {
        netNode()->networkSetup()->assignInterface( Candidate );
      }
      return 1;
    }

    // no free PAN
    return 0;
}

