#include <GUIUtils.h>
#include "bluetoothRFCOMMedit.h"

BluetoothRFCOMMEdit::BluetoothRFCOMMEdit( QWidget * Parent ) : 
      BluetoothRFCOMMGUI( Parent ){

}

QString BluetoothRFCOMMEdit::acceptable( void ) {
    return QString();
}

void BluetoothRFCOMMEdit::showData( BluetoothRFCOMMData & Data ) {
}

bool BluetoothRFCOMMEdit::commit( BluetoothRFCOMMData & Data ) {
    return 0;
}
