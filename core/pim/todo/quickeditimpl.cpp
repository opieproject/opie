#include <qlineedit.h>

#include <opie/oclickablelabel.h>

#include "mainwindow.h"
#include "quickeditimpl.h"


QuickEditImpl::QuickEditImpl( Todo::MainWindow* win, QWidget* arent )
    : QHBox(arent), Todo::QuickEdit(win) {
    m_lbl = new OClickableLabel(this );
    m_lbl->setMinimumWidth(12);
    m_lbl->setText("3");

    m_edit = new QLineEdit(this );

    m_enter = new OClickableLabel(this);
    m_enter->setText("Enter");

    m_more  = new OClickableLabel(this);
    m_more->setText("More");


    // connect
    connect(m_lbl, SIGNAL(clicked() ),
            this, SLOT(slotPrio()) );
    connect(m_enter, SIGNAL(clicked() ),
            this, SLOT(slotEnter() ) );
    connect(m_more, SIGNAL(clicked() ),
            this, SLOT(slotMore() ) );

    m_menu = 0l;
    reinit();
    setMaximumHeight( m_edit->sizeHint().height() );
}
QuickEditImpl::~QuickEditImpl() {

}
OTodo QuickEditImpl::todo()const {
    return m_todo;
}
QWidget* QuickEditImpl::widget() {
    return this;
}
QSize QuickEditImpl::sizeHint()const{
    return m_edit->sizeHint();
}
void QuickEditImpl::slotEnter() {
    OTodo todo;


    if (!m_edit->text().isEmpty() ) {
        todo.setUid(1 ); // new uid
        todo.setPriority( m_lbl->text().toInt() );
        todo.setSummary( m_edit->text() );
        if ( mainWindow()->currentCatId() != 0 )
            todo.setCategories( mainWindow()->currentCatId() );

        m_todo = todo;
       commit();
    }
    m_todo = todo;
    reinit();
}
void QuickEditImpl::slotPrio() {
    m_state++;
    if (m_state > 2 )
        m_state = 0;

    switch(m_state ) {
    case 0:
        m_lbl->setText( "1" );
        break;
    case 2:
        m_lbl->setText( "5" );
        break;
    case 1:
    default:
        m_lbl->setText( "3");
        break;
    }
}
void QuickEditImpl::slotMore() {
}
void QuickEditImpl::reinit() {
    m_state = 1;
    m_lbl->setText("3");
    m_edit->clear();
}
