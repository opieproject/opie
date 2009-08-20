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

#include <qpe/config.h>

#include <opie2/odebug.h>

#include <qpe/qpeapplication.h>

#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qvaluestack.h>
#include <qwhatsthis.h>
#include <qtl.h>

using namespace Opie;

static const QColor s_colorNormalLight = QColor(255, 150, 150);
static const QColor s_colorRepeatLight = QColor(150, 150, 255);
static const QColor s_colorHolidayLight= QColor(150, 255, 150);

//---------------------------------------------------------------------------

class ODateBookMonthTablePrivate
{
public:
    ODateBookMonthTablePrivate() {};
    ~ODateBookMonthTablePrivate() { mMonthEvents.clear(); };

    OPimOccurrence::List mMonthEvents;
    bool onMonday;
};

ODateBookMonthTable::ODateBookMonthTable( QWidget *parent, const char *name)
    : QTable( 6, 7, parent, name )
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
    }
    else {
        selYear = year = y;
        selMonth = month = m;
        selDay = day = d;
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
}

void ODateBookMonthTable::setupTable()
{
    QValueList<Calendar::Day> days = Calendar::daysOfMonth( year, month, d->onMonday );
    QValueList<Calendar::Day>::Iterator it = days.begin();
    int row = 0, col = 0;
    int crow = 0;
    int ccol = 0;
    for ( ; it != days.end(); ++it ) {
        ODayItemMonth *i = (ODayItemMonth *)item( row, col );
        if ( !i ) {
            i = new ODayItemMonth( this, QTableItem::Never, "" );
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
        }
        else {
            ++col;
        }
    }
    setCurrentCell( crow, ccol );
    emit populateEvents();
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

    ODayItemMonth *i = (ODayItemMonth*)item( row, col );
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

void ODateBookMonthTable::layoutEvents()
{
    OPimOccurrence::List::ConstIterator it = d->mMonthEvents.begin();
    // now that the events are sorted, basically go through the list, make
    // a small list for every day and set it for each item...
    // clear all the items...
    while ( it != d->mMonthEvents.end() ) {
        OPimOccurrence::List dayEvent;
        OPimOccurrence e = *it;
        ++it;
        dayEvent.append( e );
        while ( it != d->mMonthEvents.end()
                && e.date() == (*it).date() ) {
            dayEvent.append( *it );
            ++it;
        }
        int row, col;
        if( findDate( e.date(), row, col ) ) {
            ODayItemMonth* w = static_cast<ODayItemMonth*>( item( row, col ) );
            w->setEvents( dayEvent );
            updateCell( row, col );
        }
        dayEvent.clear();
    }
}

void ODateBookMonthTable::visibleDateRange( QDate &start, QDate &end )
{
    start = getDateAt(0,0);
    end = getDateAt(numRows()-1, numCols()-1);
}

void ODateBookMonthTable::setEvents( const OPimOccurrence::List &list )
{
    d->mMonthEvents = list;

    // Sort the events, because layoutEvents() relies upon them being sorted
    // and we can't expect them to be sorted already
    qHeapSort( d->mMonthEvents );

    layoutEvents();
}

void ODateBookMonthTable::setupLabels()
{
    for ( int i = 0; i < 7; ++i ) {
//        horizontalHeader()->resizeSection( i, 30 );
//        setColumnStretchable( i, TRUE );
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

ODateBookMonth::ODateBookMonth( QWidget *parent, const char *name, bool ac )
    : QVBox( parent, name ),
      autoClose( ac )
{
    setFocusPolicy(StrongFocus);
    year = 1900;
    month = 1;
    day = 1;
    header = new DateBookMonthHeader( this, "DateBookMonthHeader" );
    table = new ODateBookMonthTable( this, "DateBookMonthTable" );
    header->setDate( year, month );
    table->setDate( year, month, day );
    header->setFocusPolicy(NoFocus);
    table->setFocusPolicy(NoFocus);
    connect( header, SIGNAL( dateChanged(int,int) ),
         this, SLOT( setDate(int,int) ) );
    connect( table, SIGNAL( dateClicked(int,int,int) ),
         this, SLOT( finalDate(int,int,int) ) );
    connect( table, SIGNAL( populateEvents() ),
         this, SIGNAL( populateEvents() ) );
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
    redraw();
}

void ODateBookMonth::setDate( int y, int m, int d )
{
    header->setDate( y, m);
    table->setDate( y, m, d);
    year = y;
    month = m;
    day = d;
    redraw();
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

QDate ODateBookMonth::selectedDate() const
{
    if ( !table )
        return QDate::currentDate();
    int y, m, d;
    table->getDate( y, m, d );
    return QDate( y, m, d );
}

QDate ODateBookMonth::date() const
{
    return QDate( year, month, day );
}

void ODateBookMonth::visibleDateRange( QDate &start, QDate &end )
{
    table->visibleDateRange( start, end );
}

void ODateBookMonth::setEvents( const OPimOccurrence::List &list )
{
    table->setEvents( list );
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

//---------------------------------------------------------------------------

class ODayItemMonthPrivate
{
public:
    ODayItemMonthPrivate() {};
    ~ODayItemMonthPrivate() { mDayEvents.clear(); };
    OPimOccurrence::List mDayEvents;
};

ODayItemMonth::ODayItemMonth( QTable *table, EditType et, const QString &t )
        : QTableItem( table, et, t )
{
    d = new ODayItemMonthPrivate();
}

ODayItemMonth::~ODayItemMonth()
{
    delete d;
}

void ODayItemMonth::setEvents( const OPimOccurrence::List &effEv )
{
    d->mDayEvents = effEv;
}

void ODayItemMonth::clearEffEvents()
{
    d->mDayEvents.clear();
}

void ODayItemMonth::paint( QPainter *p, const QColorGroup &cg,
                          const QRect &cr, bool selected )
{
    p->save();

    QColorGroup g( cg );
    g.setBrush( QColorGroup::Base, back );
    g.setColor( QColorGroup::Text, forg );
    if ( selected )
        p->setPen( g.highlightedText() );
    else
        p->setPen( g.text() );

    QValueStack<int> normalLine;
    QValueStack<int> repeatLine;
    QValueStack<int> travelLine;

    bool normalAllDay = FALSE;
    bool repeatAllDay = FALSE;
    bool travelAllDay = FALSE;
    bool holidayAllDay = FALSE;

    OPimOccurrence::List::ConstIterator itDays = d->mDayEvents.begin();

    for ( ; itDays != d->mDayEvents.end(); ++itDays ) {
        int w = cr.width();
        OPimEvent ev = (*itDays).toEvent();

        int f = (*itDays).startTime().hour(); // assume Effective event
        int t = (*itDays).endTime().hour();          // is truncated.

        if (ev.isAllDay()) {
            if (!ev.hasRecurrence()) {
                normalAllDay = TRUE;
                if (!ev.isValidUid()) {
                    holidayAllDay = TRUE;
                }
            }
            else {
                repeatAllDay = TRUE;
            }
        }
        else {
            int sLine, eLine;
            if (f == 0)
                sLine = 0;
            else if (f < 8 )
                sLine = 1;
            else if (f >= 17)
                sLine = w - 4;
            else {
                sLine = (f - 8) * (w - 8);
                if (sLine)
                    sLine /= 8;
                sLine += 4;
            }
            if (t == 23)
                eLine = w;
            else if (t < 8)
                eLine = 4;
            else if (t >= 17)
                eLine = w - 1;
            else {
                eLine = (t - 8) * (w - 8);
                if (eLine)
                    eLine /= 8;
                eLine += 4;
            }
            if (!ev.hasRecurrence()) {
                normalLine.push(sLine);
                normalLine.push(eLine);
            }
            else {
                repeatLine.push(sLine);
                repeatLine.push(eLine);
            }
        }
    }

    // draw the background
    if (normalAllDay || repeatAllDay || travelAllDay || holidayAllDay) {
        p->save();

        if (normalAllDay) {
            if (repeatAllDay) {
                p->fillRect( 0, 0, cr.width(), cr.height() / 2,
                        s_colorNormalLight );
                p->fillRect( 0, cr.height() / 2, cr.width(), cr.height() / 2,
                        colorRepeatLight );
            }
            else {
                if (!holidayAllDay) {
                    p->fillRect( 0, 0, cr.width(), cr.height(),
                    s_colorNormalLight );
                }
                else {
                    p->fillRect( 0, 0, cr.width(), cr.height(),
                    s_colorHolidayLight );
                }
            }
        }
        else if (repeatAllDay) {
            p->fillRect( 0, 0, cr.width(), cr.height(),
                    s_colorRepeatLight );
        }
    }
    else {
        p->fillRect( 0, 0, cr.width(),
                cr.height(), selected
                ?  g.brush( QColorGroup::Highlight )
                : g.brush( QColorGroup::Base ) );
    }

    // The lines
    // now for the lines.
    int h = 5;
    int y = cr.height() / 2 - h;

    while(normalLine.count() >= 2) {
        int x2 = normalLine.pop();
        int x1 = normalLine.pop();
        if (x2 < x1 + 2)
            x2 = x1 + 2;
        p->fillRect(x1, y, x2 - x1, h, colorNormal);
    }

    y += h;

    while(repeatLine.count() >= 2) {
        int x2 = repeatLine.pop();
        int x1 = repeatLine.pop();
        if (x2 < x1 + 2)
            x2 = x1 + 2;
        p->fillRect(x1, y, x2 - x1, h, colorRepeat);
    }


    // Finally, draw the number.
    QFont f = p->font();
    if(qApp->desktop()->width() >= 480)
    {
        f.setPointSize( f.pointSize() - 2 );
    }
    else
    {
        f.setPointSize( ( f.pointSize() / 3 ) * 2 );
    }
    p->setFont( f );
    QFontMetrics fm( f );
    p->drawText( 1, 1 + fm.ascent(), QString::number( day() ) );

    p->restore();
}

void ODayItemMonth::setType( Calendar::Day::Type t )
{
    switch ( t ) {
    case Calendar::Day::PrevMonth:
    case Calendar::Day::NextMonth:
        back = QBrush( QColor( 224, 224, 224 ) );
        forg = black;
        break;
    case Calendar::Day::ThisMonth:
        back = QBrush( white );
        forg = black;
        break;
    }
    typ = t;
}
