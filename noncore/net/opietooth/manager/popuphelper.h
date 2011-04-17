
#ifndef OPIE_TOOTH_POPUP_HELPER
#define OPIE_TOOTH_POPUP_HELPER

#include <qlistview.h>
#include <qpopupmenu.h>
#include <qmap.h>

#include <opie2/obluetoothservices.h>

namespace OpieTooth {
    typedef QPopupMenu* (*popupFactory)(const Opie::Bluez::OBluetoothServices&, BTDeviceItem*);
    typedef QMap<int,  popupFactory> FactoryMap;
    class PopupHelper {
    public:
        PopupHelper();
        ~PopupHelper();
        void insert( int id, popupFactory fact );
        QPopupMenu* find( int id, const Opie::Bluez::OBluetoothServices&, BTDeviceItem* );
    private:
        void init();
        FactoryMap m_map;

    };
};

#endif
