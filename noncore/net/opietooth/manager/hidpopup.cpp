/* $Id: hidpopup.cpp,v 1.1 2006-07-06 16:43:36 korovkin Exp $ */
/* PAN context menu */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qmessagebox.h>
#include <opie2/odebug.h>
#include <opie2/obluetooth.h>
#include <opie2/obluetoothdevicehandler.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;
using namespace Opie::Bluez;

#include <qtimer.h>

#include "hidpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
HidPopup::HidPopup(const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem* item, Opie::Bluez::DeviceHandlerPool *devHandlerPool ) :
    QPopupMenu()  
{

    owarn << "HidPopup c'tor" << oendl;

    m_item = item;
    QAction* a;
    QAction* c;

    /* connect action */
    a = new QAction(); // so it's get deleted
    a->setText( tr("connect") );
    a->addTo( this );
    connect( a, SIGNAL( activated() ), this, SLOT( slotConnect() ) );

    c = new QAction();
    c->setText( tr( "disconnect" ) );
    c->addTo( this );
    connect( c, SIGNAL( activated() ), this, SLOT( slotDisconnect() ) );

    OBluetooth *bt = OBluetooth::instance();
    OBluetoothInterface *intf = bt->defaultInterface();
    OBluetoothDevice *dev = intf->findDevice( m_item->mac() );

    if( dev ) {
        m_devHandler = (InputDeviceHandler *)devHandlerPool->getHandler( dev, InputDeviceHandler::dbusInterface() );
    }
    else {
        m_devHandler = NULL;
        odebug << "no device" << oendl;
    }
}

HidPopup::~HidPopup()
{
}

void HidPopup::slotConnect()
{
    m_devHandler->connect();
}

void HidPopup::slotDisconnect()
{
    m_devHandler->disconnect();
}
