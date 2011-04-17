/* $Id: dunpopup.cpp,v 1.6 2006-04-04 18:53:07 korovkin Exp $ */
/* DUN context menu */
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
#include <opie2/oprocess.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

#include "dunpopup.h"
#include "dundialog.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
DunPopup::DunPopup( const Opie::Bluez::OBluetoothServices& service,
    BTDeviceItem* item ) : QPopupMenu(), m_service(service) {

    owarn << "DunPopup c'tor" << oendl;

    m_item = item;
    QAction *a, *b, *c;

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

};

DunPopup::~DunPopup() {

}

void DunPopup::slotConnect() {
    odebug << "connect" << oendl;
    DunDialog dundlg(m_item->mac(),
        m_service.protocolDescriptorList().last().port());
    QPEApplication::execDialog( &dundlg );
}

void DunPopup::slotDisconnect()  {
    OProcess dunDis;
    OProcess pppDis;
    OProcess dunKill;
    dunDis << "dund" << "--kill" << m_item->mac();
    dunDis.start(OProcess::DontCare, OProcess::NoCommunication);
    dunKill << "killall" << "-q" << "dund";
    dunKill.start(OProcess::DontCare, OProcess::NoCommunication);
    pppDis << "killall" << "-q" << "pppd";
    pppDis.start(OProcess::DontCare, OProcess::NoCommunication);
    sleep(1);
    QMessageBox::information(this, tr("DUN Disconnect"), tr("DUN Disconnected"));
}


void DunPopup::slotConnectAndConfig() {
    slotConnect();

    // more intelligence here later like passing the device ( bnepX )
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "networksettings" );
}
