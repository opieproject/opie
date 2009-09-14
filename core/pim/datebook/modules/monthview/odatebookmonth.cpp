/* this is a straight copy of datemonthview. We can not make child of
 * it 'cause the origin view isn't virtual in any form.
 */
/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#include "odatebookmonth.h"
#include "datebooktypes.h"

#include <qpe/config.h>
#include <qpe/datebookmonth.h>

#include <qpe/qpeapplication.h>

#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qvaluestack.h>
#include <qwhatsthis.h>


//---------------------------------------------------------------------------

class ODateBookMonthTablePrivate
{
public:
    ODateBookMonthTablePrivate() : onMonday(false) {};
    ~ODateBookMonthTablePrivate() { mMonthEvents.clear(); };

    QValueList<EffectiveEvent> mMonthEvents;
    bool onMonday;
};

ODateBookMonthTable::ODateBookMonthTable( QWidget *parent, const char *name,
                                        DateBookDBHoliday *newDb  )
    : QTable( 6, 7, parent, name ),
      db( newDb )
{
    d = new ODateBookMonthTablePrivate();
    selYear = -1;
    selMonth = -1;
    selDay = -1;

    /* init these as well  make valgrind happy and be consistent with Qtopia1.6 -zecke */
    year = -1;
    month = -1;
    day = -1;

    Config cfg( "qpe" );
    cfg.setGroup( "Time" );
    d->onMonday = cfg.readBoolEntry( "MONDAY" );

    horizontalHeader()->setResizeEnabled( FALSE );
    // we have to do this here... or suffer the consequences later...
    for ( int i = 0; i < 7; i++ ){
    horizontalHeader()->resizeSection( i, 30 );
    setColumnStretchable( i, TRUE );
    }
    setupLabels();

    verticalHeader()->hide();
    setLeftMargin( 0 );
    for ( int i = 0; i < 6; ++i )
        setRowStretchable( i, TRUE );

    setSelectionMode( NoSelection );

    connect( this, SIGNAL( clicked(int,int,int,const QPoint&) ),
         this, SLOT( dayClicked(int,int) ) );
    connect( this, SIGNAL( currentChanged(int,int) ),
             this, SLOT( dragDay(int,int) ) );
    setVScrollBarMode( AlwaysOff );
    setHScrollBarMode( AlwaysOff );
}

ODateBookMonthTable::~ODateBookMonthTable()
{
    monthsEvents.clear();
    delete d;
}

void ODateBookMonthTable::setDate(int y, int m, int d)
{
    if (month == m && year == y) {
    if ( selYear == -1 )
        year = selYear;
    if ( selMonth == -1 )
        month = selMonth;
    int r1, c1, r2, c2;
    findDay(selDay, r1, c1);
    selDay = day = d;
    findDay(selDay, r2, c2);
    setCurrentCell( r2, c2 );
    //updateCell(r1,c1);
    //updateCell(r2,c2);
    } else {
    selYear = year = y;
    selMonth = month = m;
    selDay = day = d;
    setupTable();
    }
}

void ODateBookMonthTable::redraw()
{
    setupLabels();
    setupTable();
}

void ODateBookMonthTable::setWeekStart( bool onMonday )
{
    d->onMonday = onMonday;
    setupLabels();
    setupTable();
}

void ODateBookMonthTable::setupTable()
{
    QValueList<Calendar::Day> days = Calendar::daysOfMonth( year, month, d->onMonday );
    QValueList<Calendar::Day>::Iterator it = days.begin();
    int row = 0, col = 0;
    int crow = 0;
    int ccol = 0;
    for ( ; it != days.end(); ++it ) {
    DayItemMonth *i = (DayItemMonth *)item( row, col );
    if ( !i ) {
        i = new DayItemMonth( this, QTableItem::Never, "" );
        setItem( row, col, i );
    }
    Calendar::Day calDay = *it;
    i->clearEffEvents();
    i->setDay( calDay.date );
    i->setType( calDay.type );
    if ( i->day() == day && calDay.type == Calendar::Day::ThisMonth ) {
        crow = row;
        ccol = col;
    }

    updateCell( row, col );

    if ( col == 6 ) {
        ++row;
        col = 0;
    } else {
        ++col;
    }
    }
    setCurrentCell( crow, ccol );
    getEvents();
}

void ODateBookMonthTable::findDay( int day, int &row, int &col )
{
    QDate dtBegin( year, month, 1 );
    int skips = dtBegin.dayOfWeek();
    int effective_day = day + skips - 1; // row/columns begin at 0
    // make an extra adjustment if we start on Mondays.
    if ( d->onMonday )
    effective_day--;
    row = effective_day / 7;
    col = effective_day % 7;
}

bool ODateBookMonthTable::findDate( QDate date, int &row, int &col )
{
    int rows = numRows();
    int cols = numCols();
    for(int r=0;r<rows;r++) {
        for(int c=0;c<cols;c++) {
            if(getDateAt(r, c) == date) {
                row = r;
                col = c;
                return true;
            }
        }
    }
    return false;
}

void ODateBookMonthTable::dayClicked( int row, int col )
{
    changeDaySelection( row, col );
    emit dateClicked( selYear, selMonth,  selDay );
}

void ODateBookMonthTable::dragDay( int row, int col )
{
    changeDaySelection( row, col );
}

void ODateBookMonthTable::changeDaySelection( int row, int col )
{
    QDate selDate = getDateAt( row, col );
    selYear = selDate.year();
    selMonth = selDate.month();
    selDay = selDate.day();
}

QDate ODateBookMonthTable::getDateAt( int row, int col )
{
    int itemMonth, itemYear;

    DayItemMonth *i = (DayItemMonth*)item( row, col );
    if ( !i )
        return QDate(1900, 1, 1);
    switch ( i->type() ) {
    case Calendar::Day::ThisMonth:
            itemMonth = month;
        break;
    case Calendar::Day::PrevMonth:
            itemMonth = month-1;
        break;
    default:
            itemMonth = month+1;
    }

    itemYear = year;
    if ( itemMonth <= 0 ) {
        itemMonth = 12;
        itemYear--;
    }
    else if ( itemMonth > 12 ) {
        itemMonth = 1;
        itemYear++;
    }

    return QDate( itemYear, itemMonth, i->day());
}

void ODateBookMonthTable::viewportMouseReleaseEvent( QMouseEvent * )
{
    dayClicked( currentRow(), currentColumn() );
}

void ODateBookMonthTable::getEvents()
{
    if ( !db )
    return;

    QDate dtStart = getDateAt(0,0);
    QDate dtEnd = getDateAt(numRows()-1, numCols()-1);
    d->mMonthEvents = db->getEffectiveEvents( dtStart, dtEnd);
    QValueListIterator<EffectiveEvent> it = d->mMonthEvents.begin();
    // now that the events are sorted, basically go through the list, make
    // a small list for every day and set it for each item...
    // clear all the items...
    while ( it != d->mMonthEvents.end() ) {
    QValueList<EffectiveEvent> dayEvent;
    EffectiveEvent e = *it;
    ++it;
    dayEvent.append( e );
    while ( it != d->mMonthEvents.end()
            && e.date() == (*it).date() ) {
        dayEvent.append( *it );
        ++it;
    }
    int row, col;
        findDate( e.date(), row, col );
    DayItemMonth* w = static_cast<DayItemMonth*>( item( row, col ) );
    w->setEvents( dayEvent );
    updateCell( row, col );
    dayEvent.clear();
    }
}


void ODateBookMonthTable::setupLabels()
{
    for ( int i = 0; i < 7; ++i ) {
//  horizontalHeader()->resizeSection( i, 30 );
//  setColumnStretchable( i, TRUE );
    if ( d->onMonday )
        horizontalHeader()->setLabel( i, Calendar::nameOfDay( i + 1 ) );
    else {
        if ( i == 0 )
        horizontalHeader()->setLabel( i, Calendar::nameOfDay( 7 ) );
        else
        horizontalHeader()->setLabel( i, Calendar::nameOfDay( i ) );
    }
    }
}


//---------------------------------------------------------------------------

ODateBookMonth::ODateBookMonth( QWidget *parent, const char *name, bool ac,
                              DateBookDBHoliday *data )
    : QVBox( parent, name ),
      autoClose( ac )
{
    setFocusPolicy(StrongFocus);
    year = QDate::currentDate().year();
    month = QDate::currentDate().month();
    day = QDate::currentDate().day();
    header = new DateBookMonthHeader( this, "DateBookMonthHeader" );
    table = new ODateBookMonthTable( this, "DateBookMonthTable", data );
    header->setDate( year, month );
    table->setDate( year, month, QDate::currentDate().day() );
    header->setFocusPolicy(NoFocus);
    table->setFocusPolicy(NoFocus);
    connect( header, SIGNAL( dateChanged(int,int) ),
         this, SLOT( setDate(int,int) ) );
    connect( table, SIGNAL( dateClicked(int,int,int) ),
         this, SLOT( finalDate(int,int,int) ) );
    connect( qApp, SIGNAL(weekChanged(bool)), this,
         SLOT(slotWeekChange(bool)) );
    table->setFocus();
}

ODateBookMonth::~ODateBookMonth()
{

}

void ODateBookMonth::setDate( int y, int m )
{
    /* only change the date if this is a different date,
     * other wise we may mistakenly overide the day */
    if ( (y != year) || (m != month) ) {
    year = y;
    month = m;
    QDate nd( y, m, 1 );
    if ( nd.daysInMonth() < day )
        day = nd.daysInMonth();
    table->setDate( year, month, day );
    }
}

void ODateBookMonth::setDate( int y, int m, int d )
{
    header->setDate( y, m);
    table->setDate( y, m, d);
    year = y;
    month = m;
    day = d;
}

/* called when we wish to close or pass back the date */
void ODateBookMonth::finalDate(int y, int m, int d)
{
    setDate( y, m, d );

    emit dateClicked(y, m, d);
    //    emit dateClicked(QDate(y, m, d).toString());

    if ( autoClose && parentWidget() )
    parentWidget()->close();
}

void ODateBookMonth::setDate( QDate d)
{
    setDate(d.year(), d.month(), d.day());
}

void ODateBookMonth::redraw()
{
    table->setDate( year, month, day );
    table->redraw();
}

QDate  ODateBookMonth::selectedDate() const
{
    if ( !table )
    return QDate::currentDate();
    int y, m, d;
    table->getDate( y, m, d );
    return QDate( y, m, d );
}

void ODateBookMonth::slotWeekChange( bool startOnMonday )
{
    table->setWeekStart( startOnMonday );
}

void ODateBookMonth::keyPressEvent( QKeyEvent *e )
{
    switch(e->key()) {
    case Key_Up:
        setDate(QDate(year, month, day).addDays(-7));
        break;
    case Key_Down:
        setDate(QDate(year, month, day).addDays(7));
        break;
    case Key_Left:
        setDate(QDate(year, month, day).addDays(-1));
        break;
    case Key_Right:
        setDate(QDate(year, month, day).addDays(1));
        break;
    case Key_Space:
        qWarning("space");
        emit dateClicked(year, month, day);
        if ( autoClose && parentWidget() )
        parentWidget()->close();
        break;
    default:
        qWarning("ignore");
        e->ignore();
        break;
    }
}
