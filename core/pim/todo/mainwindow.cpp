/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qmenubar.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qwidgetstack.h>
#include <qaction.h>

#include <qpe/config.h>
#include <qpe/ir.h>
#include <qpe/resource.h>
#include <qpe/qpemessagebox.h>


#include "todoentryimpl.h"
#include "mainwindow.h"


using namespace Todo;

MainWindow::MainWindow( QWidget* parent,
                        const char* name ) {

    m_syncing = false;
    initUI();
    initActions();
    initConfig();
}
void MainWindow::initActions() {
    QAction* a = new QAction( tr("New Task" ), Resource::loadPixmap( "new" ),
                              QString::null, 0, this, 0 );
    connect(a, SIGNAL( activated() ),
            this, SLOT( slotNew() ) );
    a->addTo(m_tool );
    a->addTo(m_edit );

    a = new QAction( tr("Edit Task"), Resource::loadIconSet( "edit" ),
                     QString::null, 0, this, 0 );
    connect(a, SIGNAL(activated() ),
            this, SLOT( slotEdit() ) );
    a->addTo( m_tool );
    a->addTo( m_edit );
    m_editAction = a;

    a = new QAction( QString::null, tr("View Task"), 0, this, 0 );
    connect(a, SIGNAL( activated() ),
            this, SLOT( slotShowDetails() ) );
    a->addTo( m_edit );

    m_edit->insertSeparator();

    a = new QAction( tr("Delete..."),  Resource::loadIconSet( "trash" ),
                     QString::null, 0, this, 0 );
    a->addTo( m_tool );
    a->addTo( m_edit );
    a->setEnabled( FALSE );
    m_deleteAction = a;

    a = new QAction( QString::null, tr("Delete all..."), 0, this, 0 );
    connect(a, SIGNAL( activated() ),
            this, SLOT( slotDeleteAll() ) );
    a->addTo(m_edit );
    m_deleteAllAction = a;

    a = new QAction( QString::null, tr("Delete completed"),
                     0, this, 0 );
    connect(a, SIGNAL( activated() ),
            this, SLOT( slotDeleteCompleted() ) );
    a->addTo(m_edit );
    a->setEnabled( TRUE );
    m_deleteCompleteAction = a;

    m_edit->insertSeparator();

    a = new QAction( QString::null, tr("Duplicate"), 0, this, 0 );
    connect(a, SIGNAL( activated() ),
            this, SLOT( slotDuplicate() ) );
    a->addTo(m_edit );
    m_duplicateAction = a;

    m_edit->insertSeparator();

    if ( Ir::supported() ) {
	a = new QAction( tr( "Beam" ), Resource::loadPixmap( "beam" ),
			 QString::null, 0, this, 0 );
	connect( a, SIGNAL( activated() ),
		 this, SLOT( slotBeam() ) );
	a->addTo( m_edit );
	a->addTo( m_tool );
    }

    a = new QAction( tr("Find"), Resource::loadIconSet( "mag" ),
                     QString::null, 0, this, 0 );
    connect(a, SIGNAL( activated() ),
            this, SLOT( slotFind() ) );
    a->addTo( m_options );
    m_findAction = a;

    m_options->insertSeparator();

    m_completedAction = new QAction( QString::null, tr("Completed tasks"),
                                     0, this, 0, TRUE );
    m_completedAction->addTo( m_options );
    m_completedAction->setOn( showCompleted() );
    connect(m_completedAction, SIGNAL( toggled(bool) ),
            this, SLOT(slotShowCompleted(bool) ) );

    m_showDeadLineAction = new QAction( QString::null, tr("Show Deadline"),
                                        0, this, 0, TRUE );
    m_showDeadLineAction->addTo( m_options );
    m_showDeadLineAction->setOn( showDeadline() );
    connect(m_showDeadLineAction, SIGNAL(toggled(bool) ),
            this, SLOT( slotShowDeadLine( bool ) ) );

    m_options->insertSeparator();
    populateCategories();

    m_bar->insertItem( tr("Data") ,m_edit );
    m_bar->insertItem( tr("Category"),  m_catMenu );
    m_bar->insertItem( tr("Options"), m_options );


}
/* m_curCat from Config */
void MainWindow::initConfig() {
    Config config( "todo" );
    config.setGroup( "View" );
    m_completed =  config.readBoolEntry( "ShowComplete", TRUE );
    m_curCat = config.readEntry( "Category",    QString::null );
    m_deadline =  config.readBoolEntry( "ShowDeadLine", TRUE);
}
void MainWindow::initUI() {
    m_stack = new QWidgetStack(this,  "main stack");
    setCentralWidget( m_stack );

    setToolBarsMovable( FALSE );

    m_tool = new QToolBar( this );
    m_tool->setHorizontalStretchable( TRUE );

    m_bar = new QMenuBar( m_tool );

    /** QPopupMenu */
    m_edit = new QPopupMenu( this );
    m_options = new QPopupMenu( this );
    m_catMenu = new QPopupMenu( this );
    m_catMenu->setCheckable( TRUE );
}
MainWindow::~MainWindow() {

}
QPopupMenu* MainWindow::contextMenu( int uid ) {
    return 0l;
}
ToDoDB::Iterator MainWindow::begin() {
    return m_todoMgr.begin();
}
ToDoDB::Iterator MainWindow::end() {
    return m_todoMgr.end();
}
ToDoEvent MainWindow::event( int uid ) {
    return m_todoMgr.event( uid );
}
bool MainWindow::isSyncing()const {
    return m_syncing;
}
void MainWindow::slotReload() {
    m_todoMgr.reload();
    currentView()->setTodos( begin(), end() );
    raiseCurrentView();
}
void MainWindow::closeEvent( QCloseEvent* e ) {
    e->accept();
    /*if (!m_viewVisible ) {
        mStack->raiseWidget( currentView() );
        e->ignore();
        return;
        }*/
    /*
     * we should have flushed and now we're still saving
     * so there is no need to flush
     */
    if (m_syncing ) {
        e->accept();
        return;
    }
    bool quit = false;
    if ( m_todoMgr.saveAll() ){
        quit = true;
    }else {
	if ( QMessageBox::critical( this, tr("Out of space"),
				    tr("Todo was unable\n"
				       "to save your changes.\n"
				       "Free up some space\n"
				       "and try again.\n"
				       "\nQuit Anyway?"),
				    QMessageBox::Yes|QMessageBox::Escape,
				    QMessageBox::No|QMessageBox::Default)
	     != QMessageBox::No ) {
	    e->accept();
            quit = true;
	}else
	    e->ignore();

    }

    if (quit ) {
        Config config( "todo" );
        config.setGroup( "View" );
        config.writeEntry( "ShowComplete", showCompleted() );
        config.writeEntry( "Category", currentCategory() );
        config.writeEntry( "ShowDeadLine", showDeadline());
    }
}
void MainWindow::slotNew() {
    if(m_syncing) {
	QMessageBox::warning(this, tr("Todo"),
			     tr("Can not edit data, currently syncing"));
	return;
    }


    NewTaskDialog e( currentCatId(), this, 0, TRUE );


#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif
    int ret = e.exec();

    if ( ret == QDialog::Accepted ) {
        ToDoEvent todo = e.todoEntry();
	//todo.assignUid();
        currentView()->addEvent( todo );
        m_todoMgr.add( todo );
    }
    // I'm afraid we must call this every time now, otherwise
    // spend expensive time comparing all these strings...
    populateCategories();
    raiseCurrentView( );
}
void MainWindow::slotDuplicate() {
    if(m_syncing) {
        QMessageBox::warning(this, tr("Todo"),
                             tr("Can not edit data, currently syncing"));
        return;
    }
    ToDoEvent ev = m_todoMgr.event( currentView()->current() );
    /* let's generate a new uid */
    ev.setUid(-1);
    m_todoMgr.add( ev );
    currentView()->addEvent( ev );
    raiseCurrentView();
}
void MainWindow::slotDelete() {
    if(m_syncing) {
	QMessageBox::warning(this, tr("Todo"),
			     tr("Can not edit data, currently syncing"));
	return;
    }
    QString strName = currentView()->currentRepresentation();
    if (!QPEMessageBox::confirmDelete(this, tr("Todo"), strName ) )
        return;

    m_todoMgr.remove( currentView()->current() );
    currentView()->removeEvent( currentView()->current() );
    raiseCurrentView();
}
void MainWindow::slotDeleteAll() {
    if(m_syncing) {
        QMessageBox::warning(this, tr("Todo"),
                             tr("Can not edit data, currently syncing"));
        return;
    }

  //QString strName = table->text( table->currentRow(), 2 ).left( 30 );

    if ( !QPEMessageBox::confirmDelete( this, tr( "Todo" ), tr("all tasks?") ) )
        return;

    m_todoMgr.removeAll();
    currentView()->clear();

    raiseCurrentView();
}
void MainWindow::slotDeleteCompleted() {
  if(m_syncing) {
    QMessageBox::warning(this, tr("Todo"),
			 tr("Can not edit data, currently syncing"));
    return;
  }

  if ( !QPEMessageBox::confirmDelete( this, tr( "Todo" ), tr("all completed tasks?") ) )
    return;

  m_todoMgr.remove( currentView()->completed() );
}
void MainWindow::slotFind() {

}
void MainWindow::slotEdit() {
    if(m_syncing) {
	QMessageBox::warning(this, tr("Todo"),
			     tr("Can not edit data, currently syncing"));
	return;
    }

    ToDoEvent todo = m_todoMgr.event( currentView()->current() );
    qWarning("slotEdit" );
    NewTaskDialog e( todo, this, 0, TRUE );
    e.setCaption( tr( "Edit Task" ) );

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif
    int ret = e.exec();

    if ( ret == QDialog::Accepted ) {
        qWarning("Replacing now" );
        todo = e.todoEntry();
        m_todoMgr.update( todo.uid(), todo );
	currentView()->replaceEvent( todo );
    }
    populateCategories();
    raiseCurrentView();
}
/*
 * set the category
 */
void MainWindow::setCategory( int c) {
    if ( c <= 0 ) return;
    for ( unsigned int i = 1; i < m_catMenu->count(); i++ )
        m_catMenu->setItemChecked(i, c == (int)i );

    if (c == 1 ) {
        m_curCat = QString::null;
        currentView()->setShowCategory( QString::null );
        setCaption( tr("Todo") + " - " + tr("All Categories" ) );

    }else if ( c == (int)m_catMenu->count() - 1 ) {
        m_curCat = tr("Unfiled");
        setCaption( tr("Todo") + " - " + tr("Unfiled") );
    }else {
        m_curCat = m_todoMgr.categories()[c-2];
        setCaption( tr("Todo") + " - " + m_curCat );
    }
    currentView()->setShowCategory( m_curCat );
    raiseCurrentView();
}
void MainWindow::slotShowDeadLine( bool dead) {
    m_deadline = dead;
    currentView()->setShowDeadline( dead );
}
void MainWindow::slotShowCompleted( bool show) {
    m_completed = show;
    currentView()->setShowCompleted( m_completed );
}
void MainWindow::setDocument( const QString& ) {

}
void MainWindow::slotBeam() {

}
void MainWindow::beamDone( Ir* ) {

}

void MainWindow::slotFlush() {
    m_syncing = FALSE;
    m_todoMgr.save();
}
void MainWindow::slotShowDetails() {

}
/*
 * populate the Categories
 * Menu
 */
void MainWindow::populateCategories() {

    m_catMenu->clear();
    int id, rememberId;
    id = 1;
    rememberId = 1;

    m_catMenu->insertItem( tr( "All Categories" ), id++ );
    m_catMenu->insertSeparator();
    QStringList categories = m_todoMgr.categories();
    categories.append( tr( "Unfiled" ) );
    for ( QStringList::Iterator it = categories.begin();
	  it != categories.end(); ++it ) {
	m_catMenu->insertItem( *it, id );
	if ( *it == currentCategory() )
	    rememberId = id;
	++id;
    }
    setCategory( rememberId );
}
bool MainWindow::showCompleted()const {
    return m_completed;
}
bool MainWindow::showDeadline()const {
    return m_deadline;
}
QString MainWindow::currentCategory()const {
    return m_curCat;
}
int MainWindow::currentCatId() {
    return m_todoMgr.catId( m_curCat );
}
View* MainWindow::currentView() {
    return 0l;
}
void MainWindow::raiseCurrentView() {

}
