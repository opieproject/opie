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

    if( ! II ) {
      Log(( "no interface assigned." ));
      return QString();
    }

    QStringList SL;

    if( A == Up ) {
      // we can bring UP if lower level is available
      SL << "ifup";
    } else if( A == Down ) {
      SL << "ifdown";
    } else {
      return QString();
    }

    SL << QString().sprintf( "%s=%s-c%d-allowed",
                        II->Name.latin1(), II->Name.latin1(),
                        nodeCollection()->number() );

    if( ! NSResources->system().runAsRoot( SL ) ) {
      return QString("Cannot call %1").arg(SL.join(" "));
    }

    return QString();
}
