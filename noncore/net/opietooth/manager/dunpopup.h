/* $Id: dunpopup.h,v 1.3 2006-04-04 12:15:10 korovkin Exp $ */
/* DUN context menu */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef DUNPOPUP_H
#define DUNPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>
#include <opie2/obluetoothservices.h>

#include "btdeviceitem.h"


namespace OpieTooth {

    class DunPopup : public QPopupMenu {

	Q_OBJECT

    public:
        DunPopup(const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem* );
        ~DunPopup();

    private:
        QAction* m_push;
        OpieTooth::BTDeviceItem *m_item; //device item
        Opie::Bluez::OBluetoothServices m_service; //device service (port)
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotConnectAndConfig();
   };
};

#endif
