#include <asconnection.h>
#include "networkdata.h"

class NetworkRun  : public AsConnection {

public :

      NetworkRun( ANetNodeInstance * NNI, 
                  NetworkData & Data ) : AsConnection( NNI )
        { }

      virtual AsConnection * asConnection( void ) 
        { return (AsConnection *)this; }

protected :

      void detectState( NodeCollection * );
      bool setState( NodeCollection * ,Action_t A );
      bool canSetState( State_t , Action_t A );

      bool handlesInterface( const QString & I );

};
