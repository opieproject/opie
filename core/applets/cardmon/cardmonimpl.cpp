#include "cardmon.h"
#include "cardmonimpl.h"


CardMonitorImpl::CardMonitorImpl()
    : cardMonitor(0), ref(0)
{
}

CardMonitorImpl::~CardMonitorImpl()
{
    delete cardMonitor;
}

QWidget *CardMonitorImpl::applet( QWidget *parent )
{
    if ( !cardMonitor )
	cardMonitor = new CardMonitor( parent );
    return cardMonitor;
}

int CardMonitorImpl::position() const
{
    return 7;
}

QRESULT CardMonitorImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_TaskbarApplet )
	*iface = this;

    if ( *iface )
	(*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( CardMonitorImpl )
}
