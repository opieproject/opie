#include <netnode.h>
#include "irdadata.h"

class IRDARun  : public RuntimeInfo {

public :

      IRDARun( ANetNodeInstance * NNI, 
               IRDAData & Data ) : RuntimeInfo( NNI )
        { }

      virtual RuntimeInfo * line( void ) 
        { return this; }

      virtual QString deviceFile( void )
        { return QString( "/dev/irda" ); }

      State_t detectState( void )
        { return Unknown; }

protected :

      QString setMyState( NodeCollection * , Action_t, bool )
        { return QString(); }
};
