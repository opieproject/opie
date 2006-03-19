#ifndef OBEXPOPUP_H
#define OBEXPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>
#include <services.h>
#include "btdeviceitem.h"

namespace OpieTooth {

    class ObexPopup : public QPopupMenu {
        Q_OBJECT
    public:
        ObexPopup(const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item);
        ~ObexPopup();
    protected:
        QAction* m_push;
        OpieTooth::BTDeviceItem *m_item;
        Services m_service;
    protected slots:
        void slotPush();
    };
};

#endif
