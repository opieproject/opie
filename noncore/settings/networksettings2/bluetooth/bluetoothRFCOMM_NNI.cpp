#include "bluetoothRFCOMMedit.h"
#include "bluetoothRFCOMM_NNI.h"
#include "bluetooth_NN.h"

ABluetoothRFCOMM::ABluetoothRFCOMM( BluetoothRFCOMMNetNode * PNN ) : ANetNodeInstance( PNN ) {
    GUI = 0;
    RT = 0;
}

void ABluetoothRFCOMM::setSpecificAttribute( QString & , QString & ) {
}

void ABluetoothRFCOMM::saveSpecificAttribute( QTextStream & ) {
}


QWidget * ABluetoothRFCOMM::edit( QWidget * parent ) {
    GUI = new BluetoothRFCOMMEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString ABluetoothRFCOMM::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void ABluetoothRFCOMM::commit( void ) {
    if( GUI->commit( Data ) )
      setModified( 1 );
}
