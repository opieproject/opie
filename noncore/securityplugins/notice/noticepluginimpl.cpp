#include "noticepluginimpl.h"

NoticePluginImpl::NoticePluginImpl() {
    noticePlugin  = new NoticePlugin();
}

NoticePluginImpl::~NoticePluginImpl() {
    delete noticePlugin;
}

MultiauthPluginObject* NoticePluginImpl::plugin() {
    return noticePlugin;
}

    Q_EXPORT_INTERFACE() {
        Q_CREATE_INSTANCE( NoticePluginImpl )
    }

QRESULT NoticePluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_MultiauthPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    } else {
        return QE_NOINTERFACE;
    }
    return QS_OK;

}
