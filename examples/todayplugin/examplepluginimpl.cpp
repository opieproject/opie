/*
 * examplepluginimpl.cpp
 *
 * email       : harlekin@handhelds.org
 *
 */



#include "exampleplugin.h"
#include "examplepluginimpl.h"

ExamplePluginImpl::ExamplePluginImpl() {
    examplePlugin  = new ExamplePlugin();
}

ExamplePluginImpl::~ExamplePluginImpl() {
    delete examplePlugin;
}


TodayPluginObject* ExamplePluginImpl::guiPart() {
    return examplePlugin;
}

QRESULT ExamplePluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_TodayPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    }else
	return QS_FALSE;

    return QS_OK;

}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( ExamplePluginImpl );
}
