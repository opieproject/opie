#include "KeyHelperApplet.h"
#include "KeyHelperWidget.h"

#if 0
KeyHelperApplet::KeyHelperApplet()
    : widget(0), ref(0)
{
    qDebug("KeyHelperApplet::KeyHelperApplet()");
}

KeyHelperApplet::~KeyHelperApplet()
{
    qDebug("KeyHelperApplet::~KeyHelperApplet()");
    delete widget;
}

QWidget* KeyHelperApplet::applet(QWidget* parent)
{
    if(!widget){
        widget = new KeyHelperWidget(parent);
    }
    return(widget);
}

int KeyHelperApplet::position() const
{
    return(6);
}

QRESULT KeyHelperApplet::queryInterface(const QUuid& uuid,
    QUnknownInterface** iface)
{
    *iface = 0;

    if(QFile::exists("/tmp/disable-keyhelper")
        || QFile::exists("/mnt/card/disable-keyhelper")
        || QFile::exists("/mnt/cf/disable-keyhelper")){
        return QS_FALSE;
    }

    if(uuid == IID_QUnknown){
        *iface = this;
    } else if(uuid == IID_TaskbarApplet){
        *iface = this;
    }

    if(*iface){
        (*iface)->addRef();
    }
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE(KeyHelperApplet)
}

#else
#include <opie2/otaskbarapplet.h>

using namespace Opie::Ui;

EXPORT_OPIE_APPLET_v1( KeyHelperWidget )

#endif
