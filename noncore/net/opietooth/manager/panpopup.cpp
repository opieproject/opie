/* $Id: panpopup.cpp,v 1.9 2006-04-04 12:15:10 korovkin Exp $ */
/* PAN context menu */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qpe/qcopenvelope_qws.h>
#include <qmessagebox.h>
#include <opie2/odebug.h>
#include <opie2/obluetooth.h>
#include <opie2/obluetoothdevicehandler.h>

using namespace Opie::Core;
using namespace Opie::Bluez;

#include <qtimer.h>

#include "panpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
PanPopup::PanPopup( const OBluetoothServices& service, BTDeviceItem* item, DeviceHandlerPool *devHandlerPool )
    : QPopupMenu(), m_service(service)
{
    owarn << "PanPopup c'tor" << oendl;

    m_item = item;
    QAction *a, *b, *c;

    /* connect action */
    a = new QAction(); // so it's get deleted
    a->setText( tr("connect") );
    a->addTo( this );
    connect( a, SIGNAL( activated() ), this, SLOT( slotConnect() ) );


    b = new QAction();
    b->setText( tr( "connect+conf" ) );
    b->addTo( this );
    connect( b, SIGNAL( activated() ), this, SLOT( slotConnectAndConfig() ) );

    c = new QAction();
    c->setText( tr( "disconnect" ) );
    c->addTo( this );
    connect( c, SIGNAL( activated() ), this, SLOT( slotDisconnect() ) );    

    OBluetooth *bt = OBluetooth::instance();
    OBluetoothInterface *intf = bt->defaultInterface();
    OBluetoothDevice *dev = intf->findDevice( m_item->mac() );

    if( dev ) {
        m_devHandler = (NetworkDeviceHandler *)devHandlerPool->getHandler( dev, NetworkDeviceHandler::dbusInterface() );
    }
    else {
        m_devHandler = NULL;
        odebug << "no device" << oendl;
    }
}

PanPopup::~PanPopup()
{
}

void PanPopup::slotConnect()
{
    QMap<int, QString> list = m_service.classIdList();
    QMap<int, QString>::Iterator it = list.begin();
    QString pattern = "";
    if ( it != list.end() ) {
        switch( it.key() ) {
            case 0x1115:
                pattern = "panu";
                break;
            case 0x1116:
                pattern = "nap";
                break;
            case 0x1117:
                pattern = "gn";
                break;
        }
    }
    odebug << "PanPopup::slotConnect pattern=" << pattern << oendl;
    if( !pattern.isEmpty() )
        m_devHandler->connect( pattern );
}

void PanPopup::slotDisconnect()
{
    m_devHandler->disconnect();
}

void PanPopup::slotConnectAndConfig()
{
    slotConnect();

    // more intelligence here later like passing the device ( bnepX )
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "networksettings" );
}
