

#ifndef OPIE_TOOTH_BT_SERVICE_ITEM
#define OPIE_TOOTH_BT_SERVICE_ITEM

#include <services.h>

#include "btlistitem.h"

namespace OpieTooth {

    class BTServiceItem : public BTListItem {
    public:
        BTServiceItem( QListViewItem* item, const Services& );
        ~BTServiceItem();
        QString type() const;
        int typeId() const;
        Services services() const;
    private:
        Services m_service;

    };
};


#endif
