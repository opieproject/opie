#ifndef PROFILERUN_H
#define PROFILERUN_H

#include <netnode.h>
#include "profiledata.h"

class ProfileRun  : public RuntimeInfo {

public :

      ProfileRun( ANetNodeInstance * NNI, ProfileData & D ) : 
            RuntimeInfo( NNI )
        { Data = &D;  }

      virtual RuntimeInfo * fullSetup( void ) 
        { return this; }
      virtual const QString & description( void )
        { return Data->Description; }
      virtual bool triggersVPN( void ) 
        { return Data->TriggerVPN; }

      State_t detectState( void );

protected :

      QString setMyState( NodeCollection * , Action_t, bool );

private :

      ProfileData * Data;

};
#endif
