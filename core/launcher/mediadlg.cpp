#include "mediadlg.h"
#include "mediumwidget.h"
#include <qtopia/storage.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qlabel.h>

Mediadlg::Mediadlg(FileSystem*fs,QWidget *parent, const char *name,  bool modal, WFlags fl)
    : QDialog(parent,name,modal,fl)
{
    m_fs = fs;
    m_widget = 0;
    init();
}

Mediadlg::~Mediadlg()
{
}

void Mediadlg::init()
{
    if (!m_fs) return;
    m_lay = new QVBoxLayout( this);
    m_lay->setSpacing(0);
    m_lay->setMargin(0);
    QLabel*tLabel = new QLabel(this);
    tLabel->setText("<center>"+tr("A new storage media detected:")+"<br>"+m_fs->name()+"<br>"+tr("What should I do with it?")+"</center>");
    m_lay->addWidget(tLabel);
    m_widget = new MediumMountSetting::MediumMountWidget(m_fs->path(),QPixmap(),this);
    m_lay->addWidget(m_widget);
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_lay->addItem(spacer);
}

void Mediadlg::accept()
{
    if (m_widget) m_widget->writeConfig();
    QDialog::accept();
}
