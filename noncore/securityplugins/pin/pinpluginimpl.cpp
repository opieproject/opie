#include "pinpluginimpl.h"

PinPluginImpl::PinPluginImpl() {
    pinPlugin  = new PinPlugin();
}

PinPluginImpl::~PinPluginImpl() {
    delete pinPlugin;
}

MultiauthPluginObject* PinPluginImpl::plugin() {
    return pinPlugin;
}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( PinPluginImpl )
}

QRESULT PinPluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_MultiauthPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    } else {
        return QE_NOINTERFACE;
    }
    return QS_OK;

}
