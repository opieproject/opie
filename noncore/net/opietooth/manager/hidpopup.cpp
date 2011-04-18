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
#include <qpe/qpeapplication.h>
using namespace Opie::Core;
using namespace Opie::Bluez;

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>
#include <dbus/qdbusdatamap.h>

#include <qtimer.h>

#include "hidpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
HidPopup::HidPopup(const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem* item) :
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

};

HidPopup::~HidPopup()
{
}

void HidPopup::slotConnect()
{
    odebug << "connect" << oendl;
    OBluetooth *bt = OBluetooth::instance();
    OBluetoothInterface *intf = bt->defaultInterface();
    OBluetoothDevice *dev = intf->findDevice( m_item->mac() );
    if( dev ) {
        QDBusConnection connection = QDBusConnection::systemBus();
        QDBusProxy *devProxy = new QDBusProxy(0);
        devProxy->setService("org.bluez");
        devProxy->setPath(dev->devicePath());
        devProxy->setInterface("org.bluez.Input");
        devProxy->setConnection(connection);
        QDBusMessage reply = devProxy->sendWithReply("Connect", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage) {
            odebug << "********* connect success" << oendl;
        }
        else if (reply.type() == QDBusMessage::ErrorMessage)
            odebug << "********* connect fail: " << reply.error().name() << ": " << reply.error().message() << oendl;
    }
    else {
        odebug << "no device" << oendl;
    }
}

void HidPopup::slotDisconnect()
{
    odebug << "disconnect" << oendl;
    OBluetooth *bt = OBluetooth::instance();
    OBluetoothInterface *intf = bt->defaultInterface();
    OBluetoothDevice *dev = intf->findDevice( m_item->mac() );
    if( dev ) {
        QDBusConnection connection = QDBusConnection::systemBus();
        QDBusProxy *devProxy = new QDBusProxy(0);
        devProxy->setService("org.bluez");
        devProxy->setPath(dev->devicePath());
        devProxy->setInterface("org.bluez.Input");
        devProxy->setConnection(connection);
        QDBusMessage reply = devProxy->sendWithReply("Disconnect", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage ) {
            odebug << "********* disconnect success" << oendl;
        }
        else if (reply.type() == QDBusMessage::ErrorMessage)
            odebug << "********* disconnect fail: " << reply.error().name() << ": " << reply.error().message() << oendl;
    }
    else {
        odebug << "no device" << oendl;
    }
}
