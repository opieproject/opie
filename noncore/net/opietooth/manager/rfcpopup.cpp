
#include <qtimer.h>

#include "pppdialog.h"
#include "rfcpopup.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
RfcCommPopup::RfcCommPopup()
    : QPopupMenu()  {
    qWarning("RfcCommPopup c'tor");

    QAction* a;

    /* connect action */
    a = new QAction(  ); // so it's get deleted
    a->setText("Connect");
    a->addTo( this );
    connect( a,  SIGNAL( activated() ),
             this, SLOT( slotConnect() ) );


    /* disconnect action */
    a = new QAction(  );
    a->setText("Disconnect");
    a->addTo( this );
    connect( a, SIGNAL( activated() ) ,
            this, SLOT( slotDisconnect() ) );


    /* foo action */
    a = new QAction(  );
    a->setText("Foo");
    a->addTo( this );
    connect( a, SIGNAL( activated() ),
            this, SLOT( slotFoo() ) );


    /* bar action */
    a = new QAction(  );
    a->setText( "Bar" );
    a->addTo( this );
    connect( a, SIGNAL( activated() ),
            this, SLOT( slotBar() ) );

};


RfcCommPopup::~RfcCommPopup() {
/*  delete m_con;
    delete m_dis;
    delete m_foo;
    delete m_bar; */
}


void RfcCommPopup::slotConnect() {
    qWarning("connect");
    PPPDialog pppDialog;
    pppDialog.showMaximized();
    pppDialog.exec();
}


void RfcCommPopup::slotDisconnect() {
    qWarning("slot disconnected");
}


void RfcCommPopup::slotFoo() {
    qWarning("slotFoo");
}


void RfcCommPopup::slotBar() {
    qWarning("slotBar");
};

