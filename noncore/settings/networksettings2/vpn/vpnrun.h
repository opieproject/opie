#ifndef VPNRUN_H
#define VPNRUN_H

#include <asconnection.h>
#include "vpndata.h"

class VPNRun  : public AsConnection {

public :

      VPNRun( ANetNodeInstance * NNI, VPNData & Data ) : 
              AsConnection( NNI )
        { }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection *, Action_t )
        { return 0; }

      bool canSetState( State_t, Action_t )
        { return 0; }

      bool handlesInterface( const QString & I );

};

#endif
