/* $Id: hidpopup.h,v 1.1 2006-07-06 16:43:36 korovkin Exp $ */
/* PAN context menu */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef HIDPOPUP_H
#define HIDPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>

#include <opie2/obluetoothservices.h>
#include "btdeviceitem.h"

namespace Opie {
namespace Bluez {
class DeviceHandlerPool;
class InputDeviceHandler;
}
}

namespace OpieTooth {

    class HidPopup : public QPopupMenu {

        Q_OBJECT

    public:
        HidPopup(const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem*, Opie::Bluez::DeviceHandlerPool *devHandlerPool );
        ~HidPopup();

    private:
        OpieTooth::BTDeviceItem *m_item;
        Opie::Bluez::InputDeviceHandler *m_devHandler;
    private slots:
        void slotConnect();
        void slotDisconnect();
    };
};

#endif
