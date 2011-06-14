/* $Id: panpopup.h,v 1.4 2006-04-04 12:15:10 korovkin Exp $ */
/* PAN context menu */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PANPOPUP_H
#define PANPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>

#include <opie2/obluetoothservices.h>

#include "btdeviceitem.h"

namespace Opie {
namespace Bluez {
class DeviceHandlerPool;
class NetworkDeviceHandler;
}
}

namespace OpieTooth {

    class PanPopup : public QPopupMenu {
        Q_OBJECT
    public:
        PanPopup( const Opie::Bluez::OBluetoothServices& service, OpieTooth::BTDeviceItem*, 
                  Opie::Bluez::DeviceHandlerPool *devHandlerPool );
        ~PanPopup();
    private:
        OpieTooth::BTDeviceItem *m_item;
        Opie::Bluez::OBluetoothServices m_service;
        Opie::Bluez::NetworkDeviceHandler *m_devHandler;
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotConnectAndConfig();
    };
};

#endif
