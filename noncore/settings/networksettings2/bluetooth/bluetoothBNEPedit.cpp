#include <qtopia/qcopenvelope_qws.h>
#include <qlistview.h>
#include <qcheckbox.h>

#include <GUIUtils.h>
#include "bluetoothBNEPedit.h"


BluetoothBNEPEdit::BluetoothBNEPEdit( QWidget * Parent ) : BluetoothBNEPGUI( Parent ){

}

QString BluetoothBNEPEdit::acceptable( void ) {
    if( ( ! AnyPAN_CB->isChecked() ) && 
          BTPANServers_LV->firstChild() == 0 ) {
      return tr("<p>No bluetooth device addresses specified</p>");
    }

    return QString();
}

bool BluetoothBNEPEdit::commit( BluetoothBNEPData & Data ) {
      QListViewItem * it = BTPANServers_LV->firstChild();
      Data.BDAddress.clear();
      while( it ) {
        Data.BDAddress << it->text(0);
        it = it->nextSibling();
      }
      return 0;
}

void BluetoothBNEPEdit::showData( BluetoothBNEPData & Data ) {
      QListViewItem * lvit;
      BTPANServers_LV->clear();

      for ( QStringList::Iterator it = Data.BDAddress.begin(); 
            it != Data.BDAddress.end(); 
            ++it ) {
        lvit = new QListViewItem(BTPANServers_LV);
        lvit->setText( 0, (*it) );
      }
}

void BluetoothBNEPEdit::SLOT_StartBTMgr( void ) {
      QCopEnvelope e( "QPE/System", "execute(QString)" );
              e << QString( "bluetooth-manager" );

}
