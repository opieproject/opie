#include <asline.h>
#include "bluetoothRFCOMMdata.h"

class BluetoothRFCOMMRun  : public AsLine {

public :

      BluetoothRFCOMMRun( ANetNodeInstance * NNI, 
                          BluetoothRFCOMMData & Data ) : AsLine( NNI )
        { }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection * , Action_t )
        { return 0; }

      bool canSetState( State_t , Action_t )
        { return 0; }
};
