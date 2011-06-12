
#include "pppdialog.h"
#include "rfcpopup.h"
#include "rfcommassigndialogimpl.h"

/* OPIE */
#include <qpe/qpeapplication.h>
#include <opie2/odebug.h>
#include <opie2/oprocess.h>
#include <opie2/obluetooth.h>
#include <opie2/obluetoothdevicehandler.h>
using namespace Opie::Core;
using namespace Opie::Bluez;

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
RfCommPopup::RfCommPopup(const Opie::Bluez::OBluetoothServices& service,
  OpieTooth::BTDeviceItem* item, Opie::Bluez::DeviceHandlerPool *devHandlerPool)
        : QPopupMenu(), m_service(service)
{
    owarn << "RfCommPopup c'tor" << oendl;

    QAction* a;

    m_item = item;

    /* connect action */
    a = new QAction(  );
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

    /* Bind action */
    a = new QAction(  );
    a->setText("Bind table");
    a->addTo( this );
    connect( a, SIGNAL( activated() ),
             this, SLOT( slotBind() ) );


    OBluetooth *bt = OBluetooth::instance();
    OBluetoothInterface *intf = bt->defaultInterface();
    OBluetoothDevice *dev = intf->findDevice( m_item->mac() );

    if( dev ) {
        m_devHandler = (SerialDeviceHandler *)devHandlerPool->getHandler( dev, SerialDeviceHandler::dbusInterface() );
    }
    else {
        m_devHandler = NULL;
        odebug << "no device" << oendl;
    }
}

RfCommPopup::~RfCommPopup()
{
}

void RfCommPopup::slotConnect()
{
    m_devHandler->connect( QString::number( m_service.protocolDescriptorList().last().port() ) );
}

void RfCommPopup::slotDisconnect()
{
    m_devHandler->disconnect();
}

void RfCommPopup::slotBind()
{
    RfcommAssignDialog rfcommAssign ( this, "RfcommAssignDialog", true, WStyle_ContextHelp  );

    rfcommAssign.newDevice( m_item->mac(),
        m_service.protocolDescriptorList().last().port());

    if ( QPEApplication::execDialog( &rfcommAssign ) == QDialog::Accepted )
    {
        rfcommAssign.saveConfig();
    }
}
