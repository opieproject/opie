
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
QPopupMenu* PopupHelper::find( int id ) {
    FactoryMap::Iterator it = m_map.find(id );
    if ( it != m_map.end() ) {
        popupFactory fact = it.data();
        return (*fact)();
    }
    return 0l;
}
void PopupHelper::init() {

}
