
#include "btlistitem.h"

namespace OpieTooth {


    BTListItem::BTListItem( QListView * parent  ) : QListViewItem( parent ) {
    }

    BTListItem::BTListItem( QListViewItem * parent ) : QListViewItem( parent ) {
    }


    // name, and then mac and then servicetype
    BTListItem::BTListItem( QListView * parent,  const QString &name , const QString& mac, const QString& type   )
        : QListViewItem( parent, name ){

        m_name = name;
        m_mac = mac;
        m_type = type;

    }

    BTListItem::BTListItem( QListViewItem * parent , const QString& name, const QString& mac, const QString& type   )
        : QListViewItem( parent, name ){

        m_name = name;
        m_mac = mac;
        m_type = type;
    }


    void BTListItem::setMac( const QString& mac ) {
        m_mac = mac;
    }

    QString BTListItem::mac() const {
        return m_mac;
    }

    void BTListItem::setName( const QString& name ) {
        m_name = name;
    }

    QString BTListItem::name() const {
        return m_name;
    }

    void BTListItem::setType( const QString& type ) {
        m_type = type;
    }

    QString BTListItem::type() const {
        return m_type;
    }


    BTListItem::~BTListItem() {
    }

};
