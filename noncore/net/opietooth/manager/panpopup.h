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
