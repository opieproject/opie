#include <connection.h>
#include "btconnectionitem.h"

using namespace OpieTooth;

BTConnectionItem::BTConnectionItem( QListView* parent, const ConnectionState& con )
    : BTListItem( parent ){
    m_con = con;
    setText(0, m_con.mac() );
    setText(1, m_con.connectionMode() );
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

void BTConnectionItem::setName( QString name ) {
	m_name = name;
	setText( 0, m_name );
}

QString BTConnectionItem::name() {
	return m_name;
}


void BTConnectionItem::setSignalStrength( QString strength ) {
	m_signalStrength = strength;
	setText( 2, m_signalStrength );
}

QString BTConnectionItem::signalStrength()  {
	return m_signalStrength;
}
