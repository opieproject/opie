#include <qaction.h>
#include <qlineedit.h>
#include <qwhatsthis.h>

#include <qpe/resource.h>

#include <opie/oclickablelabel.h>

#include "mainwindow.h"
#include "quickeditimpl.h"


QuickEditImpl::QuickEditImpl( QWidget* parent, bool visible )
    : QPEToolBar( (QMainWindow *)parent ), Todo::QuickEdit( (Todo::MainWindow *)parent ) {
    setHorizontalStretchable( TRUE );

    // Load priority icons
    // TODO - probably should be done globally somewhere else,
    //        see also tableview.cpp/h, taskeditoroverview.cpp/h
    priority1 = Resource::loadPixmap( "todo/priority1" );
    priority3 = Resource::loadPixmap( "todo/priority3" );
    priority5 = Resource::loadPixmap( "todo/priority5" );

    m_lbl = new OClickableLabel( this );
    m_lbl->setMinimumWidth( 15 );
    m_lbl->setPixmap( priority3 );
    connect(m_lbl, SIGNAL(clicked() ), this, SLOT(slotPrio()) );
    QWhatsThis::add( m_lbl, tr( "Click here to set the priority of new task.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

    m_edit = new QLineEdit( this );
    setStretchableWidget( m_edit );
    QWhatsThis::add( m_edit, tr( "Enter description of new task here.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );
    
    QAction *a = new QAction( tr( "More" ), Resource::loadPixmap( "todo/more" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotMore() ) );
    a->addTo( this );
    a->setWhatsThis( tr( "Click here to enter additional information for new task.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

    a = new QAction( tr( "Enter" ), Resource::loadPixmap( "enter" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotEnter() ) );
    a->addTo( this );
    a->setWhatsThis( tr( "Click here to add new task.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

    a = new QAction( tr( "Cancel" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotCancel() ) );
    a->addTo( this );
    a->setWhatsThis( tr( "Click here to reset new task information.\n\nThis area is called the quick task bar.\n\nIt allows you to quickly add a new task to your list.  This area can be shown or hidden by selecting Options->'Show quick task bar' from the menu above." ) );

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
        todo.setPriority( m_state );
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
    m_state -= 2;
    if ( m_state < 1 )
        m_state = 5;

    switch( m_state ) {
    case 1:
        m_lbl->setPixmap( priority1 );
        break;
    case 5:
        m_lbl->setPixmap( priority5 );
        break;
    case 3:
    default:
        m_lbl->setPixmap( priority3 );
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
    m_state = 3;
    m_lbl->setPixmap( priority3 );
    m_edit->clear();
}
