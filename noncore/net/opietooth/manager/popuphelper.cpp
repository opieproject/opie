
#include "stdpopups.h"

#include "popuphelper.h"

using namespace OpieTooth;

PopupHelper::PopupHelper() {
    init();
}

PopupHelper::~PopupHelper() {

}

void PopupHelper::insert( int id, popupFactory fact ) {
    m_map.insert(id, fact );
}

QPopupMenu* PopupHelper::find( int id, const Services& ser, QListViewItem* item ) {
    //qWarning("find");
    FactoryMap::Iterator it = m_map.find(id );
    if ( it != m_map.end() ) {
        //qWarning("found");
        popupFactory fact = it.data();
        return (*fact)(ser, item);
    }
    return 0l;
}

void PopupHelper::init() {
    insert( 4355, newRfcComPopup );
    insert( 4353, newRfcComPopup );
    insert( 3, newObexPushPopup );
}
