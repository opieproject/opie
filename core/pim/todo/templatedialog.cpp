#include <qpushbutton.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qlineedit.h>

#include "templatedialog.h"


using namespace Todo;

TemplateDialog::TemplateDialog( QWidget* widget )
    : QDialog( widget, "TemplateDialog", TRUE )
{
    setCaption( tr("Template Editor") );
    m_main = new QVBoxLayout(this );

    m_list = new QListView( this );
    m_main->addWidget( m_list, 100 );

    m_lne = new QLineEdit( this );
    m_main->addWidget( m_lne );

    m_btnBar = new QHBox( this );
    m_add = new QPushButton( tr("Add"), m_btnBar );
    m_edit = new QPushButton( tr("Edit"), m_btnBar );
    m_rem = new QPushButton( tr("Remove"), m_btnBar );
    m_main->addWidget( m_btnBar );

    connect(m_add,  SIGNAL(clicked() ),
            this,   SLOT(slotAdd() ) );
    connect(m_edit, SIGNAL(clicked() ),
            this,   SLOT(slotEdit() ) );
    connect(m_rem,  SIGNAL(clicked() ),
            this,   SLOT(slotEdit() ) );
    connect(m_lne, SIGNAL(returnPressed() ),
            this,   SLOT(slotReturn() ) );

}
TemplateDialog::~TemplateDialog() {
    // Qt does delete our widgets
}
QListView* TemplateDialog::listView() {
    return m_list;
}
QLineEdit* TemplateDialog::edit() {
    return m_lne;
}
void TemplateDialog::slotAdd() {
    qWarning("Not Implemented here");
}
void TemplateDialog::slotRemove() {
    qWarning("Not Implemented here");
}
void TemplateDialog::slotEdit() {
    qWarning("Not Implemented here");
}
void TemplateDialog::slotReturn() {
    qWarning("Not Implemented here");
}
