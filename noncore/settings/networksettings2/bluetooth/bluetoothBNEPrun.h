#include <asdevice.h>
#include "bluetoothBNEPdata.h"

class BluetoothBNEPRun  : public AsDevice {

public :

      BluetoothBNEPRun( ANetNodeInstance * NNI, 
                        BluetoothBNEPData & D ) : 
                        AsDevice( NNI ),
                        Data( D),
                        Pat( "bnep[0-6]" )
        { }

      virtual AsDevice * asDevice( void ) 
        { return (AsDevice *)this; }

      virtual AsDevice * device( void ) 
        { return asDevice(); }

protected :

      void detectState( NodeCollection * );
      bool setState( NodeCollection * , Action_t, bool );
      bool canSetState( State_t , Action_t );
      bool handlesInterface( const QString & );

private :

      InterfaceInfo * getInterface( void );
      BluetoothBNEPData &         Data;
      static QDict<QString> *     PANConnections;
      QRegExp Pat;
};
