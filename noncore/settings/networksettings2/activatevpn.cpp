#include <qlistview.h>
#include <qheader.h>
#include <resources.h>
#include "activatevpn.h"

class MyCheckListItem : public QCheckListItem {

public :

      MyCheckListItem( NetworkSetup * N, QListView * V );

      NetworkSetup * NC;

};

MyCheckListItem::MyCheckListItem( NetworkSetup * N, QListView * V ):
                QCheckListItem( V, N->name() ) {
      NC = N;
}

ActivateVPN::ActivateVPN( const QString & I ) :
                  ActivateVPNGUI( 0, 0, TRUE ), NSD() {

    QCheckListItem * CI;
    VPN_LV->clear();
    VPN_LV->header()->hide();

    // find all NetworkSetups that want to be triggered by this interface
    for( QDictIterator<NetworkSetup> it(NSResources->networkSetups());
         it.current();
         ++it ) {
      if( it.current()->triggeredBy( I ) ) {
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
