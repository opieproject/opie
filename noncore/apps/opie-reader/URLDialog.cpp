#include "URLDialog.h"
#include "qlayout.h"

#include <qpe/qpeapplication.h>

CURLDialog::CURLDialog(const QString& fname, bool fs, QWidget* parent, const char* name) : QDialog(parent, name, true)
{
    setCaption(tr("Save URL"));
    QVBoxLayout *tmp = new QVBoxLayout(this);
    QVButtonGroup* vb = new QVButtonGroup(fname, this);
    tmp->addWidget(vb);
    m_clipboard = new QCheckBox(tr("Clipboard"), vb);
    m_localfile = new QCheckBox(tr("Local file"), vb);
    m_globalfile = new QCheckBox(tr("Global file"), vb);
    if (fs)
        QPEApplication::showDialog( this );
}
