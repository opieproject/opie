#include <resources.h>
#include "ppprun.h"

PPPRun::PPPRun( ANetNodeInstance * NNI, PPPData & Data ) :
          AsConnection( NNI ), AsDevice( NNI ), Pat( "eth[0-9]" ) { 
    D = &Data; 
}

void PPPRun::detectState( NodeCollection * NC ) {
    if( isMyPPPDRunning( ) ) {
      if( isMyPPPUp() ) {
        NC->setCurrentState( IsUp );
      } else {
        NC->setCurrentState( Available );
      }
    } else {
      NC->setCurrentState( Off ); // at least this
      // but could also be unavailable
      AsDevice::netNode()->nextNode()->runtime()->detectState( NC ); 
    }
}

bool PPPRun::setState( NodeCollection * NC, Action_t A, bool ) { 
    switch( A ) {
      case Activate :
        NC->setCurrentState( Available );
        // no
        break;
      case Deactivate :
        if( NC->currentState() == IsUp ) {
          NC->state( Down );
        }
        // cannot really disable
        NC->setCurrentState( Available );
        break;
      case Up :
        if( NC->currentState() != IsUp ) {
          // start my PPPD
          NC->setCurrentState( IsUp );
        } 
        break;
      case Down :
        if( NC->currentState() == IsUp ) {
          // stop my PPPD
          NC->setCurrentState( Available );
        } 
        break;
      default : // FT
        break;
    }
    return 1;
}

bool PPPRun::isMyPPPDRunning( void ) {
    return 0;
}

bool PPPRun::isMyPPPUp( void ) {
    System & S = NSResources->system();
    InterfaceInfo * Run;
    QRegExp R( "ppp[0-9]" );

    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      Run = It.current();
      if( R.match( Run->Name ) >= 0 &&
          Run->IsPointToPoint 
        ) {
        // this is a LAN card
        if( Run->assignedNode() == AsDevice::netNode() ) {
          // assigned to us
          return 1;
        } 
      }
    }
    return 0;
}

bool PPPRun::handlesInterface( const QString & S ) {
    return Pat.match( S ) >= 0;
}
