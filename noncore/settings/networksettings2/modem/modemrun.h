#include <netnode.h>
#include "modemdata.h"

class ModemRun  : public RuntimeInfo {

public :

      ModemRun( ANetNodeInstance * NNI, 
                ModemData & Data ) : RuntimeInfo ( NNI )
        { }

      virtual RuntimeInfo * line( void ) 
        { return this; }

      virtual QString deviceFile( void )
        { return QString("/dev/modem"); }

      State_t detectState( void )
        { return Unknown;}

protected :

      QString setMyState( NodeCollection * , Action_t, bool )
        { return QString(); }
};
