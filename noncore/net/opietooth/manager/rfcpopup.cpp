
#include <qtimer.h>

#include "pppdialog.h"
#include "rfcpopup.h"
#include "rfcommassigndialogimpl.h"

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
RfcCommPopup::RfcCommPopup( OpieTooth::BTDeviceItem* item  )
    : QPopupMenu()  {
    qWarning("RfcCommPopup c'tor");

    QAction* a;


    m_item = item;

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
    a->setText("Bind table");
    a->addTo( this );
    connect( a, SIGNAL( activated() ),
            this, SLOT( slotBind() ) );


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


void RfcCommPopup::slotBind() {
    RfcommAssignDialog rfcommAssign ( this, "RfcommAssignDialog", true, WStyle_ContextHelp  );

    rfcommAssign.showMaximized();
    rfcommAssign.newDevice( m_item->mac() );

    if ( rfcommAssign.exec() == QDialog::Accepted )  {
        rfcommAssign.saveConfig();
    }
}


void RfcCommPopup::slotBar() {
    qWarning("slotBar");
};

