#include <netnode.h>
#include "bluetoothBNEPdata.h"

#include <OTGateway.h>

class BluetoothBNEPRun  : public RuntimeInfo {

public :

      BluetoothBNEPRun( ANetNodeInstance * NNI, 
                        BluetoothBNEPData & D );
      virtual ~BluetoothBNEPRun( void );

      // i am a device
      virtual RuntimeInfo * device( void ) 
        { return this; }

      bool handlesInterface( const QString & );
      bool handlesInterface( InterfaceInfo * );

      State_t detectState( void );

protected :

      QString setMyState( NetworkSetup * , Action_t, bool );

private :

      bool hasFreePANNetworkSetup( bool Grab = 0 );

      BluetoothBNEPData &         Data;
      Opietooth2::OTGateway *     OT;
      QRegExp                     Pat;
};
