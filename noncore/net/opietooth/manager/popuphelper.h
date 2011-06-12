
#ifndef OPIE_TOOTH_POPUP_HELPER
#define OPIE_TOOTH_POPUP_HELPER

#include <qlistview.h>
#include <qpopupmenu.h>
#include <qmap.h>

#include <opie2/obluetoothservices.h>
#include <opie2/obluetoothdevicehandler.h>

namespace OpieTooth {
    typedef QPopupMenu* (*popupFactory)(const Opie::Bluez::OBluetoothServices&, BTDeviceItem*, Opie::Bluez::DeviceHandlerPool*);
    typedef QMap<int,  popupFactory> FactoryMap;
    class PopupHelper {
    public:
        PopupHelper(Opie::Bluez::DeviceHandlerPool*);
        ~PopupHelper();
        void insert( int id, popupFactory fact );
        QPopupMenu* find( int id, const Opie::Bluez::OBluetoothServices&, BTDeviceItem* );
    private:
        void init();
        FactoryMap m_map;
        Opie::Bluez::DeviceHandlerPool *m_devHandlerPool;
    };
};

#endif
