#include <asline.h>
#include "cabledata.h"

class CableRun  : public AsLine {

public :

      CableRun( ANetNodeInstance * NNI, 
                CableData & Data ) : AsLine( NNI )
        { D = &Data; }

protected :

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A );
      bool canSetState( State_t Curr, Action_t A );

private :

      CableData_t * D;
};
