#include "bluepingpluginimpl.h"

BluepingPluginImpl::BluepingPluginImpl() {
    bluepingPlugin  = new BluepingPlugin();
}

BluepingPluginImpl::~BluepingPluginImpl() {
    delete bluepingPlugin;
}

MultiauthPluginObject* BluepingPluginImpl::plugin() {
    return bluepingPlugin;
}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( BluepingPluginImpl )
}

QRESULT BluepingPluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_MultiauthPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    } else {
        return QE_NOINTERFACE;
    }
    return QS_OK;

}
