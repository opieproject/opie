#include <stdio.h>
#include <qlabel.h>
#include <qbutton.h>
#include <qwindowsystem_qws.h>
#include "zkbapplet.h"
#include "zkbwidget.h"


ZkbApplet::ZkbApplet() : app(0){
}

ZkbApplet::~ZkbApplet()
{
    if (app) delete app;
}

QWidget* ZkbApplet::applet(QWidget* parent) {
	if (app == 0) {
		app = new ZkbWidget(parent);
	}

	return app;
}

int ZkbApplet::position() const {
	return 8;
}

QRESULT ZkbApplet::queryInterface(const QUuid& uuid, QUnknownInterface** iface) {

	*iface = 0;
	if (uuid == IID_QUnknown) {
		*iface = this;
	} else if (uuid == IID_TaskbarApplet) {
		*iface = this;
	} else {
                return QS_FALSE;
        }

	if (*iface) {
		(*iface)->addRef();
	}

	return QS_OK;
}

Q_EXPORT_INTERFACE() {
	Q_CREATE_INSTANCE(ZkbApplet)
}
