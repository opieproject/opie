#include "bluetoothBNEPdata.h"
#include "bluetoothBNEPGUI.h"

class BluetoothBNEPEdit  : public BluetoothBNEPGUI {

public :

    BluetoothBNEPEdit( QWidget * parent );

    QString acceptable( void );
    bool commit( BluetoothBNEPData & Data );
    void showData( BluetoothBNEPData & Data );
};
