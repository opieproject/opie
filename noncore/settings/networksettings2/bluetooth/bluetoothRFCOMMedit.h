#include "bluetoothRFCOMMdata.h"
#include "bluetoothRFCOMMGUI.h"

#include <Opietooth.h>
using namespace Opietooth2;

class BluetoothRFCOMMEdit  : public BluetoothRFCOMMGUI {

public :

    BluetoothRFCOMMEdit( QWidget * parent );
    virtual ~BluetoothRFCOMMEdit( void );

    QString acceptable( void );
    void showData( BluetoothRFCOMMData & Data );
    bool commit( BluetoothRFCOMMData & Data );

    bool Modified;
    OTGateway * OT;

public slots :

    void SLOT_AddServer( void );
    void SLOT_RemoveServer( void );
    void SLOT_FindDevice( void );

};
