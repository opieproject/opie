#include "dummypluginimpl.h"

DummyPluginImpl::DummyPluginImpl() {
    dummyPlugin  = new DummyPlugin();
}

DummyPluginImpl::~DummyPluginImpl() {
    delete dummyPlugin;
}

MultiauthPluginObject* DummyPluginImpl::plugin() {
    return dummyPlugin;
}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( DummyPluginImpl )
}

QRESULT DummyPluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_MultiauthPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    } else {
        return QE_NOINTERFACE;
    }
    return QS_OK;

}
