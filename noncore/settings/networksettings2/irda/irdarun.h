#include <asline.h>
#include "irdadata.h"

class IRDARun  : public AsLine {

public :

      IRDARun( ANetNodeInstance * NNI, 
               IRDAData & Data ) : AsLine( NNI )
        { }

      virtual AsLine * asLine( void ) 
        { return (AsLine *)this; }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection * , Action_t )
        { return 0; }

      bool canSetState( State_t , Action_t )
        { return 0; }
};
