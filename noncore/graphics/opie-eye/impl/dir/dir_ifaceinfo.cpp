/*
 * GPLv2
 * zecke@handhelds.org
 */

#include "dir_ifaceinfo.h"
#include "dir_setup.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>

/* QT */
#include <qwidget.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>

DirInterfaceInfo::DirInterfaceInfo() {
}

DirInterfaceInfo::~DirInterfaceInfo() {
}

QString DirInterfaceInfo::name()const {
    return QObject::tr("Directory View" );
}

QWidget* DirInterfaceInfo::configWidget(const Config& cfg) {
    DirImageWidget* wid = new DirImageWidget();
    wid->chkbox->setChecked(cfg.readBoolEntry("Dir_Check_All_Files",false));
    wid->recBox->setChecked(cfg.readBoolEntry("Dir_Check_Recursive_Files",false));
    wid->recDepth->setValue(cfg.readNumEntry("Dir_Recursive_Files_Depth",10));
    wid->recDepth->setEnabled(wid->recBox->isChecked());
    return wid;
}

void DirInterfaceInfo::writeConfig( QWidget* _wid, Config& cfg) {
    owarn << "Write Config" << oendl;
    DirImageWidget* wid = static_cast<DirImageWidget*>(_wid);
    cfg.writeEntry("Dir_Check_All_Files", wid->chkbox->isChecked());
    cfg.writeEntry("Dir_Check_Recursive_Files", wid->recBox->isChecked());
    cfg.writeEntry("Dir_Recursive_Files_Depth",wid->recDepth->value());
}
