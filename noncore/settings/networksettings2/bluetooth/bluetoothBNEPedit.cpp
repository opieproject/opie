#include <qtopia/qcopenvelope_qws.h>

#include <GUIUtils.h>
#include "bluetoothBNEPedit.h"


BluetoothBNEPEdit::BluetoothBNEPEdit( QWidget * Parent ) : BluetoothBNEPGUI( Parent ){

}

QString BluetoothBNEPEdit::acceptable( void ) {
    return QString();
}

bool BluetoothBNEPEdit::commit( BluetoothBNEPData & ) {
    return 0;
}

void BluetoothBNEPEdit::showData( BluetoothBNEPData & ) {
}

void BluetoothBNEPEdit::SLOT_StartBTMgr( void ) {
      QCopEnvelope e( "QPE/System", "execute(QString)" );
              e << QString( "bluetooth-manager" );

}
