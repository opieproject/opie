#ifndef BTLISTITEM_H
#define BTLISTITEM_H

#include <qlistview.h>

namespace OpieTooth {

    class BTListItem : public QListViewItem {

    public:
        BTListItem( QListView * parent  );
        BTListItem( QListViewItem * parent );

        // name, and then mac and then servicetype
        BTListItem( QListView * ,  QString, QString, QString  );
        BTListItem( QListViewItem * parent , QString, QString , QString );
        ~BTListItem();

        void setMac( QString );
        QString mac();
        void setName( QString );
        QString name();
        void setType( QString );
        QString type();
    private:
        QString m_name;
        QString m_mac;
        QString m_type;

    };
};

#endif
