#include "bluetoothBNEPedit.h"
#include "bluetoothBNEP_NNI.h"
#include "bluetoothBNEP_NN.h"

ABluetoothBNEP::ABluetoothBNEP( BluetoothBNEPNetNode * PNN ) : 
    ANetNodeInstance( PNN ), Data() {
    GUI = 0;
    RT = 0;
    Data.AllowAll = 1;
}

void ABluetoothBNEP::setSpecificAttribute( QString & S, QString & A ) {
      if( S == "bdaddress" ) {
        Data.BDAddress << A;
      } else if ( S == "allowall" ) {
        Data.AllowAll = 1;
      }
}

void ABluetoothBNEP::saveSpecificAttribute( QTextStream & TS ) {
      TS  << "allowall=" << Data.AllowAll << endl;
      for ( QStringList::Iterator it = Data.BDAddress.begin(); 
            it != Data.BDAddress.end(); 
            ++it ) {
        TS  << "bdaddress=" << (*it) << endl;
      }
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
