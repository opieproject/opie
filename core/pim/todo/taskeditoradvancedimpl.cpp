#include <qmultilineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>


#include <opie/otodo.h>
#include <opie/opimmaintainer.h>
#include <opie/opimstate.h>

#include "taskeditoradvancedimpl.h"


TaskEditorAdvancedImpl::TaskEditorAdvancedImpl( QWidget* parent, const char* name )
    : TaskEditorAdvanced( parent, name ) {
    initUI();
}
TaskEditorAdvancedImpl::~TaskEditorAdvancedImpl() {
}
void TaskEditorAdvancedImpl::load( const OTodo& todo) {
    m_edit->setText( todo.description() );

    /* OPimState */
    int state = todo.state().state();

    /* defualt to not started */
    if ( state == OPimState::Undefined )
        state = OPimState::NotStarted;

    cmbState->setCurrentItem( state );

    /* Maintainer Mode */
    state = todo.maintainer().mode();
    if ( state == OPimMaintainer::Undefined )
        state = OPimMaintainer::Nothing;

    cmbMode->setCurrentItem( state );

}
void TaskEditorAdvancedImpl::save( OTodo& todo) {
    todo.setDescription( m_edit->text() );
    todo.setState( OPimState( cmbState->currentItem() ) );

    /* Fix me resolve name to uid.....*/
    todo.setMaintainer( OPimMaintainer( cmbMode->currentItem(), -10 ) );
    qWarning("save");
}
/*
 * int the damn UI
 */
void TaskEditorAdvancedImpl::initUI() {
    /* a MultiLineEdit */
    m_edit = new QMultiLineEdit( this );
    m_edit->setWordWrap( QMultiLineEdit::WidgetWidth );

    /* a Label */
    QLabel* lbl = new QLabel(this );
    lbl->setText( tr("Description") );

    /* add it to the QGridLayout of our base class */
    static_cast<QGridLayout*>(layout() )->addWidget( lbl   , 3, 0 );
    static_cast<QGridLayout*>(layout() )->addWidget( m_edit, 4, 0 );
}
