#include <connection.h>
#include "btconnectionitem.h"

using namespace OpieTooth;

BTConnectionItem::BTConnectionItem( QListView* parent, const ConnectionState& con )
    : BTListItem( parent ){
    m_con = con;
    setText(0, m_con.mac() );
}
BTConnectionItem::~BTConnectionItem() {

}
QString BTConnectionItem::type() const {
    return QString::fromLatin1("connection");
}
int BTConnectionItem::typeId() const {
    return Connection;
}
ConnectionState BTConnectionItem::connection() const {
    return m_con;
}

