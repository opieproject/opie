#include <qpe/qcopenvelope_qws.h>

#include <qtimer.h>

#include "panpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
PanPopup::PanPopup( OpieTooth::BTDeviceItem* item ) : QPopupMenu()  {

    qWarning("PanPopup c'tor");

    m_item = item;
    m_panconnect = 0l;
    QAction *a, *b;

    /* connect action */
    a = new QAction( ); // so it's get deleted
    a->setText( "connect" );
    a->addTo( this );
    connect( a, SIGNAL( activated() ), this, SLOT( slotConnect() ) );

    b = new QAction();
    b->setText( "connect+conf" );
    b->addTo( this );
    connect( b, SIGNAL( activated() ), this, SLOT( slotConnectAndConfig() ) );
};

PanPopup::~PanPopup() {
    delete m_panconnect;
}

void PanPopup::slotConnect() {


    // SHOULD move to lib
    // before pand must be in "pand --listen --role panu" mode ( client )

    m_panconnect = new OProcess();
    *m_panconnect << "pand" << "--connect" << m_item->mac();

    connect( m_panconnect, SIGNAL( processExited( OProcess* ) ) ,
             this, SLOT( slotConnectExited( OProcess* ) ) );
    connect( m_panconnect, SIGNAL( receivedStdout( OProcess*, char*, int ) ),
             this, SLOT( slotConnectOut( OProcess*, char*, int ) ) );
    if (!m_panconnect->start( OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        qWarning( "could not start" );
        delete m_panconnect;
    }
}

void PanPopup::slotExited( OProcess* proc ) {
    delete m_panconnect;
}

void PanPopup::slotStdOut(OProcess* proc, char* chars, int len)
{}


void PanPopup::slotConnectAndConfig() {
    slotConnect();

    // more intelligence here later like passing the device ( bnepX )
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "networksetup" );

}
