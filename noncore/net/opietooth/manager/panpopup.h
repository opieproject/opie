#ifndef PANPOPUP_H
#define PANPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>

#include <startpanconnection.h>

#include "btdeviceitem.h"


namespace OpieTooth {

    class PanPopup : public QPopupMenu {

	Q_OBJECT

    public:
        PanPopup( OpieTooth::BTDeviceItem* );
        ~PanPopup();

    private:
        QAction* m_push;
	OpieTooth::StartPanConnection* m_panconnection;
	OpieTooth::BTDeviceItem *m_item;
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotConnectAndConfig();
   };
};

#endif
