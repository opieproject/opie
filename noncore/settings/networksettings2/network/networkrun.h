#include <netnode.h>
#include "networkdata.h"

class NetworkRun  : public RuntimeInfo {

public :

      NetworkRun( ANetNodeInstance * NNI,
                  NetworkData & Data ) : RuntimeInfo( NNI )
        { }

      virtual RuntimeInfo * connection( void )
        { return this; }

      State_t detectState( void );

protected :

      QString setMyState( NetworkSetup * , Action_t, bool );
};
