#include <GUIUtils.h>
#include "bluetoothBNEPedit.h"


BluetoothBNEPEdit::BluetoothBNEPEdit( QWidget * Parent ) : BluetoothBNEPGUI( Parent ){

}

QString BluetoothBNEPEdit::acceptable( void ) {
    return QString();
}

bool BluetoothBNEPEdit::commit( BluetoothBNEPData & Data ) {
    return 0;
}

void BluetoothBNEPEdit::showData( BluetoothBNEPData & Data ) {
}
