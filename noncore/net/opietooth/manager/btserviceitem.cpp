
#include <qobject.h>
#include "btserviceitem.h"

using namespace OpieTooth;

BTServiceItem::BTServiceItem( QListViewItem* item,  const Services& serv )
    : BTListItem( item ) {
    m_service = serv;
    setText(0, QObject::tr(serv.serviceName() ) );
}
BTServiceItem::~BTServiceItem() {

}
QString BTServiceItem::type() const {
    return QString::fromLatin1("service");
}
int BTServiceItem::typeId() const {
    return Service;
}
Services BTServiceItem::services() const {
    return m_service;
}
