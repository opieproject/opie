
//#include "rfccompopup.h"

#include "stdpopups.h"

extern "C" {

    QPopupMenu* newRfcComPopup( const OpieTooth::Services& servive, QListViewItem* item ) {
        //  return OpieTooth::RfcComPopup( servive,  item );
        return 0l;
    }
    QPopupMenu* newObexPushPopup( const OpieTooth::Services&, QListViewItem* ) {
        return 0l;
    }

}
