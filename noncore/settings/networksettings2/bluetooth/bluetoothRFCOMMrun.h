#include <asline.h>
#include "bluetoothRFCOMMdata.h"

class BluetoothRFCOMMRun  : public AsLine {

public :

      BluetoothRFCOMMRun( ANetNodeInstance * NNI, 
                          BluetoothRFCOMMData & Data ) : AsLine( NNI )
        { }

      virtual AsLine * asLine( void ) 
        { return (AsLine *)this; }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection * , Action_t, bool )
        { return 0; }

      bool canSetState( State_t , Action_t )
        { return 0; }
};
