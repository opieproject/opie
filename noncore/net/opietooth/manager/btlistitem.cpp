
#include "btlistitem.h"

namespace OpieTooth {


    BTListItem::BTListItem( QListView * parent  ) : QListViewItem( parent ) {

    }

    BTListItem::BTListItem( QListViewItem * parent ) : QListViewItem( parent ) {

    }


    // name, and then mac and then servicetype
    BTListItem::BTListItem( QListView * parent,  QString name , QString mac, QString type   )
        : QListViewItem( parent, name, mac, type  ){

    }

    BTListItem::BTListItem( QListViewItem * parent , QString name, QString mac, QString type   )
        : QListViewItem( parent, name, mac, type  ){

    }

    BTListItem::~BTListItem() {
    }

}
