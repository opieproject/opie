#include <system.h>
#include <asdevice.h>
#include "networkrun.h"

void NetworkRun::detectState( NodeCollection * NC ) { 
    RuntimeInfo * RI = netNode()->nextNode()->runtime();
    AsDevice * Next = RI->asDevice();
    InterfaceInfo * II = Next->assignedInterface();

    if( II ) {
      // device has assigned interface
      NC->setCurrentState( (( II->IsUp ) ? IsUp : Available) );
      return;
    }

    // has no interface -> delegate
    RI->detectState( NC );
}

bool NetworkRun::setState( NodeCollection * NC, Action_t A ) { 
    // we handle UP and DOWN
    RuntimeInfo * RI = netNode()->nextNode()->runtime();
    AsDevice * Next = RI->asDevice();
    InterfaceInfo * II = Next->assignedInterface();

    if( A == Up ) {
      // we can bring UP if lower level is available
      if( NC->currentState() == Available ) {
        QString S;
        S.sprintf( "ifup %s=%s-c%d-allowed", 
            II->Name.latin1(), II->Name.latin1(),
            connection()->number() );
        NSResources->system().runAsRoot( S );
      }
      return 1;
    } else if( A == Down ) {
      if( NC->currentState() == IsUp ) {
        QString S;
        S.sprintf( "ifdown %s=%s-c%d-allowed", 
            II->Name.latin1(), II->Name.latin1(),
            connection()->number() );
        NSResources->system().runAsRoot( S );
      }
      return 1;
    } 
    // delegate
    return RI->setState( NC, A );
}

bool NetworkRun::canSetState( State_t Curr, Action_t A ) {
    // we handle UP and DOWN
    RuntimeInfo * RI = netNode()->nextNode()->runtime();

    if( A == Up ) {
      return ( Curr == Available );
    } else if( A == Down ) {
      return ( Curr == IsUp );
    } 
    // delegate
    return RI->canSetState( Curr, A );
}

bool NetworkRun::handlesInterface( const QString & S ) {
    // donno -> pass deeper
    return netNode()->nextNode()->runtime()->handlesInterface(S);
}
