#include <qlistview.h>
#include <qheader.h>
#include <resources.h>
#include "activatevpn.h"

class MyCheckListItem : public QCheckListItem {

public :

      MyCheckListItem( NodeCollection * N, QListView * V );

      NodeCollection * NC;

};

MyCheckListItem::MyCheckListItem( NodeCollection * N, QListView * V ):
                QCheckListItem( V, N->name() ) {
      NC = N;
}

ActivateVPN::ActivateVPN( void ) :
                  ActivateVPNGUI( 0, 0, TRUE ), NSD() {

    QCheckListItem * CI;
    VPN_LV->clear();
    VPN_LV->header()->hide();

    for( QDictIterator<NodeCollection> it(NSResources->connections());
         it.current();
         ++it ) {
      if( it.current()->triggeredBy( "vpn" ) ) {
        CI = new MyCheckListItem( it.current(), VPN_LV );
      }
    }
}

ActivateVPN::~ActivateVPN( void ) {
}

void ActivateVPN::SLOT_ChangedVPNSetting( QListViewItem * I ) {

      MyCheckListItem * MI = (MyCheckListItem *)I;

      printf( "%s : %d\n", 
          MI->text(0).latin1(),
          MI->isOn() );
}
