#include <asline.h>
#include "modemdata.h"

class ModemRun  : public AsLine {

public :

      ModemRun( ANetNodeInstance * NNI, 
                ModemData & Data ) : AsLine ( NNI )
        { }

      virtual AsLine * asLine( void ) 
        { return (AsLine *)this; }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection *, Action_t, bool )
        { return 0; }

      bool canSetState( State_t, Action_t )
        { return 0; }
};
