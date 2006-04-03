#ifndef DUNPOPUP_H
#define DUNPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>
#include <services.h>

#include "btdeviceitem.h"


namespace OpieTooth {

    class DunPopup : public QPopupMenu {

	Q_OBJECT

    public:
        DunPopup(const OpieTooth::Services&, OpieTooth::BTDeviceItem* );
        ~DunPopup();

    private:
        QAction* m_push;
        OpieTooth::BTDeviceItem *m_item; //device item 
        Services m_service; //device service (port)
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotConnectAndConfig();
   };
};

#endif
