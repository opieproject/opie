#include "obexhandler.h"
#include "obeximpl.h"

using namespace OpieObex;

/* TRANSLATOR OpieObex::ObexImpl */

ObexImpl::ObexImpl() {
    m_handler = new ObexHandler;
}
ObexImpl::~ObexImpl() {
    delete m_handler;
}
QRESULT ObexImpl::queryInterface( const QUuid& uuid, QUnknownInterface **iface ) {
    *iface = 0;
    if ( uuid == IID_QUnknown ) {
        *iface = this;
    }else if ( uuid == IID_ObexInterface )
        *iface = this;

    if (*iface)
        (*iface)->addRef();

    return QS_OK;
}


Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( ObexImpl )
}
