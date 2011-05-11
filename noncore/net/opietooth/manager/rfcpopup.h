#ifndef RFCPOPUP_H
#define RFCPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>
#include <opie2/obluetoothservices.h>
#include "btdeviceitem.h"

namespace OpieTooth {
    /**
     * A simple reference implementation for
     * the popup helper factory.
     * This class derives from QPopupMenu and uses
     * plugged QActions to do all the nasty in it's
     * slots. After the work is done everything must
     * be deleted.
     */
    class RfCommPopup : public QPopupMenu {
        Q_OBJECT
    public:
        RfCommPopup(const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem*);
        ~RfCommPopup();

    private:
        QAction* m_con;
        QAction* m_dis;
        QAction* m_bind;
        OpieTooth::BTDeviceItem *m_item;
        Opie::Bluez::OBluetoothServices m_service;
        int procId; //Connection process number
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotBind();
    };
};

#endif
