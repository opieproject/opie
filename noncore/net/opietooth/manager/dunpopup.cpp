#include <qpe/qcopenvelope_qws.h>

#include <qtimer.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

#include "dunpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
DunPopup::DunPopup( OpieTooth::BTDeviceItem* item ) : QPopupMenu()  {

    owarn << "DunPopup c'tor" << oendl; 

    m_item = item;
    QAction *a, *b, *c;

    m_dunconnection = 0l;
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

DunPopup::~DunPopup() {

}

void DunPopup::slotConnect() {

    m_dunconnection = new StartDunConnection( m_item->mac() );
    m_dunconnection->start();
}

void DunPopup::slotDisconnect()  {
    m_dunconnection->stop();
}


void DunPopup::slotConnectAndConfig() {
    slotConnect();

    // more intelligence here later like passing the device ( bnepX )
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "networksettings" );

}
