/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
   Copyright (C) 2002 zecke
   Copyright (C) 2002 Stefan Eilers
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#define QTOPIA_INTERNAL_FD

#include "mainwindow.h"
#include "todoentryimpl.h"
#include "todotable.h"

#include <opie/tododb.h>
#include <opie/todovcalresource.h>
#include <opie/ofontmenu.h>

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/finddialog.h>
#include <qpe/global.h>
#include <qpe/ir.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
//#include <qpe/task.h>
#include <qpe/qpetoolbar.h>

#include <qaction.h>
#include <qarray.h>
#include <qdatastream.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>

static QString todolistXMLFilename()
{
    return Global::applicationFileName("todolist","todolist.xml");
}

static QString categoriesXMLFilename()
{
    return Global::applicationFileName("todolist","categories.xml");
}

TodoWindow::TodoWindow( QWidget *parent, const char *name, WFlags f = 0 ) :
    QMainWindow( parent, name, f ), syncing(FALSE)
{
//     QTime t;
//     t.start();
    
    setCaption( tr("Todo") );
    QString str;
    table = new TodoTable( this );
    table->setColumnWidth( 2, 10 );
    table->setPaintingEnabled( FALSE );
    table->setUpdatesEnabled( FALSE );
    table->viewport()->setUpdatesEnabled( FALSE );

    {
	str = todolistXMLFilename();
	if ( str.isNull() )
	    QMessageBox::critical( this,
				   tr("Out of Space"),
				   tr("Unable to create startup files\n"
				      "Free up some space\n"
				      "before you enter any data") );
	else
	    table->load( str );
    }

    // repeat for categories...
    str = categoriesXMLFilename();
    if ( str.isNull() )
	QMessageBox::critical( this,
			       tr( "Out of Space" ),
			       tr( "Unable to create startup files\n"
				   "Free up some space\n"
				   "before you enter any data") );

    setCentralWidget( table );
    setToolBarsMovable( FALSE );

//     qDebug("after load: t=%d", t.elapsed() );
    
    Config config( "todo" );
    config.setGroup( "View" );
    bool complete = config.readBoolEntry( "ShowComplete", true );
    table->setShowCompleted( complete );

    bool showdeadline = config.readBoolEntry("ShowDeadLine", true);
    table->setShowDeadline (showdeadline);

    QString category = config.readEntry( "Category", QString::null );
    table->setShowCategory( category );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setHorizontalStretchable( TRUE );

    QPEMenuBar *mb = new QPEMenuBar( bar );

    catMenu = new QPopupMenu( this );
    QPopupMenu *edit = new QPopupMenu( this );
    QPopupMenu *options = new QPopupMenu(this );

    contextMenu = new QPopupMenu( this );

    bar = new QPEToolBar( this );

    QAction *a = new QAction( tr( "New Task" ), Resource::loadPixmap( "new" ),
			      QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ),
             this, SLOT( slotNew() ) );
    a->addTo( bar );
    a->addTo( edit );

    a = new QAction( tr( "Edit Task" ), Resource::loadIconSet( "edit" ),
		     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ),
             this, SLOT( slotEdit() ) );
    a->addTo( bar );
    a->addTo( edit );
    a->addTo( contextMenu );
    a->setEnabled( FALSE );
    editAction = a;
    edit->insertSeparator();

    a = new QAction( tr( "Delete..." ), Resource::loadIconSet( "trash" ),
		     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ),
             this, SLOT( slotDelete() ) );
    a->addTo( bar );
    a->addTo( edit );
    a->addTo( contextMenu );
    a->setEnabled( FALSE );
    deleteAction = a;

    // delete All in category is missing....
    // set All Done
    // set All Done in category

    a = new QAction( QString::null, tr( "Delete all..."), 0, this, 0 );
    connect(a, SIGNAL( activated() ),
	    this, SLOT( slotDeleteAll() ) );
    a->addTo(edit );
    a->setEnabled( FALSE );
    deleteAllAction = a;  

    edit->insertSeparator();
    a = new QAction( QString::null, tr("Duplicate" ), 0, this, 0 );
    connect(a, SIGNAL( activated() ),
	    this, SLOT( slotDuplicate() ) );
    a->addTo(edit );
    a->setEnabled( FALSE );
    duplicateAction = a;

    edit->insertSeparator();
    if ( Ir::supported() ) {
	a = new QAction( tr( "Beam" ), Resource::loadPixmap( "beam" ),
			 QString::null, 0, this, 0 );
	connect( a, SIGNAL( activated() ),
		 this, SLOT( slotBeam() ) );
	a->addTo( edit );
	a->addTo( bar );
    }

    a = new QAction( tr( "Find" ), Resource::loadIconSet( "mag" ),
		     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ),
	     this, SLOT( slotFind() ) );
    a->addTo( bar );
    a->addTo( options );
    options->insertSeparator();

    if ( table->numRows() )
	a->setEnabled( TRUE );
    else
	a->setEnabled( FALSE );

    //a->setEnabled( FALSE );
    findAction = a;
//     qDebug("mainwindow #2: t=%d", t.elapsed() );

    completedAction = new QAction( QString::null, tr("Completed tasks"), 0, this, 0, TRUE );

    showdeadlineAction = new QAction( QString::null, tr( "Show Deadline" ), 0, this, 0, TRUE );

    catMenu->setCheckable( true );
    populateCategories();

    
    completedAction->addTo( options );
    completedAction->setOn( table->showCompleted() );
    showdeadlineAction->addTo( options );
    showdeadlineAction->setOn( table->showDeadline() );
    options->insertSeparator( );
    QList<QWidget> list;
    list.append(table );
    OFontMenu *menu = new OFontMenu(this, "menu",list );
    menu->forceSize( table->horizontalHeader(), 10 );
    //catMenu->insertItem(tr("Fonts"), menu );
    list.clear();
    options->insertItem( tr("Fonts"), menu );


    mb->insertItem( tr( "Data" ), edit );
    mb->insertItem( tr( "Category" ), catMenu );
    mb->insertItem( tr( "Options"), options );
    resize( 200, 300 );
    if ( table->numRows() > 0 )
        currentEntryChanged( 0, 0 );
    connect( table, SIGNAL( signalEdit() ),
             this, SLOT( slotEdit() ) );
    connect( table, SIGNAL(signalShowMenu(const QPoint &)),
	     this, SLOT( slotShowPopup(const QPoint &)) );

//     qDebug("mainwindow #3: t=%d", t.elapsed() );
    table->updateVisible();
    table->setUpdatesEnabled( TRUE );
    table->setPaintingEnabled( TRUE );
    table->viewport()->setUpdatesEnabled( TRUE );

    connect( completedAction, SIGNAL( toggled(bool) ), this, SLOT( showCompleted(bool) ) );
    connect( showdeadlineAction, SIGNAL( toggled(bool) ), this, SLOT( showDeadline(bool) ) );
    connect( catMenu, SIGNAL(activated(int)), this, SLOT(setCategory(int)) );
    connect( table, SIGNAL( currentChanged( int, int ) ),
             this, SLOT( currentEntryChanged( int, int ) ) );

//     qDebug("done: t=%d", t.elapsed() );
}

void TodoWindow::slotNew()
{
    if(syncing) {
	QMessageBox::warning(this, tr("Todo"),
			     tr("Can not edit data, currently syncing"));
	return;
    }

    int id;
    id = -1;
    QArray<int> ids;
    ids = table->currentEntry().categories();
    if ( ids.count() )
	id = ids[0];
    NewTaskDialog e( id, this, 0, TRUE );

    ToDoEvent todo;

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif
    int ret = e.exec();
//    qWarning("finished" );
    if ( ret == QDialog::Accepted ) {
	table->setPaintingEnabled( false );
        todo = e.todoEntry();
	//todo.assignUid();
        table->addEntry( todo );
	table->setPaintingEnabled( true );
	findAction->setEnabled( TRUE );
    }
    // I'm afraid we must call this every time now, otherwise
    // spend expensive time comparing all these strings...
    populateCategories();
}

TodoWindow::~TodoWindow()
{
}

void TodoWindow::slotDelete()
{
    if(syncing) {
	QMessageBox::warning(this, tr("Todo"),
			     tr("Can not edit data, currently syncing"));
	return;
    }

    if ( table->currentRow() == -1 )
        return;

    QString strName = table->text( table->currentRow(), 2 ).left( 30 );

    if ( !QPEMessageBox::confirmDelete( this, tr( "Todo" ), strName ) )
	return;



    table->setPaintingEnabled( false );
    table->removeCurrentEntry();
    table->setPaintingEnabled( true );

    if ( table->numRows() == 0 ) {
        currentEntryChanged( -1, 0 );
	findAction->setEnabled( FALSE );
    }
}
void TodoWindow::slotDeleteAll()
{
  if(syncing) {
    QMessageBox::warning(this, tr("Todo"),
			 tr("Can not edit data, currently syncing"));
    return;
  }
  
  //QString strName = table->text( table->currentRow(), 2 ).left( 30 );
  
  if ( !QPEMessageBox::confirmDelete( this, tr( "Todo" ), tr("Should I delete all tasks?") ) )
    return;
  
  
  
  table->setPaintingEnabled( false );
  table->removeAllEntries();
  table->setPaintingEnabled( true );
  
  if ( table->numRows() == 0 ) {
    currentEntryChanged( -1, 0 );
    findAction->setEnabled( FALSE );
  }
}

void TodoWindow::slotEdit()
{
    if(syncing) {
	QMessageBox::warning(this, tr("Todo"),
			     tr("Can not edit data, currently syncing"));
	return;
    }

    ToDoEvent todo = table->currentEntry();

    NewTaskDialog e( todo, this, 0, TRUE );
    e.setCaption( tr( "Edit Task" ) );

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif
    int ret = e.exec();

    if ( ret == QDialog::Accepted ) {
	table->setPaintingEnabled( false );
        todo = e.todoEntry();
	table->replaceCurrentEntry( todo );
	table->setPaintingEnabled( true );
    }
    populateCategories();

}
void TodoWindow::slotDuplicate()
{
  if(syncing) {
    QMessageBox::warning(this, tr("Todo"),
			 tr("Can not edit data, currently syncing"));
    return;
  }
  ToDoEvent ev = table->currentEntry();
  ToDoEvent ev2 = ToDoEvent( ev );
  table->setPaintingEnabled( false );
  table->addEntry( ev2 );
  table->setPaintingEnabled( true );
}
void TodoWindow::slotShowPopup( const QPoint &p )
{
    contextMenu->popup( p );
}

void TodoWindow::showCompleted( bool s )
{
    if ( !table->isUpdatesEnabled() )
	return;
    table->setPaintingEnabled( false );
    table->setShowCompleted( s );
    table->setPaintingEnabled( true );
}

void TodoWindow::currentEntryChanged( int r, int )
{
    if ( r != -1 && table->rowHeight( r ) > 0 ) {
        editAction->setEnabled( TRUE );
        deleteAction->setEnabled( TRUE );
	duplicateAction->setEnabled( TRUE );
	deleteAllAction->setEnabled( TRUE );
    } else {
        editAction->setEnabled( FALSE );
        deleteAction->setEnabled( FALSE );
	duplicateAction->setEnabled( FALSE );
	deleteAllAction->setEnabled( FALSE );
    }
}

void TodoWindow::setCategory( int c )
{
    if ( c <= 0 ) return;
    if ( !table->isUpdatesEnabled() )
	return; 
   table->setPaintingEnabled( false );
    for ( unsigned int i = 1; i < catMenu->count(); i++ )
	catMenu->setItemChecked( i, c == (int)i );
    if ( c == 1 ) {
	table->setShowCategory( QString::null );
	setCaption( tr("Todo") + " - " + tr( "All Categories" ) );
    } else if ( c == (int)catMenu->count() - 1 ) {
	table->setShowCategory( tr( "Unfiled" ) );
	setCaption( tr("Todo") + " - " + tr( "Unfiled" ) );
    } else {
	QString cat = table->categories()[c - 2];
	table->setShowCategory( cat );
	setCaption( tr("Todo") + " - " + cat );
    }
    table->setPaintingEnabled( true );
}

void TodoWindow::populateCategories()
{
    catMenu->clear();
    int id, rememberId;
    id = 1;
    catMenu->insertItem( tr( "All Categories" ), id++ );
    catMenu->insertSeparator();
    QStringList categories = table->categories();
    categories.append( tr( "Unfiled" ) );
    for ( QStringList::Iterator it = categories.begin();
	  it != categories.end(); ++it ) {
	catMenu->insertItem( *it, id );
	if ( *it == table->showCategory() )
	    rememberId = id;
	++id;
    }
    if ( table->showCategory().isEmpty() )
	setCategory( 1 );
    else
	setCategory( rememberId );
}

void TodoWindow::reload()
{
    table->clear();
    table->load( todolistXMLFilename() );
    syncing = FALSE;
}

void TodoWindow::flush()
{
    syncing = TRUE;
    table->save( todolistXMLFilename() );
}

void TodoWindow::closeEvent( QCloseEvent *e )
{
    if(syncing) {
	/* no need to save if in the middle of syncing */
	e->accept();
	return;
    }

    if ( table->save( todolistXMLFilename() ) ) {
	e->accept();
    	// repeat for categories...
	// if writing configs fail, it will emit an
	// error, but I feel that it is "ok" for us to exit
	// espically since we aren't told if the write succeeded...
	Config config( "todo" );
	config.setGroup( "View" );
	config.writeEntry( "ShowComplete", table->showCompleted() );
	config.writeEntry( "Category", table->showCategory() );
	/* added 20.01.2k2 by se */
	config.writeEntry( "ShowDeadLine", table->showDeadline());
    } else {
	if ( QMessageBox::critical( this, tr("Out of space"),
				    tr("Todo was unable\n"
				       "to save your changes.\n"
				       "Free up some space\n"
				       "and try again.\n"
				       "\nQuit Anyway?"),
				    QMessageBox::Yes|QMessageBox::Escape,
				    QMessageBox::No|QMessageBox::Default)
	     != QMessageBox::No )
	    e->accept();
	else
	    e->ignore();
    }
}

void TodoWindow::slotFind()
{
    // put everything back to view all for searching...
    if ( !catMenu->isItemChecked( 0 ) )
	setCategory( 0 );

    FindDialog dlg( "Todo List",  this );
    QObject::connect( &dlg,
		      SIGNAL(signalFindClicked(const QString &,
					       bool, bool, int)),
		      table,
		      SLOT(slotDoFind(const QString&, bool, bool, int)) );
    QObject::connect( table, SIGNAL(signalNotFound()), &dlg,
		      SLOT(slotNotFound()) );
    QObject::connect( table, SIGNAL(signalWrapAround()), &dlg,
		      SLOT(slotWrapAround()) );
    dlg.exec();
    if ( table->numSelections() )
	table->clearSelection();
    table->clearFindRow();
}


void TodoWindow::setDocument( const QString &filename )
{
    if ( filename.find(".vcs") != int(filename.length()) - 4 ) return;

    ToDoDB todoDB(filename, new ToDoVCalResource() );
    QValueList<ToDoEvent> tl = todoDB.rawToDos();
    for( QValueList<ToDoEvent>::Iterator it = tl.begin(); it != tl.end(); ++it ) {
	table->addEntry( *it );
    }
}

static const char * beamfile = "/tmp/obex/todo.vcs";

void TodoWindow::slotBeam()
{
    unlink( beamfile ); // delete if exists
    ToDoEvent c = table->currentEntry();
    mkdir("/tmp/obex/", 0755);
    ToDoDB todoDB( beamfile, new ToDoVCalResource() );
    todoDB.addEvent( c );
    todoDB.save();
    Ir *ir = new Ir( this );
    connect( ir, SIGNAL( done( Ir * ) ), this, SLOT( beamDone( Ir * ) ) );
    QString description = c.description();
    ir->send( beamfile, description, "text/x-vCalendar" );
}

void TodoWindow::beamDone( Ir *ir )
{
    delete ir;
    unlink( beamfile );
}

/* added 20.01.2k2 by se */
void TodoWindow::showDeadline( bool s )
{
    table->setPaintingEnabled( false );
    table->setShowDeadline( s );
    table->setPaintingEnabled( true );
}
