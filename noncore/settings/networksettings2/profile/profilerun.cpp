#include <resources.h>

#include "profilerun.h"

void ProfileRun::detectState( NodeCollection * NC ) { 
    if( Data->Disabled ) {
      NC->setCurrentState( Disabled );
    } else {
      // find next item in connection
      // convert to runtime and ask to detect the state
      netNode()->nextNode()->runtime()->detectState( NC );
    }
}

bool ProfileRun::setState( NodeCollection * NC, Action_t A ) { 
    ANetNodeInstance * NNNI;

    NNNI = netNode()->nextNode();
    switch ( A ) {
      case Enable :
        if( NC->currentState() == Disabled ) {
          Data->Disabled = 0;
          NC->setCurrentState( Off ); // at least
          // ... but request deeper 
          NNNI->runtime()->detectState(NC);
        }
        return 1;
      case Disable :
        switch( NC->currentState() ) {
          case IsUp :
          case Available :
            // bring Deactivate (will bring down) 
            if( ! NNNI->runtime()->setState(NC, Deactivate) )
              return 0;
          default :
            break;
        }
        Data->Disabled = 1;
        NC->setCurrentState( Disabled );
        return 1;
      default :
        break;
    }
    return NNNI->runtime()->setState(NC, A);
}

bool ProfileRun::canSetState( State_t Curr, Action_t A ) { 
    RuntimeInfo * RI;
    switch ( A ) {
      case Enable :
      case Disable :
        // always possible
        return 1;
      default :
        break;
    }
    RI = netNode()->nextNode()->runtime();
    return ( Curr != Disabled ) ?
        RI->canSetState(Curr, A) : 0;
}

bool ProfileRun::handlesInterface( const QString & S ) {
    // donno -> pass deeper
    return netNode()->nextNode()->runtime()->handlesInterface(S);
}
