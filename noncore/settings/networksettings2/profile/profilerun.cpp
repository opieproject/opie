#include <resources.h>

#include "profilerun.h"

State_t ProfileRun::detectState( void ) { 
    InterfaceInfo * II = networkSetup()->assignedInterface();

    Log(( "Interface %p %p %p: %d\n", II, 
            netNode(), netNode()->networkSetup(), (II) ? II->IsUp : 0 ));
    if( Data->Disabled ) {
      return Disabled;
    } 
    return Unknown;
}

QString ProfileRun::setMyState( NetworkSetup * NC, Action_t A, bool ) { 
    odebug << "Profile " << Data->Disabled << oendl;
    if( A == Disable ) {
      if( ! Data->Disabled ) {
        Data->Disabled = 1;
        NC->setModified( 1 );
      }
    } else if( A == Enable ) {
      if( Data->Disabled ) { 
        Data->Disabled = 0;
        NC->setModified( 1 );
      }
    }

    return QString();
}
