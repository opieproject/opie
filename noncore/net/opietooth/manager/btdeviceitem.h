
#ifndef OPIE_TOOTH_BT_DEVIVE_ITEM
#define OPIE_TOOTH_BT_DEVIVE_ITEM


#include <remotedevice.h>
#include "btlistitem.h"

namespace OpieTooth {
    class BTDeviceItem : public BTListItem {
    public:
        BTDeviceItem( QListView* parent, const RemoteDevice& dev );
        ~BTDeviceItem();
        RemoteDevice remoteDevice() const;
        QString type()const;
        int typeId()const;
        QString mac()const;
        QString name() const;
    private:
        RemoteDevice m_device;
    };

};


#endif
