#include "bluetoothRFCOMMedit.h"
#include "bluetoothRFCOMM_NNI.h"
#include "bluetoothRFCOMM_NN.h"

ABluetoothRFCOMM::ABluetoothRFCOMM( BluetoothRFCOMMNetNode * PNN ) : 
                  ANetNodeInstance( PNN ), Data() {
      Data.Devices.setAutoDelete( TRUE );
      GUI = 0;
      RT = 0;
}

void ABluetoothRFCOMM::setSpecificAttribute( QString & A, QString & V) {

      if( A == "bdaddress" ) {
        Data.Devices.resize( Data.Devices.size() + 1 );
        Data.Devices.insert( Data.Devices.size() - 1, new RFCOMMChannel);
        Data.Devices[ Data.Devices.size() - 1 ]->BDAddress = V;
      } else if ( A == "channel" ) {
        Data.Devices[ Data.Devices.size() - 1 ]->Channel = V.toLong();
      } else if ( A == "name" ) {
        Data.Devices[ Data.Devices.size() - 1 ]->Name = V;
      }
}

void ABluetoothRFCOMM::saveSpecificAttribute( QTextStream & TS ) {
      for( unsigned int i = 0 ;
           i < Data.Devices.count();
           i ++ ) {
        TS << "bdaddress=" << Data.Devices[i]->BDAddress << endl;
        TS << "name=" << quote( Data.Devices[i]->Name ) << endl;
        TS << "channel=" << Data.Devices[i]->Channel << endl;
      }
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
