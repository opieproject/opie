#include <resources.h>

#include "profilerun.h"

State_t ProfileRun::detectState( void ) { 

    Log(( "Profile %sabled\n", (Data->Enabled) ? "en" : "dis" ));

    if( Data->Enabled ) {
      return Unknown;
    } 
    return Disabled;
}

QString ProfileRun::setMyState( NetworkSetup * NC, Action_t A, bool ) { 
    odebug << "Profile " << Data->Enabled << oendl;
    if( A == Disable ) {
      if( Data->Enabled ) {
        Data->Enabled = 0;
        NC->setModified( 1 );
      }
    } else if( A == Enable ) {
      if( ! Data->Enabled ) { 
        Data->Enabled = 1;
        NC->setModified( 1 );
      }
    }

    return QString();
}
