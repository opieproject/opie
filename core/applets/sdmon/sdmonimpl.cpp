#include "sdmon.h"
#include "sdmonimpl.h"


SdMonitorImpl::SdMonitorImpl()
    : sdMonitor(0), ref(0)
{
}

SdMonitorImpl::~SdMonitorImpl()
{
    delete sdMonitor;
}

QWidget *SdMonitorImpl::applet( QWidget *parent )
{
    if ( !sdMonitor )
	  sdMonitor = new SdMonitor( parent );
    return sdMonitor;
}

int SdMonitorImpl::position() const
{
    return 5;
}

QRESULT SdMonitorImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
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
    Q_CREATE_INSTANCE( SdMonitorImpl )
}
