

#ifndef OPIE_TOOTH_BT_SERVICE_ITEM
#define OPIE_TOOTH_BT_SERVICE_ITEM

#include <opie2/obluetoothservices.h>

#include "btlistitem.h"

namespace OpieTooth {

    class BTServiceItem : public BTListItem {
    public:
        BTServiceItem( QListViewItem* item, const Opie::Bluez::OBluetoothServices& );
        ~BTServiceItem();
        QString type() const;
        int typeId() const;
        Opie::Bluez::OBluetoothServices services() const;
        int serviceId() const;
    private:
        Opie::Bluez::OBluetoothServices m_service;

    };
};


#endif
