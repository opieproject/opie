#include <qlistview.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <OTPeer.h>
#include <OTGateway.h>
#include <Opietooth.h>
#include <resources.h>
#include <GUIUtils.h>
#include "bluetoothRFCOMMedit.h"

using namespace Opietooth2;

class PeerLBI : public QListViewItem {

public :

      PeerLBI( OTPeer * P, int Ch, QListView * LV ) : QListViewItem( LV ) {
        Peer = P;
        Channel = Ch;

        setText( 0, (P->name().isEmpty()) ?
                      P->address().toString() :
                      P->name() );
        QString S;
        S.setNum( Ch );
        setText( 1, S );
        setText( 2, P->address().toString() );
      }
      ~PeerLBI( ) {
      }

      inline int channel( void ) const
        { return Channel; }
      inline OTPeer * peer( void ) const
        { return Peer; }

      int Channel;
      OTPeer * Peer;
};

BluetoothRFCOMMEdit::BluetoothRFCOMMEdit( QWidget * Parent ) :
      BluetoothRFCOMMGUI( Parent ){
      Modified = 0;
      OT = OTGateway::getOTGateway();

      Add_TB->setPixmap( NSResources->getPixmap( "add" ) );
      Remove_TB->setPixmap( NSResources->getPixmap( "remove" ) );
      FindDevice_TB->setPixmap( NSResources->getPixmap( "Devices/bluetooth" ) );
      Addresses_LV->setColumnAlignment( 1, Qt::AlignRight );
}

BluetoothRFCOMMEdit::~BluetoothRFCOMMEdit( void ) {
      OTGateway::releaseOTGateway();
}

QString BluetoothRFCOMMEdit::acceptable( void ) {
      return QString();
}

bool BluetoothRFCOMMEdit::commit( BluetoothRFCOMMData & Data ) {
    int ct = 0;
    PeerLBI * I;

    if( Modified ) {
      QListViewItem * it = Addresses_LV->firstChild();

      Data.Devices.resize( 0 );
      while( it ) {

        ct ++;
        Data.Devices.resize( ct );
        I = (PeerLBI * )it;

        Data.Devices.insert( ct-1, new RFCOMMChannel );

        Data.Devices[ct-1]->BDAddress = I->peer()->address().toString();
        Data.Devices[ct-1]->Name = I->peer()->name();
        Data.Devices[ct-1]->Channel = I->channel();

        it = it->nextSibling();
      }
    }

    return Modified;
}

void BluetoothRFCOMMEdit::showData( BluetoothRFCOMMData & Data ) {

    OTPeer * P;

    for( unsigned int i = 0;
         i < Data.Devices.count();
         i ++ ) {
      P = new OTPeer( OT );
      P->setAddress( OTDeviceAddress( Data.Devices[i]->BDAddress ) );
      P->setName( Data.Devices[i]->Name );

      new PeerLBI( P, Data.Devices[i]->Channel, Addresses_LV );
    }
    Modified = 0;
}

void BluetoothRFCOMMEdit::SLOT_AddServer( void ) {
    QListViewItem * it = Addresses_LV->firstChild();

    while( it ) {
      // check address
      if( it->text(2) == Address_LE->text() ) {
        // already in table
        return;
      }
      it = it->nextSibling();
    }

    // new server
    Modified = 1;
    OTPeer * P = new OTPeer( OT );
    P->setAddress( OTDeviceAddress( Address_LE->text() ) );
    P->setName( Name_LBL->text() );
    new PeerLBI( P, Channel_SB->value(), Addresses_LV );

    Address_LE->setText("");
    Name_LBL->setText("");
    Channel_SB->setValue(1);
}

void BluetoothRFCOMMEdit::SLOT_RemoveServer( void ) {

    QListViewItem * it = Addresses_LV->firstChild();

    while( it ) {
      // check address
      if( it->isSelected() ) {
        delete it;
        Modified = 1;
        return;
      }
      it = it->nextSibling();
    }
}

void BluetoothRFCOMMEdit::SLOT_FindDevice( void ) {
    OTPeer * Peer;
    int Channel;

    // find device in Opietooth
    if( OTScan::getDevice( Peer, Channel, OT ) == QDialog::Accepted ) {
      Address_LE->setText( Peer->address().toString() );
      Name_LBL->setText( Peer->name() );
      Channel_SB->setValue( Channel );
    }
}
