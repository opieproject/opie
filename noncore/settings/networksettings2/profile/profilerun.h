#ifndef PROFILERUN_H
#define PROFILERUN_H

#include <asfullsetup.h>
#include "profiledata.h"

class ProfileRun  : public AsFullSetup {

public :

      ProfileRun( ANetNodeInstance * NNI, ProfileData & D ) : 
            AsFullSetup( NNI )
        { Data = &D;  }

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A );
      bool canSetState( State_t Curr, Action_t A );

      bool handlesInterface( const QString & I );

      virtual const QString & description( void )
        { return Data->Description; }
private :

      ProfileData * Data;

};
#endif
