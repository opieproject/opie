#include "bluetoothRFCOMMdata.h"
#include "bluetoothRFCOMMGUI.h"

class BluetoothRFCOMMEdit  : public BluetoothRFCOMMGUI {

public :

    BluetoothRFCOMMEdit( QWidget * parent );

    QString acceptable( void );
    void showData( BluetoothRFCOMMData & Data );
    bool commit( BluetoothRFCOMMData & Data );
};
