
#include "btlistitem.h"

namespace OpieTooth {


    BTListItem::BTListItem( QListView * parent  ) : QListViewItem( parent ) {
        m_name = "";
        m_mac ="";
        m_type ="";
    }

    BTListItem::BTListItem( QListViewItem * parent ) : QListViewItem( parent ) {
        m_name = "";
        m_mac ="";
        m_type ="";
    }


    // name, and then mac and then servicetype
    BTListItem::BTListItem( QListView * parent,  QString name , QString mac, QString type   )
        : QListViewItem( parent, name ){

        m_name = name;
        m_mac = mac;
        m_type = type;

    }

    BTListItem::BTListItem( QListViewItem * parent , QString name, QString mac, QString type   )
        : QListViewItem( parent, name ){

        m_name = name;
        m_mac = mac;
        m_type = type;
    }


    void BTListItem::setMac( QString mac ) {
        m_mac = mac;
    }

    QString BTListItem::mac() {
        return m_mac;
    }

    void BTListItem::setName( QString name ) {
        m_name = name;
    }

    QString BTListItem::name() {
        return m_name;
    }

    void BTListItem::setType( QString type ) {
        m_type = type;
    }

    QString BTListItem::type() {
        return m_type;
    }


    BTListItem::~BTListItem() {
    }

};
