#include <asdevice.h>
#include "bluetoothBNEPdata.h"

class BluetoothBNEPRun  : public AsDevice {

public :

      BluetoothBNEPRun( ANetNodeInstance * NNI, 
                        BluetoothBNEPData & Data ) : AsDevice( NNI )
        { }

      virtual long count( void )
        { return 3; }
      virtual QString genNic( long nr )
        { QString S; return S.sprintf( "bnep%ld", nr ); }
      virtual AsDevice * device( void ) 
        { return asDevice(); }

protected :

      void detectState( NodeCollection * )
        { }

      bool setState( NodeCollection * , Action_t  )
        { return 0; }

      bool canSetState( State_t , Action_t )
        { return 0; }

      bool handlesInterface( const QString & )
        { return 0; }
};
