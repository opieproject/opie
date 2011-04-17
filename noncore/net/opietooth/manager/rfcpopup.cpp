
#include "pppdialog.h"
#include "rfcpopup.h"
#include "rfcommassigndialogimpl.h"

/* OPIE */
#include <qpe/qpeapplication.h>
#include <opie2/odebug.h>
#include <opie2/oprocess.h>

using namespace Opie::Core;

/* QT */
#include <qtimer.h>

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
RfcCommPopup::RfcCommPopup(const Opie::Bluez::OBluetoothServices& service,
  OpieTooth::BTDeviceItem* item)
        : QPopupMenu(), m_service(service)
{
    owarn << "RfcCommPopup c'tor" << oendl;

    QAction* a;
    int port = service.protocolDescriptorList().last().port();
    QString mac = item->mac();
    unsigned int i;

    procId = -1;
    m_item = item;

    for (i = 0; i < sizeof(PPPDialog::conns) / sizeof(Connection); i++) {
      if (PPPDialog::conns[i].port == port &&
          PPPDialog::conns[i].btAddr == mac &&
          PPPDialog::conns[i].proc.isRunning()) {
          /* disconnect action */
          a = new QAction(  );
          a->setText("Disconnect");
          a->addTo( this );
          connect( a, SIGNAL( activated() ) ,
                this, SLOT( slotDisconnect() ) );
          procId = i;
          break;
      }
    }
    if (procId == -1) {
        /* connect action */
        a = new QAction(  ); // so it's get deleted
        a->setText("Connect");
        a->addTo( this );
        connect( a,  SIGNAL( activated() ),
                this, SLOT( slotConnect() ) );
    }

    /* Bind action */
    a = new QAction(  );
    a->setText("Bind table");
    a->addTo( this );
    connect( a, SIGNAL( activated() ),
             this, SLOT( slotBind() ) );

};


RfcCommPopup::~RfcCommPopup()
{
    /*  delete m_con;
        delete m_dis;
        delete m_bind; */
}


void RfcCommPopup::slotConnect()
{

    owarn << "connect" << oendl;

    PPPDialog pppDialog(m_item->mac(),
      m_service.protocolDescriptorList().last().port());
    QPEApplication::execDialog( &pppDialog );
}


void RfcCommPopup::slotDisconnect()
{
    owarn << "slot disconnected " << procId << oendl;
    if (procId >= 0)
        PPPDialog::conns[procId].proc.kill();
}


void RfcCommPopup::slotBind()
{
    RfcommAssignDialog rfcommAssign ( this, "RfcommAssignDialog", true, WStyle_ContextHelp  );

    rfcommAssign.newDevice( m_item->mac(),
        m_service.protocolDescriptorList().last().port());

    if ( QPEApplication::execDialog( &rfcommAssign ) == QDialog::Accepted )
    {
        rfcommAssign.saveConfig();
    }
}
