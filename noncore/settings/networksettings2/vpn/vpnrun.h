#ifndef VPNRUN_H
#define VPNRUN_H

#include <asconnection.h>
#include "vpndata.h"

class VPNRun  : public AsConnection {

public :

      VPNRun( ANetNodeInstance * NNI, VPNData & Data ) : 
              AsConnection( NNI )
        { }

      virtual AsConnection * asConnection( void ) 
        { return (AsConnection *)this; }
protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection *, Action_t, bool )
        { return 0; }

      bool canSetState( State_t, Action_t )
        { return 0; }

      bool handlesInterface( const QString & I );

};

#endif
