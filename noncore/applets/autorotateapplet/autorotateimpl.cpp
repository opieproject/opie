#include "autorotate.h"
#include "autorotateimpl.h"


AutoRotateImpl::AutoRotateImpl()
    : autoRotate(0), ref(0) {
        qDebug ("here");
}

AutoRotateImpl::~AutoRotateImpl() {
    delete autoRotate;
}

QWidget *AutoRotateImpl::applet( QWidget *parent ) {
    if ( !autoRotate ) {
	autoRotate = new AutoRotate( parent );
    }
    return autoRotate;
}

int AutoRotateImpl::position() const {
    return 7;
}

QRESULT AutoRotateImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
    *iface = 0;
    if ( uuid == IID_QUnknown ) {
	*iface = this;
    } else if ( uuid == IID_TaskbarApplet ) {
	*iface = this;
    }

    if ( *iface ) {
	(*iface)->addRef();
    }
    return QS_OK;
}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( AutoRotateImpl )
}
