#include <qpe/qcopenvelope_qws.h>

#include <qtimer.h>

#include "panpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
PanPopup::PanPopup( OpieTooth::BTDeviceItem* item ) : QPopupMenu()  {

    owarn << "PanPopup c'tor" << oendl; 

    m_item = item;
    QAction *a, *b, *c;

    m_panconnection = 0l;
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

};

PanPopup::~PanPopup() {

}

void PanPopup::slotConnect() {

    m_panconnection = new StartPanConnection( m_item->mac() );
    m_panconnection->start();
}

void PanPopup::slotDisconnect()  {
    if (!m_panconnection) m_panconnection = new StartPanConnection( m_item->mac() );
    m_panconnection->stop();
}


void PanPopup::slotConnectAndConfig() {
    slotConnect();

    // more intelligence here later like passing the device ( bnepX )
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "networksettings" );

}
