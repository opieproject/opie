#include <qaction.h>
#include <qpopupmenu.h>

#include "mainwindow.h"
#include "todotemplatemanager.h"

#include "templatedialogimpl.h"
#include "templateeditor.h"

using namespace Todo;

TemplateEditor::TemplateEditor( MainWindow* main,
                                TemplateManager* manager )
    : QObject( main ), m_main( main ), m_man( manager )
{
    init();

}
TemplateEditor::~TemplateEditor() {

}
/* ok we add us to the Menubar */
void TemplateEditor::init() {
    QAction* a = new QAction( QString::null, tr("Configure Templates"),
                              0, this, 0, FALSE );
    connect(a, SIGNAL(activated() ),
            this, SLOT(setUp() ) );

    a->addTo( m_main->options() );
}
void TemplateEditor::setUp() {
    qWarning("set up");
    TemplateDialogImpl dlg(m_main, m_man );
    int ret= dlg.exec();
    if (QDialog::Accepted != ret ) {
        m_man->load();
    }else
        m_main->populateTemplates();
}
