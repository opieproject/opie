#include <asdevice.h>
#include "bluetoothBNEPdata.h"

class BluetoothBNEPRun  : public AsDevice {

public :

      BluetoothBNEPRun( ANetNodeInstance * NNI, 
                        BluetoothBNEPData & Data ) : AsDevice( NNI )
        { }

      virtual AsDevice * asDevice( void ) 
        { return (AsDevice *)this; }

      virtual AsDevice * device( void ) 
        { return asDevice(); }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection * , Action_t, bool )
        { return 0; }

      bool canSetState( State_t , Action_t )
        { return 0; }

      bool handlesInterface( const QString & )
        { return 0; }
};
