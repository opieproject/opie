#include <asline.h>
#include "modemdata.h"

class ModemRun  : public AsLine {

public :

      ModemRun( ANetNodeInstance * NNI, 
                ModemData & Data ) : AsLine ( NNI )
        { }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection *, Action_t )
        { return 0; }

      bool canSetState( State_t, Action_t )
        { return 0; }
};
