#include "bluetoothBNEPedit.h"
#include "bluetoothBNEP_NNI.h"
#include "bluetooth_NN.h"

ABluetoothBNEP::ABluetoothBNEP( BluetoothBNEPNetNode * PNN ) : ANetNodeInstance( PNN ) {
    GUI = 0;
    RT = 0;
}

void ABluetoothBNEP::setSpecificAttribute( QString & , QString & ) {
}

void ABluetoothBNEP::saveSpecificAttribute( QTextStream & ) {
}

QWidget * ABluetoothBNEP::edit( QWidget * parent ) {
    GUI = new BluetoothBNEPEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString ABluetoothBNEP::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void ABluetoothBNEP::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}
