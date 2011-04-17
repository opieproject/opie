/* $Id: obexftpopup.h,v 1.1 2006-03-19 14:59:21 korovkin Exp $ */
/* OBEX file browser popup */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _OBEXFTPPOPUP_H_
#define _OBEXFTPPOPUP_H_
#include <qpopupmenu.h>
#include <qaction.h>
#include <opie2/obluetoothservices.h>
#include "btdeviceitem.h"
namespace OpieTooth {

    class ObexFtpPopup : public QPopupMenu  {
        Q_OBJECT

    public:
        ObexFtpPopup(const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem*);
        ~ObexFtpPopup();
    private:
        QAction* m_push;
        QAction* m_browse;
        OpieTooth::BTDeviceItem *m_item;
        Opie::Bluez::OBluetoothServices m_service;
    protected slots:
        void slotBrowse();
        void slotPush();
    };
};
#endif
