#include <netnode.h>
#include "bluetoothBNEPdata.h"

#include <OTGateway.h>
using namespace Opietooth2;

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

      QString setMyState( NodeCollection * , Action_t, bool );

private :

      bool hasFreePANConnection( bool Grab = 0 );

      BluetoothBNEPData &         Data;
      OTGateway *                 OT;
      QRegExp Pat;
};
