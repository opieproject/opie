/*
 * GPLv2
 * zecke@handhelds.org
 */

#include "dir_ifaceinfo.h"

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

namespace {
    class DirImageWidget : public QFrame {
    public:
        DirImageWidget(): QFrame() {
            setFrameStyle(Box|Raised);
            QVBoxLayout *m_MainLayout = new QVBoxLayout( this, 6, 2, "m_MainLayout");
            chkbox = new QCheckBox( QObject::tr("Show all files"), this );
            m_MainLayout->addWidget(chkbox);
            QSpacerItem *spacer1 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
            m_MainLayout->addItem( spacer1 );

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
    return QObject::tr("Directory View" );
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
