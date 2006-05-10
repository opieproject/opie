
#include "obexdialog.h"
#include "obexpopup.h"

/* OPIE */
#include <qpe/qpeapplication.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qtimer.h>

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
ObexPopup::ObexPopup(const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item)
        : QPopupMenu(), m_service(service)
{
    odebug << "ObexPopup c'tor" << oendl; 

    m_item = item;
    /* connect action */
    m_push = new QAction( ); // so it's get deleted
    m_push->setText("Push file");
    m_push->addTo( this );
    connect(m_push, SIGNAL(activated()), SLOT(slotPush()));
}


ObexPopup::~ObexPopup()
{
    delete m_push;
}


void ObexPopup::slotPush()
{
    QString device = m_item->mac();
    int port = m_service.protocolDescriptorList().last().port();
    odebug << "push something to " << device << " " << port << oendl; 
    ObexDialog obexDialog(device, port);
    QPEApplication::execDialog( &obexDialog );
}


