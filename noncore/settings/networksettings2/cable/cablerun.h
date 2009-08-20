#include <netnode.h>
#include "cabledata.h"

class CableRun  : public RuntimeInfo {

public :

      CableRun( ANetNodeInstance * NNI,
                CableData & D ) : RuntimeInfo( NNI )
        { Data = &D; }

      virtual RuntimeInfo * line( void )
        { return this; }

      virtual QString deviceFile( void );

      State_t detectState( void );

protected :

      QString setMyState( NetworkSetup * , Action_t, bool );

private :

      CableData * Data;
};
