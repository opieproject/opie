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
#include <stdlib.h>
#include <cmath>

#include <qlineedit.h>
#include <qtimer.h>
#include <qpoint.h>
#include <qpopupmenu.h>

#include <qpe/config.h>

#include "mainwindow.h"
//#include "tableitems.h"
#include "tableview.h"

using namespace Todo;

namespace {
    static const int BoxSize = 14;
    static const int RowHeight = 20;
}


void TableView::initConfig() {
    Config config( "todo" );
    config.setGroup( "Options" );
    m_completeStrokeWidth = config.readNumEntry( "CompleteStrokeWidth", 8 );
}

TableView::TableView( MainWindow* window, QWidget* wid )
    : QTable(  wid ), TodoView( window ) {
    setUpdatesEnabled( false );
    viewport()->setUpdatesEnabled( false );
    m_enablePaint = false;
    setNumRows(0);
    setNumCols(4);

    setSorting( TRUE );
    setSelectionMode( NoSelection );
//    setColumnStretchable( 2, TRUE );
    setColumnStretchable( 3, FALSE );
    setColumnWidth(0, 20 );
    setColumnWidth(1, 35 );
    setColumnWidth(3, 18 );

    setLeftMargin( 0 );
    verticalHeader()->hide();

    horizontalHeader()->setLabel(0, tr("C.") );
    horizontalHeader()->setLabel(1, tr("Prior.") );
    horizontalHeader()->setLabel(2, tr("Description" ) );

//    setColumnStretchable(3, FALSE );

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
    setSortOrder( 0 );
    setAscending( TRUE );
    m_first = true;

    /* now let's init the config */
    initConfig();
}
/* a new day has started
 * update the day
 */
void TableView::newDay() {
    clear();
    updateView();
}
TableView::~TableView() {

}
void TableView::slotShowMenu() {
    QPopupMenu *menu = todoWindow()->contextMenu( current() );
    menu->exec(QCursor::pos() );
    delete menu;
}
QString TableView::type() const {
    return QString::fromLatin1( tr("Table View") );
}
int TableView::current() {
    int uid = sorted().uidAt(currentRow() );
    qWarning("uid %d", uid );
    return uid;
}
QString TableView::currentRepresentation() {
    return text( currentRow(), 2);
}
/* show overdue */
void TableView::showOverDue( bool ) {
    clear();
    updateView();
}

void TableView::updateView( ) {
    qWarning("update view");
    m_row = false;
    static int id;
    id = startTimer(2000 );
    /* FIXME we want one page to be read!
     *
     * Calculate that screensize
     */
    todoWindow()->setReadAhead( 4 );
    sort();
    OTodoAccess::List::Iterator it, end;
    it = sorted().begin();
    end = sorted().end();

    qWarning("setTodos");
    QTime time;
    time.start();
    m_enablePaint = false;
    setUpdatesEnabled( false );
    viewport()->setUpdatesEnabled( false );

    setNumRows( it.count() );
    if ( it.count() == 0 )
        killTimer(id);

    int elc = time.elapsed();
    setUpdatesEnabled( true );
    viewport()->setUpdatesEnabled( true );
    viewport()->update();

    m_enablePaint = true;
    int el = time.elapsed();
}
void TableView::setTodo( int, const OTodo&) {
    sort();

    /* repaint */
    repaint();
}
void TableView::addEvent( const OTodo&) {

    /* fix problems of not showing the 'Haken' */
    updateView();
}
/*
 * find the event
 * and then replace the complete row
 */
void TableView::replaceEvent( const OTodo& ev) {
    addEvent( ev );
}
/*
 * re aligning table can be slow too
 * FIXME: look what performs better
 * either this or the old align table
 */
void TableView::removeEvent( int ) {
    updateView();
}
void TableView::setShowCompleted( bool b) {
    qWarning("Show Completed %d" + b );
    updateView();
}
void TableView::setShowDeadline( bool b) {
    qWarning("Show DeadLine %d" + b );
    if (b)
        showColumn(3 );
    else
        hideColumn(3 );
}
void TableView::setShowCategory( const QString& str) {
    qWarning("setShowCategory");
    if ( str != m_oleCat || m_first )
        updateView();

    m_oleCat = str;
    m_first = false;

}
void TableView::clear() {
    setNumRows(0);
}
void TableView::slotClicked(int row, int col, int,
                            const QPoint& point) {
    if ( !cellGeometry(row, col ).contains(point ) )
        return;

    int ui= sorted().uidAt( row );


    switch( col ) {
    case 0:{
         int x = point.x() -columnPos( col );
         int y = point.y() -rowPos( row );
         int w = columnWidth( col );
         int h = rowHeight( row );
         if ( x >= ( w - BoxSize ) / 2 &&
              x <= ( w - BoxSize ) / 2 + BoxSize &&
              y >= ( h - BoxSize ) / 2 &&
              y <= ( h - BoxSize ) / 2 + BoxSize ) {
             OTodo todo = sorted()[row];
             todo.setCompleted( !todo.isCompleted() );
             TodoView::update( todo.uid(), todo );
             updateView();
         }
    }
        break;

    case 1:
        break;

    case 2: {
        m_menuTimer->stop();
        showTodo( ui );
        break;
    }
    case 3: {
        m_menuTimer->stop();
        TodoView::edit( ui );
        break;
    }
    }


}
void TableView::slotPressed(int row, int col, int,
                            const QPoint& point) {

    m_prevP = point;
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
QWidget* TableView::widget() {
    return this;
}
/*
 * We need to overwrite sortColumn
 * because we want to sort whole row
 * based
 * We event want to set the setOrder
 * to a sort() and update()
 */
void TableView::sortColumn( int col, bool asc, bool ) {
    qWarning("bool %d", asc );
    setSortOrder( col );
    setAscending( asc );
    updateView();
}
void TableView::viewportPaintEvent( QPaintEvent* e) {
    if (m_enablePaint )
        QTable::viewportPaintEvent( e );
}
/*
 * This segment is copyrighted by TT
 * it was taken from their todolist
 * application this code is GPL
 */
void TableView::paintCell(QPainter* p,  int row, int col, const QRect& cr, bool ) {
    const QColorGroup &cg = colorGroup();

    p->save();

    OTodo task = sorted()[row];

    p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Base ) );

    QPen op = p->pen();
    p->setPen(cg.mid());
    p->drawLine( 0, cr.height() - 1, cr.width() - 1, cr.height() - 1 );
    p->drawLine( cr.width() - 1, 0, cr.width() - 1, cr.height() - 1 );
    p->setPen(op);

    QFont f = p->font();
    QFontMetrics fm(f);

    switch(col) {
	case 0:
        {
            // completed field
            int marg = ( cr.width() - BoxSize ) / 2;
            int x = 0;
            int y = ( cr.height() - BoxSize ) / 2;
            p->setPen( QPen( cg.text() ) );
            p->drawRect( x + marg, y, BoxSize, BoxSize );
            p->drawRect( x + marg+1, y+1, BoxSize-2, BoxSize-2 );
            p->setPen( darkGreen );
            x += 1;
            y += 1;
            if ( task.isCompleted() ) {
                QPointArray a( 9*2 );
                int i, xx, yy;
                xx = x+2+marg;
                yy = y+4;
                for ( i=0; i<4; i++ ) {
                    a.setPoint( 2*i,   xx, yy );
                    a.setPoint( 2*i+1, xx, yy+2 );
                    xx++; yy++;
                }
                yy -= 2;
                for ( i=4; i<9; i++ ) {
                    a.setPoint( 2*i,   xx, yy );
                    a.setPoint( 2*i+1, xx, yy+2 );
                    xx++; yy--;
                }
                p->drawLineSegments( a );
            }
        }
        break;
    case 1:
        // priority field
    {
        QString text = QString::number(task.priority());
        p->drawText(2,2 + fm.ascent(), text);
    }
    break;
    case 2:
        // description field
    {
        QString text = task.summary().isEmpty() ?
                       task.description() :
                       task.summary();
        p->drawText(2,2 + fm.ascent(), text);
    }
    break;
    case 3:
    {
        QString text;
        if (task.hasDueDate()) {
            int off = QDate::currentDate().daysTo( task.dueDate() );
            text = QString::number(off) + tr(" day(s)");
            /*
             * set color if not completed
             */
            if (!task.isCompleted() ) {
                QColor color = Qt::black;
                if ( off < 0 )
                    color = Qt::red;
                else if ( off == 0 )
                    color = Qt::yellow;
                else if ( off > 0 )
                    color = Qt::green;
                p->setPen(color  );
            }
        } else {
            text = tr("None");
        }
        p->drawText(2,2 + fm.ascent(), text);
    }
    break;
    }
    p->restore();
}
QWidget* TableView::createEditor(int row, int col, bool )const {
    switch( col ) {
    case 1: {
        /* the priority stuff */
        QComboBox* combo = new QComboBox( viewport() );
        combo->insertItem( "1" );
        combo->insertItem( "2" );
        combo->insertItem( "3" );
        combo->insertItem( "4" );
        combo->insertItem( "5" );
        combo->setCurrentItem( sorted()[row].priority()-1 );
        return combo;
    }
        /* summary */
    case 2:{
        QLineEdit* edit = new QLineEdit( viewport() );
        edit->setText( sorted()[row].summary() );
        return edit;
    }
    case 0:
    default:
        return 0l;
    }
}
void TableView::setCellContentFromEditor(int row, int col ) {
    if ( col == 1 ) {
        QWidget* wid = cellWidget(row, 1 );
        if ( wid->inherits("QComboBox") ) {
            int pri = ((QComboBox*)wid)->currentItem() + 1;
            OTodo todo = sorted()[row];
            if ( todo.priority() != pri ) {
                todo.setPriority( pri  );
                TodoView::update( todo.uid(), todo );
                updateView();
            }
        }
    }else if ( col == 2) {
        QWidget* wid = cellWidget(row, 2);
        if ( wid->inherits("QLineEdit") ) {
            QString text = ((QLineEdit*)wid)->text();
            OTodo todo = sorted()[row];
            if ( todo.summary() != text ) {
                todo.setSummary( text );
                TodoView::update( todo.uid(), todo );
                updateView();
            }
        }
    }
}
void TableView::slotPriority() {
    setCellContentFromEditor( currentRow(), currentColumn() );
}
/*
 * We'll use the TimerEvent to read ahead or to keep the cahce always
 * filled enough.
 * We will try to read ahead 4 items in both ways
 * up and down. On odd or even we will currentRow()+-4 or +-9
 *
 */
void TableView::timerEvent( QTimerEvent* ev ) {
    qWarning("sorted %d", sorted().count() );
    if (sorted().count() == 0 )
        return;

    int row = currentRow();
    if ( m_row ) {
        int ro = row-4;
        if (ro < 0 ) ro = 0;
        sorted()[ro];

        ro = row+4;
        sorted()[ro];
    } else {
        int ro = row + 8;
        sorted()[ro];

        ro = row-8;
        if (ro < 0 ) ro = 0;
        sorted()[ro];
    }

    m_row = !m_row;
}

// We want  a strike through completed ;)
// durchstreichen to complete
/*
 * MouseTracking is off this mean we only receive
 * these events if the mouse button is pressed
 * We've the previous point saved
 * We check if the previous and current Point are
 * in the same row.
 * Then we check if they're some pixel horizontal away
 * if the distance between the two points is greater than
 * 8 we mark the underlying todo as completed and do a repaint
 */
void TableView::contentsMouseReleaseEvent( QMouseEvent* e) {
    int row = rowAt(m_prevP.y());
    if ( row == rowAt( e->y() ) && row != -1 ) {
        if ( ::abs( m_prevP.x() - e->x() ) >=  m_completeStrokeWidth ) {
            qWarning("current row %d", row );
            OTodo todo = sorted()[row];
            todo.setCompleted( !todo.isCompleted() );
            TodoView::update( todo.uid(), todo );
            updateView();
            return;
        }
    }
    QTable::contentsMouseReleaseEvent( e );
}
void TableView::contentsMouseMoveEvent( QMouseEvent* e ) {
    m_menuTimer->stop();
    QTable::contentsMouseMoveEvent( e );
}
