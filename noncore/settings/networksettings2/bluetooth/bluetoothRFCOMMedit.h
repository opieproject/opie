#include "bluetoothRFCOMMdata.h"
#include "bluetoothRFCOMMGUI.h"

#include <Opietooth.h>

class BluetoothRFCOMMEdit  : public BluetoothRFCOMMGUI {

public :

    BluetoothRFCOMMEdit( QWidget * parent );
    virtual ~BluetoothRFCOMMEdit( void );

    QString acceptable( void );
    void showData( BluetoothRFCOMMData & Data );
    bool commit( BluetoothRFCOMMData & Data );

    bool                    Modified;
    Opietooth2::OTGateway * OT;

public slots :

    void SLOT_AddServer( void );
    void SLOT_RemoveServer( void );
    void SLOT_FindDevice( void );

};
