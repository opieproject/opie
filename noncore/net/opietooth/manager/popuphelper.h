
#ifndef OPIE_TOOTH_POPUP_HELPER
#define OPIE_TOOTH_POPUP_HELPER

#include <qpopupmenu.h>
#include <qmap.h>


namespace OpieTooth {
    typedef QPopupMenu* (*popupFactory)(void);
    typedef QMap<int,  popupFactory> FactoryMap;
    class PopupHelper {
    public:
        PopupHelper();
        ~PopupHelper();
        void insert( int id, popupFactory fact );
        QPopupMenu* find( int id );
    private:
        void init();
        FactoryMap m_map;

    };
};

#endif
