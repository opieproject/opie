#include <system.h>
#include <netnode.h>
#include <resources.h>
#include "networkrun.h"

State_t NetworkRun::detectState( void ) { 
    InterfaceInfo * II = nodeCollection()->assignedInterface();

    Log(( "Interface %p : %d\n", II, (II) ? II->IsUp : 0 ));
    if( II && II->IsUp ) {
      // device has assigned interface
      return IsUp;
    }

    // had no interface or interface is no longer up -> release
    nodeCollection()->assignInterface( 0 );

    return Unknown;
}

QString NetworkRun::setMyState( NodeCollection * NC, Action_t A, bool ) { 
    // we handle UP and DOWN
    InterfaceInfo * II = NC->assignedInterface();

    if( A == Up ) {
      // we can bring UP if lower level is available
      QStringList SL;
      SL << "ifup"
         << QString().sprintf( "%s=%s-c%d-allowed",
                        II->Name.latin1(), II->Name.latin1(),
                        nodeCollection()->number() );
      if( ! NSResources->system().runAsRoot( SL ) ) {
        return QString("Cannot call %1").arg(SL.join(" "));
      }
      return QString();
    } 

    if( A == Down ) {
      QStringList SL;
      if( II ) {
        SL << "ifdown"
           << II->Name.latin1();
        if( !  NSResources->system().runAsRoot( SL ) ) {
          return QString( "Cannot call %1" ).arg( SL.join( " " ));
        }
      } else {
        Log(( "no interface assigned." ));
      }
    } 
    return QString();
}
