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
#include <qspinbox.h>

namespace {
    class DirImageWidget : public QFrame {
    public:
        DirImageWidget(): QFrame() {
            setFrameStyle(Box|Raised);
            QVBoxLayout *m_MainLayout = new QVBoxLayout( this, 6, 2, "m_MainLayout");
            QGridLayout*RecDepthLayout = new QGridLayout( 0, 1, 1, 0, 6, "RecDepthLayout");

            chkbox = new QCheckBox( QObject::tr("Show all files"), this );
            m_MainLayout->addWidget(chkbox);
            recWarningLabel = new QLabel(this);
            recWarningLabel->setText(QObject::tr("<center><b>Be carefull with the following options!</b></center>"));
            m_MainLayout->addWidget(recWarningLabel);
            recBox = new QCheckBox( QObject::tr("Show files recursive"),this);
            m_MainLayout->addWidget(recBox);
            recDepthLabel = new QLabel(this);
            recDepthLabel->setText(QObject::tr("Recursion depth:"));
            RecDepthLayout->addWidget(recDepthLabel,0,0);
            recDepth = new QSpinBox(this);
            recDepth->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
            recDepth->setMaxValue(10);
            recDepth->setMinValue(1);
            recDepth->setSuffix(QObject::tr(" directories"));
            RecDepthLayout->addWidget(recDepth,0,1);
            m_MainLayout->addLayout(RecDepthLayout);
            QSpacerItem *spacer1 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
            m_MainLayout->addItem( spacer1 );

        }
        ~DirImageWidget() {}
        QCheckBox* chkbox,*recBox;
        QSpinBox * recDepth;
        QLabel* recDepthLabel,*recWarningLabel;

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
    wid->chkbox->setChecked(cfg.readBoolEntry("Dir_Check_All_Files",false));
    wid->recBox->setChecked(cfg.readBoolEntry("Dir_Check_Recursive_Files",false));
    wid->recDepth->setValue(cfg.readNumEntry("Dir_Recursive_Files_Depth",10));
    return wid;
}

void DirInterfaceInfo::writeConfig( QWidget* _wid, Config& cfg) {
    owarn << "Write Config" << oendl;
    DirImageWidget* wid = static_cast<DirImageWidget*>(_wid);
    cfg.writeEntry("Dir_Check_All_Files", wid->chkbox->isChecked());
    cfg.writeEntry("Dir_Check_Recursive_Files", wid->recBox->isChecked());
    cfg.writeEntry("Dir_Recursive_Files_Depth",wid->recDepth->value());
}
