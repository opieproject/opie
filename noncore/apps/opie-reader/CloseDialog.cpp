#include "CloseDialog.h"
#include "qlayout.h"

CCloseDialog::CCloseDialog(const QString& fname, bool fs, QWidget* parent, const char* name) : QDialog(parent, name, true)
{
    setCaption(tr("Tidy-up"));
    QVBoxLayout *tmp = new QVBoxLayout(this);
    QVButtonGroup* vb = new QVButtonGroup(tr("Delete"), this);
    tmp->addWidget(vb);
    QString filestring = tr("Delete") + " " + fname;
    file = new QCheckBox(filestring, vb);
    marks = new QCheckBox(tr("Delete Bookmarks"), vb);
    config = new QCheckBox(tr("Delete Configuration"), vb);
    file->setChecked(false);
    marks->setChecked(true);
    config->setChecked(true);
    if (fs) showMaximized();
}
