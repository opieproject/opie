#ifndef BTLISTITEM_H
#define BTLISTITEM_H

#include <qlistview.h>

namespace OpieTooth {

    class BTListItem : public QListViewItem {

    public:
        BTListItem( QListView * parent  );
        BTListItem( QListViewItem * parent );

        // name, and then mac and then servicetype
        BTListItem( QListView * ,  const QString&, const QString&, const QString&  );
        BTListItem( QListViewItem * parent , const QString&, const QString& , const QString& );
        ~BTListItem();

        void setMac( const QString& );
        QString mac()const;
        void setName( const QString& );
        QString name()const;
        void setType( const QString& );
        QString type()const;
    private:
        QString m_name;
        QString m_mac;
        QString m_type;

    };
};

#endif
