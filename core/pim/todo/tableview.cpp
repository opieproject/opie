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
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qtimer.h>
#include <qpoint.h>
#include <qpopupmenu.h>

#include "mainwindow.h"
//#include "tableitems.h"
#include "tableview.h"

using namespace Todo;


TableView::TableView( MainWindow* window, QWidget* wid )
    : QTable(  wid ), TodoView( window ) {
    setUpdatesEnabled( false );
    viewport()->setUpdatesEnabled( false );
    m_enablePaint = false;
    setNumRows(0);
    setNumCols(4);

    setSorting( TRUE );
    setSelectionMode( NoSelection );
    setColumnStretchable( 2, TRUE );
    setColumnWidth(0, 20 );
    setColumnWidth(1, 35 );

    setLeftMargin( 0 );
    verticalHeader()->hide();

    horizontalHeader()->setLabel(0, tr("C.") );
    horizontalHeader()->setLabel(1, tr("Prior.") );
    horizontalHeader()->setLabel(2, tr("Description" ) );

    setColumnStretchable(3, FALSE );
    setColumnWidth(3, 20 );
    horizontalHeader()->setLabel(3, tr("Deadline") );

    if ( todoWindow()->showDeadline() )
        showColumn( 3);
    else
        hideColumn(3 );

    connect((QTable*)this, SIGNAL( clicked( int, int, int, const QPoint& ) ),
            this, SLOT( slotClicked(int, int, int, const QPoint& ) ) );
    connect((QTable*)this, SIGNAL( pressed( int, int, int, const QPoint& ) ),
            this, SLOT( slotPressed(int, int, int, const QPoint& ) ) );
    connect((QTable*)this, SIGNAL(valueChanged(int, int) ),
            this, SLOT( slotValueChanged(int, int) ) );
    connect((QTable*)this, SIGNAL(currentChanged(int, int) ),
            this, SLOT( slotCurrentChanged(int, int) ) );

    m_menuTimer = new QTimer( this );
    connect( m_menuTimer, SIGNAL(timeout()),
             this, SLOT(slotShowMenu()) );

    m_enablePaint = true;
    setUpdatesEnabled( true );
    viewport()->setUpdatesEnabled( true );
    viewport()->update();
}
/* a new day has started
 * update the day
 */
void TableView::newDay() {
    clear();
    setTodos( begin(),end() );
}
TableView::~TableView() {

}
void TableView::slotShowMenu() {
    QPopupMenu *menu = todoWindow()->contextMenu( current() );
    menu->exec(QCursor::pos() );
    delete menu;
}
OTodo TableView::find(int uid ) {
    OTodo ev = TodoView::event( uid );
    return ev;
}
void TableView::updateFromTable( const OTodo& ev, CheckItem* item ) {
    TodoView::update( ev.uid(), ev );

    /* update the other columns */
    /* if completed or not we need to update
     * the table
     *
     * We've two cases
     * either item or !item
     * this makes cases more easy
     */
    if ( !item ) {
        item = m_cache[ev.uid()];
    }
    DueTextItem *due = dueItem( item->row() );
    due->setCompleted( ev.isCompleted() );
}
QString TableView::type() const {
    return QString::fromLatin1( tr("Table View") );
}
int TableView::current() {
    int cur = 0;
    CheckItem* i = checkItem( currentRow() );
    if (i )
        cur = i->uid();

    return cur;
}
QString TableView::currentRepresentation() {
    return text( currentRow(), 2);
}
/* show overdue */
void TableView::showOverDue( bool ) {
    clear();
    setTodos( begin(), end() );
}

void TableView::setTodos( OTodoAccess::List::Iterator it,
                          OTodoAccess::List::Iterator end ) {
    qWarning("setTodos");
    QTime time;
    time.start();
    m_enablePaint = false;
    setUpdatesEnabled( false );
    viewport()->setUpdatesEnabled( false );
    clear();
    QString currentCat = todoWindow()->currentCategory();
    bool showCompleted = todoWindow()->showCompleted();
    bool showOverDue   = todoWindow()->showOverDue();
    qWarning( "Current Category:" + todoWindow()->currentCategory() );
    int id = todoWindow()->currentCatId();
    QTime t;
    t.start();
    setNumRows( it.count() );
    uint i = 0;
    for (; it != end; ++it ) {
        OTodo todo = (*it);
        /* test if the categories match */
        if ( !currentCat.isEmpty() &&
             !todo.categories().contains( id ) ) {
            continue;
        }
        /* the item is completed but we shouldn't show it */
        if ( !showCompleted && todo.isCompleted() ) {
            qWarning("isCompleted ");
            continue;
        }
        /* the item is not overdue but we should only show overdue */
        if ( showOverDue && !todo.isOverdue() ) {
            continue;
        }
        /* now it's fine to add it */
        insertTodo(  todo, i );
	i++;
    }
    setNumRows( i );
    int elc = time.elapsed();
    qWarning("Adding took %d", elc/1000 );
    setUpdatesEnabled( true );
    viewport()->setUpdatesEnabled( true );
    viewport()->update();
    m_enablePaint = true;
    int el = time.elapsed();
    qWarning("adding took %d", el/1000 );
}
void TableView::setTodo( int uid, const OTodo& ev ) {
    QMap<int, CheckItem*>::Iterator it = m_cache.find( uid );

    if ( it != m_cache.end() ) {
        int row = it.data()->row();

        /* update checked */
        CheckItem* check = checkItem(row );
        if (check)
            check->setChecked( ev.isCompleted() );

        /* update the text */
        QString sum = ev.summary();
        setText(row, 2, sum.isEmpty() ?
                ev.description().left(40).simplifyWhiteSpace() :
                sum );

        /* update priority */
        setText(row, 1, QString::number( ev.priority() ) );

        /* update DueDate */
        DueTextItem *due = dueItem( row );
        due->setToDoEvent( ev );
    }
}
void TableView::addEvent( const OTodo& ev) {
    int row= numRows();
    setNumRows( row + 1 );
    insertTodo( ev, row );
}
/*
 * find the event
 * and then replace the complete row
 */
void TableView::replaceEvent( const OTodo& ev) {
    setTodo( ev.uid(), ev );
}
/*
 * re aligning table can be slow too
 * FIXME: look what performs better
 * either this or the old align table
 */
void TableView::removeEvent( int ) {
    clear();
    setTodos( begin(), end() );
}
void TableView::setShowCompleted( bool b) {
    qWarning("Show Completed %d" + b );
    setTodos( begin(), end() );
}
void TableView::setShowDeadline( bool b) {
    qWarning("Show DeadLine %d" + b );
    if (b)
        showColumn(3 );
    else
        hideColumn(3 );
}
void TableView::setShowCategory( const QString& ) {
    qWarning("setShowCategory");
    setTodos( begin(), end() );
}
void TableView::clear() {
    m_cache.clear();
    int rows = numRows();
    for (int r = 0; r < rows; r++ ) {
        for (int c = 0; c < numCols(); c++ ) {
            if ( cellWidget(r, c) )
                clearCellWidget(r, c );
            clearCell(r, c);
        }
    }
    setNumRows( 0);
}
QArray<int> TableView::completed() {
    int row = numRows();
    QArray<int> ids( row );

    int j=0;
    for (int i = 0; i < row; i++ ) {
        CheckItem* item = checkItem(i );
        if (item->isChecked() ) {
            ids[j] = item->uid();
            j++;
        }
    }
    ids.resize( j );
    return ids;
}
void TableView::slotClicked(int row, int col, int,
                            const QPoint& point) {
    if ( !cellGeometry(row, col ).contains(point ) )
        return;


    switch( col ) {
    case 0: {
        CheckItem* item = checkItem( row );
        /*
         * let's see if we centered clicked
         */
        if ( item ) {
            int x = point.x() -columnPos( col );
            int y = point.y() -rowPos( row );
            int w = columnWidth( col );
            int h = rowHeight( row );
            if ( x >= ( w - OCheckItem::BoxSize ) / 2 &&
                 x <= ( w - OCheckItem::BoxSize ) / 2 + OCheckItem::BoxSize &&
                 y >= ( h - OCheckItem::BoxSize ) / 2 &&
                 y <= ( h - OCheckItem::BoxSize ) / 2 + OCheckItem::BoxSize )
                item->toggle();
        }
    }
        break;

    case 1:
        break;

    case 2: {
        m_menuTimer->stop();
        showTodo( checkItem(row)->uid() );
        break;
    }
    case 3: {
        m_menuTimer->stop();
        TodoView::edit( checkItem(row)->uid() );
        break;
    }
    }


}
void TableView::slotPressed(int row, int col, int,
                            const QPoint& point) {

    /* TextColumn column */
    if ( col == 2 && cellGeometry( row, col ).contains( point ) )
        m_menuTimer->start( 750, TRUE );
}
void TableView::slotValueChanged( int, int ) {
    qWarning("Value Changed");
}
void TableView::slotCurrentChanged(int, int ) {
    m_menuTimer->stop();
}
/*
 * hardcode to column 0
 */
CheckItem* TableView::checkItem( int row ) {
    CheckItem *i = static_cast<CheckItem*>( item( row, 0 ) );
    return i;
}
DueTextItem* TableView::dueItem( int row ) {
    DueTextItem* i = static_cast<DueTextItem*> ( item(row, 3 ) );
    return i;
}
QWidget* TableView::widget() {
    return this;
}
/*
 * We need to overwrite sortColumn
 * because we want to sort whole row
 * based
 */
void TableView::sortColumn( int row, bool asc, bool ) {
    QTable::sortColumn( row, asc, TRUE );

}
void TableView::viewportPaintEvent( QPaintEvent* e) {
    qWarning("Paint event" );
    if (m_enablePaint )
        QTable::viewportPaintEvent( e );
}
