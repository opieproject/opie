/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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
/* Show Deadline was added by Stefan Eilers (se, eilers.stefan@epost.de)
 */
#include "todotable.h"

#include <opie/tododb.h>
#include <opie/xmltree.h>

#include <qpe/categoryselect.h>
#include <qpe/xmlreader.h>

#include <qasciidict.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qpainter.h>
#include <qtextcodec.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qtextstream.h>

#include <qcursor.h>
#include <qregexp.h>

#include <errno.h>
#include <stdlib.h>

#include <stdio.h>
#include <iostream>

namespace {

  static bool taskCompare( const ToDoEvent &task, const QRegExp &r, int category );
  static QString journalFileName();
  static ToDoEvent xmlToEvent( XMLElement *ev );
}
CheckItem::CheckItem( QTable *t, const QString &key )
    : QTableItem( t, Never, "" ), checked( FALSE ), sortKey( key )
{
}

QString CheckItem::key() const
{
    return sortKey;
}

void CheckItem::setChecked( bool b )
{
    checked = b;
    table()->updateCell( row(), col() );
}

void CheckItem::toggle()
{
    TodoTable *parent = static_cast<TodoTable*>(table());
    ToDoEvent newTodo = parent->currentEntry();
    checked = !checked;
    newTodo.setCompleted( checked );
    table()->updateCell( row(), col() );
    parent->replaceCurrentEntry( newTodo, true );
}

bool CheckItem::isChecked() const
{
    return checked;
}

static const int BoxSize = 10;

void CheckItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr,
		       bool )
{
    p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Base ) );

    int marg = ( cr.width() - BoxSize ) / 2;
    int x = 0;
    int y = ( cr.height() - BoxSize ) / 2;
    p->setPen( QPen( cg.text() ) );
    p->drawRect( x + marg, y, BoxSize, BoxSize );
    p->drawRect( x + marg+1, y+1, BoxSize-2, BoxSize-2 );
    p->setPen( darkGreen );
    x += 1;
    y += 1;
    if ( checked ) {
	QPointArray a( 7*2 );
	int i, xx, yy;
	xx = x+1+marg;
	yy = y+2;
	for ( i=0; i<3; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy++;
	}
	yy -= 2;
	for ( i=3; i<7; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy--;
	}
	p->drawLineSegments( a );
    }
}


ComboItem::ComboItem( QTable *t, EditType et )
    : QTableItem( t, et, "3" ), cb( 0 )
{
    setReplaceable( FALSE );
}

QWidget *ComboItem::createEditor() const
{
    QString txt = text();
    ( (ComboItem*)this )->cb = new QComboBox( table()->viewport() );
    cb->insertItem( "1" );
    cb->insertItem( "2" );
    cb->insertItem( "3" );
    cb->insertItem( "4" );
    cb->insertItem( "5" );
    cb->setCurrentItem( txt.toInt() - 1 );
    return cb;
}

void ComboItem::setContentFromEditor( QWidget *w )
{
    TodoTable *parent = static_cast<TodoTable*>(table());
    ToDoEvent newTodo = parent->currentEntry();

    if ( w->inherits( "QComboBox" ) )
	setText( ( (QComboBox*)w )->currentText() );
    else
	QTableItem::setContentFromEditor( w );
    newTodo.setPriority( text().toInt() );
    parent->replaceCurrentEntry( newTodo, true );
}

void ComboItem::setText( const QString &s )
{
    if ( cb )
	cb->setCurrentItem( s.toInt() - 1 );
    QTableItem::setText( s );
}

QString ComboItem::text() const
{
    if ( cb )
	return cb->currentText();
    return QTableItem::text();
}
DueTextItem::DueTextItem( QTable *t, ToDoEvent *ev )
  : QTableItem(t, Never, QString::null )
{
  setToDoEvent( ev );
}
QString DueTextItem::key() const
{
  QString key;
  if( m_hasDate ){
    if(m_off == 0 ){
      key.append("b");
    }else if( m_off > 0 ){
      key.append("c");
    }else if( m_off < 0 ){
      key.append("a");
    }
    key.append(QString::number(m_off ) );
  }else{
    key.append("d");
  }
  return key;
}
void DueTextItem::setToDoEvent( const ToDoEvent *ev )
{
  m_hasDate = ev->hasDate();
  m_completed = ev->isCompleted();
  if( ev->hasDate() ){
    QDate today = QDate::currentDate();
    m_off = today.daysTo(ev->date() );
    //qWarning("DueText m_off=%d", m_off );
    setText( QString::number(m_off)  + " day(s) " );
  }else{
    setText("n.d." );
    m_off = 0;
  }
  //qWarning("m_off=%d", m_off );
}
void DueTextItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected )
{
  //qWarning ("paint m_off=%d", m_off );
  QColorGroup cg2(cg);
  QColor text = cg.text();
  if( m_hasDate && !m_completed ){
    if( m_off < 0 ){
      cg2.setColor(QColorGroup::Text, QColor(red ) );
    }else if( m_off == 0 ){
      cg2.setColor(QColorGroup::Text, QColor(yellow) ); // orange isn't predefined
    }else if( m_off > 0){
      cg2.setColor(QColorGroup::Text, QColor(green ) );
    }
  }
  QTableItem::paint(p, cg2, cr, selected );
  cg2.setColor(QColorGroup::Text, text );
}
TodoTable::TodoTable( QWidget *parent, const char *name )
// #ifdef QT_QTABLE_NOHEADER_CONSTRUCTOR
//     : QTable( 0, 3, parent, name, TRUE ),
// #else
    : QTable( 0, 4, parent, name ),
// #endif
      showComp( true ),
      enablePainting( true ),
      mCat( 0 ),
      currFindRow( -2 ),
      showDeadl( true)
{
    mCat.load( categoryFileName() );
    setSorting( TRUE );
    setSelectionMode( NoSelection );
    setColumnStretchable( 2, TRUE );
    setColumnWidth( 0, 20 );
    setColumnWidth( 1, 35 );

    setLeftMargin( 0 );
    verticalHeader()->hide();
    horizontalHeader()->setLabel( 0, tr( "C." ) );
    horizontalHeader()->setLabel( 1, tr( "Prior." ) );
    horizontalHeader()->setLabel( 2, tr( "Description" ) );

    setColumnStretchable( 3, FALSE );
    setColumnWidth( 3, 20 );
    horizontalHeader()->setLabel( 3, tr( "Deadline" ) );    

    if (showDeadl){
      showColumn (3);
    }else{
      hideColumn (3);
    }

    connect( this, SIGNAL( clicked( int, int, int, const QPoint & ) ),
	     this, SLOT( slotClicked( int, int, int, const QPoint & ) ) );
    connect( this, SIGNAL( pressed( int, int, int, const QPoint & ) ),
	     this, SLOT( slotPressed( int, int, int, const QPoint & ) ) );
    connect( this, SIGNAL( valueChanged( int, int ) ),
             this, SLOT( slotCheckPriority( int, int ) ) );
    connect( this, SIGNAL( currentChanged( int, int ) ),
             this, SLOT( slotCurrentChanged( int, int ) ) );

    menuTimer = new QTimer( this );
    connect( menuTimer, SIGNAL(timeout()), this, SLOT(slotShowMenu()) );

    mDayTimer = new QTimer( this );
    connect( mDayTimer, SIGNAL(timeout()), this, SLOT(slotCheckDay() ) );
    mDay = QDate::currentDate();
}

void TodoTable::addEntry( const ToDoEvent &todo )
{
    int row = numRows();
    setNumRows( row + 1 );
    updateJournal( todo, ACTION_ADD );
    insertIntoTable( new ToDoEvent(todo), row );
    setCurrentCell(row, currentColumn());
    updateVisible();
}

void TodoTable::slotClicked( int row, int col, int, const QPoint &pos )
{
    if ( !cellGeometry( row, col ).contains(pos) )
	return;
    // let's switch on the column number...
    switch ( col )
    {
        case 0: {
		CheckItem *i = static_cast<CheckItem*>(item( row, col ));
		if ( i ) {
		    int x = pos.x() - columnPos( col );
		    int y = pos.y() - rowPos( row );
		    int w = columnWidth( col );
		    int h = rowHeight( row );
		    if ( i && x >= ( w - BoxSize ) / 2 && x <= ( w - BoxSize ) / 2 + BoxSize &&
			    y >= ( h - BoxSize ) / 2 && y <= ( h - BoxSize ) / 2 + BoxSize ) {
			i->toggle();
		    }
		    emit signalDoneChanged( i->isChecked() );
		}
	    }
            break;
        case 1:
            break;
        case 2:
	  // may as well edit it...
	  // menuTimer->stop();
	  // emit signalEdit();
	  // Show detailed view of the selected entry
	  {
	    menuTimer->stop();
	    ToDoEvent *todo = todoList[static_cast<CheckItem*>(item(row, 0))];
	    emit showDetails( *todo );
	  }
	  break;
        case 3: 
	  // may as well edit it...
	  menuTimer->stop();
	  // emit signalEdit();
	  break;
    }
}

void TodoTable::slotPressed( int row, int col, int, const QPoint &pos )
{
    if ( col == 2 && cellGeometry( row, col ).contains(pos) )
	menuTimer->start( 750, TRUE );
}

void TodoTable::slotShowMenu()
{
    emit signalShowMenu( QCursor::pos() );
}

void TodoTable::slotCurrentChanged( int, int )
{
    menuTimer->stop();
}

void TodoTable::internalAddEntries( QList<ToDoEvent> &list )
{
    setNumRows( list.count() );
    int row = 0;
    ToDoEvent *it;
    for ( it = list.first(); it; it = list.next() )
	insertIntoTable( it, row++ );
}


ToDoEvent TodoTable::currentEntry() const
{
  //qWarning ("in currentEntry\n");

    QTableItem *i = item( currentRow(), 0 );
    if ( !i || rowHeight( currentRow() ) <= 0 )
        return ToDoEvent();
    ToDoEvent *todo = todoList[(CheckItem*)i];
    todo->setCompleted( ( (CheckItem*)item( currentRow(), 0 ) )->isChecked() );
    todo->setPriority( ( (ComboItem*)item( currentRow(), 1 ) )->text().toInt() );
    return *todo;
}

void TodoTable::replaceCurrentEntry( const ToDoEvent &todo, bool fromTableItem )
{
    int row = currentRow();
    updateJournal( todo, ACTION_REPLACE);

    if ( !fromTableItem ) {
	journalFreeReplaceEntry( todo, row );
	updateVisible();
    }
}

void TodoTable::removeCurrentEntry()
{
    ToDoEvent *oldTodo;
    int row = currentRow();
    CheckItem *chk;

    chk = static_cast<CheckItem*>(item(row, 0 ));
    if ( !chk )
	return;
    oldTodo = todoList[chk];
    todoList.remove( chk );
    oldTodo->setCompleted( chk->isChecked() );
    oldTodo->setPriority( static_cast<ComboItem*>(item(row, 1))->text().toInt() );
    realignTable( row );
    updateVisible();
    updateJournal( *oldTodo, ACTION_REMOVE);
    delete oldTodo;
}


bool TodoTable::save( const QString &fn )
{
    QString strNewFile = fn + ".new";
    QFile::remove( strNewFile ); // just to be sure
    ToDoDB todoDB( strNewFile );
    for ( QMap<CheckItem*, ToDoEvent *>::Iterator it = todoList.begin();
	  it != todoList.end(); ++it ) {
        if ( !item( it.key()->row(), 0 ) )
            continue;
        ToDoEvent *todo = *it;
	// sync item with table
	todo->setCompleted( ((CheckItem*)item(it.key()->row(), 0))->isChecked() );
	todo->setPriority( ((ComboItem*)item( it.key()->row(), 1))->text().toInt() );
	todoDB.addEvent( *todo );
    }
    if(!todoDB.save() ){
      QFile::remove( strNewFile );
      return false;
    };
    // now do the rename
    if ( ::rename( strNewFile, fn ) < 0 )
	qWarning( "problem renaming file %s to %s errno %d",
		  strNewFile.latin1(), fn.latin1(), errno );

    // remove the journal
    QFile::remove( journalFileName() );
    return true;
}

void TodoTable::load( const QString &fn )
{
  if ( QFile::exists(journalFileName()) ) {
    applyJournal();
    QFile::remove(journalFileName() );
  }
    loadFile( fn );
//     QTable::sortColumn(2,TRUE,TRUE);
//     QTable::sortColumn(1,TRUE,TRUE);
    QTable::sortColumn(0,TRUE,TRUE);
    setCurrentCell( 0, 2 );
    setSorting(true );
    mDayTimer->start( 60 * 1000 ); // gone in 60 seconds?
}
void TodoTable::updateVisible()
{
    if ( !isUpdatesEnabled() )
	return;
  
    if (showDeadl){
      showColumn (3);
      adjustColumn(3);
    }else{
      hideColumn (3);
      adjustColumn(2);
    }

    int visible = 0;
    int id = mCat.id( "Todo List", showCat );
    for ( int row = 0; row < numRows(); row++ ) {
	CheckItem *ci = (CheckItem *)item( row, 0 );
	ToDoEvent *t = todoList[ci];
	QArray<int> vlCats = t->categories();
	bool hide = false;
	if ( !showComp && ci->isChecked() )
	    hide = true;
	if ( !showCat.isEmpty() ) {
	    if ( showCat == tr( "Unfiled" ) ) {
		if ( vlCats.count() > 0 )
		    hide = true;
	    } else {
		// do some comparing, we have to reverse our idea here... which idea? - zecke
		if ( !hide ) {
		    hide = true;
		    for ( uint it = 0; it < vlCats.count(); ++it ) {
			if ( vlCats[it] == id ) {
			    hide = false;
			    break;
			}
		    }
		}
	    }
	}
	if ( hide ) {
	    if ( currentRow() == row )
		setCurrentCell( -1, 0 );
	    if ( rowHeight( row ) > 0 )
		hideRow( row );
	} else {
	    if ( rowHeight( row ) == 0 ) {
		showRow( row );
		adjustRow( row );
	    }
	    visible++;
	}
    }
    if ( !visible )
	setCurrentCell( -1, 0 );
}

void TodoTable::viewportPaintEvent( QPaintEvent *pe )
{
    if ( enablePainting )
	QTable::viewportPaintEvent( pe );
}

void TodoTable::setPaintingEnabled( bool e )
{
    if ( e != enablePainting ) {
	if ( !enablePainting ) {
	    enablePainting = true;
	    rowHeightChanged( 0 );
	    viewport()->update();
	} else {
	    enablePainting = false;
	}
    }
}

void TodoTable::clear()
{
    for ( QMap<CheckItem*, ToDoEvent *>::Iterator it = todoList.begin();
	  it != todoList.end(); ++it ) {
	ToDoEvent *todo = it.data();
	updateJournal( *todo, ACTION_REMOVE );
	delete todo;
    }
    todoList.clear();
    for ( int r = 0; r < numRows(); ++r ) {
	for ( int c = 0; c < numCols(); ++c ) {
            if ( cellWidget( r, c ) )
                clearCellWidget( r, c );
	    clearCell( r, c );
	}
    }
    setNumRows( 0 );
}

void TodoTable::sortColumn( int col, bool ascending, bool /*wholeRows*/ )
{
    // The default for wholeRows is false, however
    // for this todo table we want to exchange complete
    // rows when sorting. Also, we always want ascending, since
    // the values have a logical order.
    QTable::sortColumn( col, ascending, TRUE );
    updateVisible();
}

void TodoTable::slotCheckPriority(int row, int col )
{
    // kludgey work around to make forward along the updated priority...
    if ( col == 1 ) {
        // let everyone know!!
        ComboItem* i = static_cast<ComboItem*>( item( row, col ) );
        emit signalPriorityChanged( i->text().toInt() );
    }
}


void TodoTable::updateJournal( const ToDoEvent &todo, journal_action action )
{
    QFile f( journalFileName() );
    if ( !f.open(IO_WriteOnly|IO_Append) )
	return;
    QString buf;
    QCString str;
    buf = "<Task";
    //    todo.save( buf );
    buf += " Action=\"" + QString::number( int(action) ) + "\"";
    buf += " Uid=\"" + QString::number( todo.uid() ) + "\""; // better write the id
    buf += " Completed=\""+ QString::number((int)todo.isCompleted() ) + "\"";
    buf += " HasDate=\""+ QString::number((int)todo.hasDate() ) +"\"";
    buf += " Priority=\"" + QString::number( todo.priority() ) + "\"";
    QArray<int> arrat = todo.categories();
    QString attr;
    for(uint i=0; i < arrat.count(); i++ ){
      attr.append(QString::number(arrat[i])+";" );
    }
    if(!attr.isEmpty() ) // remove the last ;
      attr.remove(attr.length()-1, 1 );
    buf += " Categories=\"" + attr + "\"";
    buf += " Description=\"" + todo.description() + "\"";
    if(todo.hasDate() ) {
      buf += " DateYear=\""+QString::number( todo.date().year() ) + "\"";
      buf += " DateMonth=\"" + QString::number( todo.date().month() ) + "\"";
      buf += " DateDay=\"" + QString::number( todo.date().day() ) + "\"";
    }
    buf += "/>\n";
    str = buf.utf8();
    f.writeBlock( str.data(), str.length() );
    f.close();
}

void TodoTable::rowHeightChanged( int row )
{
    if ( enablePainting )
	QTable::rowHeightChanged( row );
}

void TodoTable::loadFile( const QString &/*we use the standard*/ )
{

  QList<ToDoEvent> list;
  ToDoDB todoDB;
  QValueList<ToDoEvent> vaList = todoDB.rawToDos();
  for(QValueList<ToDoEvent>::ConstIterator it = vaList.begin(); it != vaList.end(); ++it ){
    ToDoEvent *event = new ToDoEvent( (*it) );
    list.append( event );
  }   
  vaList.clear();
  //     qDebug("parsing done=%d", t.elapsed() );
  if ( list.count() > 0 ) {
    internalAddEntries( list );
    list.clear();
  }
//     qDebug("loading done: t=%d", t.elapsed() );
}

void TodoTable::journalFreeReplaceEntry( const ToDoEvent &todo, int row )
{
    QString strTodo;
    strTodo = todo.description().left(40).simplifyWhiteSpace();
    if ( row == -1 ) {
	QMapIterator<CheckItem*, ToDoEvent *> it;
	for ( it = todoList.begin(); it != todoList.end(); ++it ) {
	    if ( *(*it) == todo ) {
		row = it.key()->row();
		it.key()->setChecked( todo.isCompleted() );
		static_cast<ComboItem*>(item(row, 1))->setText( QString::number(todo.priority()) );
		item( row, 2 )->setText( strTodo );

		if (showDeadl){
		  static_cast<DueTextItem*>(item(row,3))->setToDoEvent(&todo );
		}

		*(*it) = todo;
	    }
	}
    } else {
	ToDoEvent *t = todoList[static_cast<CheckItem*>(item(row, 0))];
	todoList.remove( static_cast<CheckItem*>(item(row, 0)) );
	delete t;
	static_cast<CheckItem*>(item(row, 0))->setChecked( todo.isCompleted() );
	static_cast<ComboItem*>(item(row, 1))->setText( QString::number(todo.priority()) );
	item( row, 2 )->setText( strTodo );

 	if (showDeadl){
	  static_cast<DueTextItem*>(item(row,3))->setToDoEvent(&todo );
 	}
	ToDoEvent *ev = new ToDoEvent( todo );
	todoList.insert( static_cast<CheckItem*>(item(row,0)), new ToDoEvent(ev) );
    }
}

void TodoTable::journalFreeRemoveEntry( int row )
{
    CheckItem *chk;
    chk = static_cast<CheckItem*>(item(row, 0 ));
    if ( !chk )
	return;
    todoList.remove( chk );

    realignTable( row );
}

void TodoTable::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Space || e->key() == Key_Return ) {
	switch ( currentColumn() ) {
	    case 0: {
		CheckItem *i = static_cast<CheckItem*>(item(currentRow(),
							    currentColumn()));
		if ( i )
		    i->toggle();
		break;
	    }
	    case 1:
		break;
	    case 2:
		emit signalEdit();
	    default:
		break;
	}
    } else {
	QTable::keyPressEvent( e );
    }
}

QStringList TodoTable::categories()
{
    // This is called seldom, so calling a load in here
    // should be fine.
    mCat.load( categoryFileName() );
    QStringList categoryList = mCat.labels( "Todo List" );
    return categoryList;
}

void TodoTable::slotDoFind( const QString &findString, bool caseSensitive,
			    bool backwards, int category )
{
    // we have to iterate through the table, this gives the illusion that
    // sorting is actually being used.
    if ( currFindRow < -1 )
	currFindRow = currentRow() - 1;
    clearSelection( TRUE );
    int rows,
	row;
    CheckItem *chk;
    QRegExp r( findString );

    r.setCaseSensitive( caseSensitive );
    rows = numRows();
    static bool wrapAround = true;

   if ( !backwards ) {
    for ( row = currFindRow + 1; row < rows; row++ ) {
	chk = static_cast<CheckItem*>( item(row, 0) );
	if ( taskCompare(*(todoList[chk]), r, category) )
	    break;
    }
    } else {
 	for ( row = currFindRow - 1; row > -1; row-- ) {
 	    chk = static_cast<CheckItem*>( item(row, 0) );
 	    if ( taskCompare(*(todoList[chk]), r, category) )
 		break;
 	}
   }
    if ( row >= rows || row < 0 ) {
	if ( row < 0 )
	    currFindRow = rows;
	else
	    currFindRow = -1;
	if ( wrapAround )
	    emit signalWrapAround();
	else
	    emit signalNotFound();
	wrapAround = !wrapAround;
    } else {
	currFindRow = row;
	QTableSelection foundSelection;
	foundSelection.init( currFindRow, 0 );
	foundSelection.expandTo( currFindRow, numCols() - 1 );
	addSelection( foundSelection );
	setCurrentCell( currFindRow, numCols() - 1 );
	// we should always be able to wrap around and find this again,
	// so don't give confusing not found message...
	wrapAround = true;
    }
}

int TodoTable::showCategoryId() const
{
    int id;
    id = -1;
    // if allcategories are selected, you get unfiled...
    if ( showCat != tr( "Unfiled" ) && showCat != tr( "All" ) )
	id = mCat.id( "Todo List", showCat );
    return id;
}
void TodoTable::applyJournal() 
{
  // we need to hack
  QFile file( journalFileName() );
  if( file.open(IO_ReadOnly ) ) {
    QByteArray ar = file.readAll();
    file.close();
    QFile file2( journalFileName() + "_new" );
    if( file2.open(IO_WriteOnly ) ){
      QTextStream str(&file2 );
      str << QString::fromLatin1("<Tasks>") << endl;
      str << ar.data();
      str << QString::fromLatin1("</Tasks>") << endl;
      file2.close();
    }
    XMLElement *root = XMLElement::load(journalFileName()+ "_new");
    XMLElement *el = root->firstChild();
    el = el->firstChild();
    ToDoDB tododb; // allready loaded ;)
    bool ok;
    int action;
    QString dummy;
    while( el ){
      dummy = el->attribute("Action" );
      action = dummy.toInt(&ok );
      ToDoEvent ev = xmlToEvent( el );
      if(ok ){
	switch( action){
	case ACTION_ADD:
	  tododb.addEvent(ev );
	  break;
	case ACTION_REMOVE:
	  tododb.removeEvent( ev );
	  break;
	case ACTION_REPLACE:
	  tododb.replaceEvent( ev );
	  break;
	}
      }
      el = el->nextChild();
    }
    QFile::remove(journalFileName()+ "_new" );
    tododb.save();
  }
}
void TodoTable::slotCheckDay()
{
  QDate date = QDate::currentDate();
  if( mDay.daysTo(date )!= 0 ){
    setPaintingEnabled( FALSE );
    for(int i=0; i < numRows(); i++ ){
      ToDoEvent *t = todoList[static_cast<CheckItem*>(item(i, 0))];      
      static_cast<DueTextItem*>(item(i, 3) )->setToDoEvent( t );

    }
    setPaintingEnabled( TRUE );
    mDay = date;
    }
  mDayTimer->start( 60 * 1000 ); // 60 seconds
}
// check Action and decide
/*
void TodoTable::doApply(XMLElement *el )
{
  QString dummy;
  bool ok;
  int action;
  dummy = el->attribute("Action" );
  action = dummy.toInt(&ok );
  ToDoEvent ev = xmlToEvent( el );
  if( ok ){
    switch( action ){
    case ACTION_ADD:
      addEntry( ev );
      break;
    case ACTION_REMOVE:{ // find an entry with the same uid and remove it then
      break;
    }
    case ACTION_REPLACE:
      break;
    }
  }
}
*/
namespace {
static bool taskCompare( const ToDoEvent &task, const QRegExp &r, int category )
{
    bool returnMe;
    QArray<int> cats;
    cats = task.categories();

    returnMe = false;
    if ( (category == -1 && cats.count() == 0) || category == -2 )
	returnMe = task.match( r );
    else {
	int i;
	for ( i = 0; i < int(cats.count()); i++ ) {
	    if ( cats[i] == category ) {
		returnMe = task.match( r );
		break;
	    }
	}
    }
    return returnMe;
}

static QString journalFileName()
{
    QString str;
    str = getenv( "HOME" );
    str += "/.opie_todojournal";
    return str;
}
static ToDoEvent xmlToEvent( XMLElement *element )
{
  QString dummy;
  ToDoEvent event;
  bool ok;
  int dumInt;
  // completed
  dummy = element->attribute("Completed" );
  dumInt = dummy.toInt(&ok );
  if(ok ) event.setCompleted( dumInt == 0 ? false : true );
  // hasDate
  dummy = element->attribute("HasDate" );
  dumInt = dummy.toInt(&ok );
  if(ok ) event.setHasDate( dumInt == 0 ? false: true );
  // set the date
  bool hasDa = dumInt;
  if ( hasDa ) { //parse the date
    int year, day, month = 0;
    year = day = month;
    // year
    dummy = element->attribute("DateYear" );
    dumInt = dummy.toInt(&ok );
    if( ok ) year = dumInt;
    // month
    dummy = element->attribute("DateMonth" );
    dumInt = dummy.toInt(&ok );
    if(ok ) month = dumInt;
    dummy = element->attribute("DateDay" );
    dumInt = dummy.toInt(&ok );
    if(ok ) day = dumInt;
    // set the date
    QDate date( year, month, day );
    event.setDate( date);
  }
  dummy = element->attribute("Priority" );
  dumInt = dummy.toInt(&ok );
  if(!ok ) dumInt = ToDoEvent::NORMAL;
  event.setPriority( dumInt );
  //description
  dummy = element->attribute("Description" );
  event.setDescription( dummy );
  // category
  dummy = element->attribute("Categories" );
  QStringList ids = QStringList::split(";", dummy );
  event.setCategories( ids );     
  
  //uid
  dummy = element->attribute("Uid" );
  dumInt = dummy.toInt(&ok );
  if(ok ) event.setUid( dumInt );
  return event;
}

}
// int TodoTable::rowHeight( int ) const
// {
//     return 18;
// }

// int TodoTable::rowPos( int row ) const
// {
//     return 18*row;
// }

// int TodoTable::rowAt( int pos ) const
// {
//     return QMIN( pos/18, numRows()-1 );
// }

