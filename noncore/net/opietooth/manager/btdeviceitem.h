
#ifndef OPIE_TOOTH_BT_DEVICE_ITEM
#define OPIE_TOOTH_BT_DEVICE_ITEM


#include <opie2/obluetoothdevicerecord.h>
#include "btlistitem.h"

namespace OpieTooth {
    class BTDeviceItem : public BTListItem {
    public:
        BTDeviceItem( QListView* parent, const Opie::Bluez::DeviceRecord& dev );
        ~BTDeviceItem();
        Opie::Bluez::DeviceRecord remoteDevice() const;
        QString type() const;
        int typeId() const;
        QString mac() const;
        QString name() const;
    private:
        Opie::Bluez::DeviceRecord m_device;
    };

};


#endif
