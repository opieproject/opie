
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
    FactoryMap::Iterator it = m_map.find(id );
    if ( it != m_map.end() ) {
        popupFactory fact = it.data();
        return (*fact)(ser,  item);
    }
    return 0l;
}
void PopupHelper::init() {
    insert( 1,  newRfcComPopup );
    insert(2, newObexPushPopup );
}
