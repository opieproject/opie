/*
 * GPLv2
 * zecke@handhelds.org
 */

#include "doc_ifaceinfo.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
using namespace Opie::Core;

/* QT */
#include <qwidget.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qlabel.h>

#if 0
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
#endif

DocInterfaceInfo::DocInterfaceInfo() {
}
DocInterfaceInfo::~DocInterfaceInfo() {
}

QString DocInterfaceInfo::name()const {
    return QString::fromLatin1(QObject::tr("DocView" ));
}

QWidget* DocInterfaceInfo::configWidget(const Config& cfg) {
#if 0
    DirImageWidget* wid = new DirImageWidget();
    wid->chkbox->setChecked(  cfg.readBoolEntry("Dir_Check_All_Files", true) );

    return wid;
#endif
    return 0L;
}

void DocInterfaceInfo::writeConfig( QWidget* , Config& ) {
#if 0
    owarn << "Write Config" << oendl; 
    DirImageWidget* wid = static_cast<DirImageWidget*>(_wid);
    cfg.writeEntry("Dir_Check_All_Files", wid->chkbox->isChecked() );
#endif
}
