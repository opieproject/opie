#include <qaction.h>
#include <qlineedit.h>

#include <qpe/resource.h>

#include <opie/oclickablelabel.h>

#include "mainwindow.h"
#include "quickeditimpl.h"


QuickEditImpl::QuickEditImpl( QWidget* parent, bool visible )
    : QPEToolBar( (QMainWindow *)parent ), Todo::QuickEdit( (Todo::MainWindow *)parent ) {
    setHorizontalStretchable( TRUE );

	// TODO - come up with icons and replace text priority values
    m_lbl = new OClickableLabel( this );
    m_lbl->setMinimumWidth(15);
    m_lbl->setText("3");
    connect(m_lbl, SIGNAL(clicked() ), this, SLOT(slotPrio()) );

    m_edit = new QLineEdit( this );
    setStretchableWidget( m_edit );

    QAction *a = new QAction( tr( "More" ), Resource::loadPixmap( "todo/more" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotMore() ) );
    a->addTo( this );

    a = new QAction( tr( "Enter" ), Resource::loadPixmap( "enter" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotEnter() ) );
    a->addTo( this );

    a = new QAction( tr( "Cancel" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotCancel() ) );
    a->addTo( this );

    m_visible = visible;
    if ( !m_visible ) {
        hide();
    }
    
    m_menu = 0l;
    reinit();
}
QuickEditImpl::~QuickEditImpl() {

}
OTodo QuickEditImpl::todo()const {
    return m_todo;
}
QWidget* QuickEditImpl::widget() {
    return this;
}
void QuickEditImpl::slotEnter() {
    OTodo todo;


    if (!m_edit->text().isEmpty() ) {
        todo.setUid(1 ); // new uid
        todo.setPriority( m_lbl->text().toInt() );
        todo.setSummary( m_edit->text() );
        if ( ((Todo::MainWindow *)parent())->currentCatId() != 0 )
            todo.setCategories( ((Todo::MainWindow *)parent())->currentCatId() );

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
    // TODO - implement
}
void QuickEditImpl::slotCancel() {
    reinit();
}
void QuickEditImpl::reinit() {
    m_state = 1;
    m_lbl->setText("3");
    m_edit->clear();
}
