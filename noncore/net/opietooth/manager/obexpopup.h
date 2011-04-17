#ifndef OBEXPOPUP_H
#define OBEXPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>
#include <opie2/obluetoothservices.h>
#include "btdeviceitem.h"

namespace OpieTooth {

    class ObexPopup : public QPopupMenu {
        Q_OBJECT
    public:
        ObexPopup(const Opie::Bluez::OBluetoothServices& service,  OpieTooth::BTDeviceItem* item);
        ~ObexPopup();
    protected:
        QAction* m_push;
        OpieTooth::BTDeviceItem *m_item;
        Opie::Bluez::OBluetoothServices m_service;
    protected slots:
        void slotPush();
    };
};

#endif
