#ifndef BTLISTITEM_H
#define BTLISTITEM_H

#include <qlistview.h>

namespace OpieTooth {

    class BTListItem : public QListViewItem {
        Q_OBJECT

    public:

        BTListItem( QListView * parent  );
        BTListItem( QListViewItem * parent );

        // name, and then mac and then servicetype
        BTListItem( QListView * ,  QString, QString, QString  );
        BTListItem( QListViewItem * parent , QString, QString , QString );
        ~BTListItem();

    }
}

#endif
