#ifndef BTLISTITEM_H
#define BTLISTITEM_H

#include <qlistview.h>

namespace OpieTooth {

    class BTListItem : public QListViewItem {
    public:
        enum Types { Device =0, Service, Connection };
        BTListItem( QListView * parent  );
        BTListItem( QListViewItem * parent );
        virtual QString type() const = 0;
        virtual int typeId() const = 0;
    };
};

#endif
