/*
 * GPLv2
 * zecke@handhelds.org
 */

#include "dir_ifaceinfo.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
using namespace Opie::Core;

/* QT */
#include <qwidget.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qlabel.h>

namespace {
    class DirImageWidget : public QHBox {
    public:
        DirImageWidget() {
            chkbox = new QCheckBox( QObject::tr("Show all files"), this );
        }
        ~DirImageWidget() {}
        QCheckBox* chkbox;
    };
}


DirInterfaceInfo::DirInterfaceInfo() {
}
DirInterfaceInfo::~DirInterfaceInfo() {
}

QString DirInterfaceInfo::name()const {
    return QString::fromLatin1(QObject::tr("DirView" ));
}

QWidget* DirInterfaceInfo::configWidget(const Config& cfg) {
    DirImageWidget* wid = new DirImageWidget();
    wid->chkbox->setChecked(  cfg.readBoolEntry("Dir_Check_All_Files", true) );

    return wid;
}

void DirInterfaceInfo::writeConfig( QWidget* _wid, Config& cfg) {
    owarn << "Write Config" << oendl; 
    DirImageWidget* wid = static_cast<DirImageWidget*>(_wid);
    cfg.writeEntry("Dir_Check_All_Files", wid->chkbox->isChecked() );
}
