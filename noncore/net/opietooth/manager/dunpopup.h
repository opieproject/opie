#ifndef DUNPOPUP_H
#define DUNPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>

/* #include <startdunconnection.h> */

#include "btdeviceitem.h"


namespace OpieTooth {

    class DunPopup : public QPopupMenu {

	Q_OBJECT

    public:
        DunPopup( OpieTooth::BTDeviceItem* );
        ~DunPopup();

    private:
        QAction* m_push;
/* 	OpieTooth::StartDunConnection* m_dunconnection; */
	OpieTooth::BTDeviceItem *m_item;
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotConnectAndConfig();
   };
};

#endif
