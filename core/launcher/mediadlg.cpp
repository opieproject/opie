#include "mediadlg.h"
#include "mediumwidget.h"
#include <qtopia/storage.h>
#include <qpixmap.h>
#include <qlayout.h>

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
    m_lay = new QVBoxLayout( this );
    m_widget = new MediumMountSetting::MediumMountWidget(m_fs->path(),QPixmap(),this);
    m_lay->addWidget(m_widget);
}

void Mediadlg::accept()
{
    if (m_widget) m_widget->writeConfig();
    QDialog::accept();
}
