#include <netnode.h>
#include "bluetoothRFCOMMdata.h"

#include <OTGateway.h>

class BluetoothRFCOMMRun  : public RuntimeInfo {

public :

      BluetoothRFCOMMRun( ANetNodeInstance * NNI, 
                          BluetoothRFCOMMData & D ) : RuntimeInfo( NNI )
        {  DeviceNr = -1; Data = &D; OT = 0; }
      virtual ~BluetoothRFCOMMRun( void );

      virtual RuntimeInfo * line( void ) 
        { return this; }
      virtual QString deviceFile( void );

      State_t detectState( void );

protected :

      QString setMyState( NetworkSetup * , Action_t, bool );

private :

      int deviceNrOfNetworkSetup( void );
      RFCOMMChannel * getChannel( void );
      BluetoothRFCOMMData *   Data;
      Opietooth2::OTGateway * OT;
      int                     DeviceNr; // cached from detection
};
