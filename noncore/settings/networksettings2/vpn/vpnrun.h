#ifndef VPNRUN_H
#define VPNRUN_H

#include <netnode.h>
#include "vpndata.h"

class VPNRun  : public RuntimeInfo {

public :

      VPNRun( ANetNodeInstance * NNI, VPNData & Data ) : 
              RuntimeInfo( NNI )
        { }

      virtual RuntimeInfo * connection( void ) 
        { return this; }

      State_t detectState( void )
        { return Unknown; }

protected :

      QString setMyState( NetworkSetup * , Action_t, bool )
        { return QString(); }
};

#endif
