#include <resources.h>
#include "ppprun.h"

PPPRun::PPPRun( ANetNodeInstance * NNI, PPPData & Data ) :
          RuntimeInfo( NNI ), Pat( "eth[0-9]" ) { 
      D = &Data; 
}

State_t PPPRun::detectState( void ) {
    if( isMyPPPDRunning( ) ) {
      return ( isMyPPPUp() ) ? IsUp : Available;
    } 
    return Off;
}

QString PPPRun::setMyState( NetworkSetup * , Action_t , bool ) { 
    return QString();
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
        if( Run->assignedToNetworkSetup() == netNode()->networkSetup() ) {
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

bool PPPRun::handlesInterface( InterfaceInfo * I ) {
    return handlesInterface( I->Name );
}
