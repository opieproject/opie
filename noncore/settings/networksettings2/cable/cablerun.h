#include <asline.h>
#include "cabledata.h"

class CableRun  : public AsLine {

public :

      CableRun( ANetNodeInstance * NNI, 
                CableData_t & D ) : AsLine( NNI )
        { Data = &D; }

      virtual AsLine * asLine( void ) 
        { return (AsLine *)this; }

      virtual QString deviceFile( void );

protected :

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A, bool Force );
      bool canSetState( State_t Curr, Action_t A );

private :

      CableData_t * Data;
};
