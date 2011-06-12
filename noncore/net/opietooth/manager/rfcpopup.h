#ifndef RFCPOPUP_H
#define RFCPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>
#include <opie2/obluetoothservices.h>
#include "btdeviceitem.h"

namespace Opie {
namespace Bluez {
class DeviceHandlerPool;
class SerialDeviceHandler;
}
}

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
        RfCommPopup(const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem*, Opie::Bluez::DeviceHandlerPool *devHandlerPool );
        ~RfCommPopup();

    private:
        OpieTooth::BTDeviceItem *m_item;
        Opie::Bluez::OBluetoothServices m_service;
        Opie::Bluez::SerialDeviceHandler *m_devHandler;
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotBind();
    };
};

#endif
