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
#include "config.h"
#include "datebookmonth.h"
#include "datebookdb.h"
#include <qtopia/private/event.h>
#include "resource.h"
#include <qpe/qpeapplication.h>
#include "timestring.h"

#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qvaluestack.h>
#include <qwhatsthis.h>


DateBookMonthHeader::DateBookMonthHeader( QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    setBackgroundMode( PaletteButton );

    begin = new QToolButton( this );
    begin->setFocusPolicy(NoFocus);
    begin->setPixmap( Resource::loadPixmap( "start" ) );
    begin->setAutoRaise( TRUE );
    begin->setFixedSize( begin->sizeHint() );
    QWhatsThis::add( begin, tr("Show January in the selected year") );

    back = new QToolButton( this );
    back->setFocusPolicy(NoFocus);
    back->setPixmap( Resource::loadPixmap( "back" ) );
    back->setAutoRaise( TRUE );
    back->setFixedSize( back->sizeHint() );
    QWhatsThis::add( back, tr("Show the previous month") );

    month = new QComboBox( FALSE, this );
    for ( int i = 0; i < 12; ++i )
	month->insertItem( Calendar::nameOfMonth( i + 1 ) );

    year = new QSpinBox( 1752, 8000, 1, this );

    next = new QToolButton( this );
    next->setFocusPolicy(NoFocus);
    next->setPixmap( Resource::loadPixmap( "forward" ) );
    next->setAutoRaise( TRUE );
    next->setFixedSize( next->sizeHint() );
    QWhatsThis::add( next, tr("Show the next month") );

    end = new QToolButton( this );
    end->setFocusPolicy(NoFocus);
    end->setPixmap( Resource::loadPixmap( "finish" ) );
    end->setAutoRaise( TRUE );
    end->setFixedSize( end->sizeHint() );
    QWhatsThis::add( end, tr("Show December in the selected year") );

    connect( month, SIGNAL( activated( int ) ),
	     this, SLOT( updateDate() ) );
    connect( year, SIGNAL( valueChanged( int ) ),
	     this, SLOT( updateDate() ) );
    connect( begin, SIGNAL( clicked() ),
	     this, SLOT( firstMonth() ) );
    connect( end, SIGNAL( clicked() ),
	     this, SLOT( lastMonth() ) );
    connect( back, SIGNAL( clicked() ),
	     this, SLOT( monthBack() ) );
    connect( next, SIGNAL( clicked() ),
	     this, SLOT( monthForward() ) );
    back->setAutoRepeat( TRUE );
    next->setAutoRepeat( TRUE );
}


DateBookMonthHeader::~DateBookMonthHeader()
{

}

void DateBookMonthHeader::updateDate()
{
    emit dateChanged( year->value(), month->currentItem() + 1 );
}

void DateBookMonthHeader::firstMonth()
{
    emit dateChanged( year->value(), 1 );
    month->setCurrentItem( 0 );
}

void DateBookMonthHeader::lastMonth()
{
    emit dateChanged( year->value(), 12 );
    month->setCurrentItem( 11 );
}

void DateBookMonthHeader::monthBack()
{
    if ( month->currentItem() > 0 ) {
	emit dateChanged( year->value(), month->currentItem() );
	month->setCurrentItem( month->currentItem() - 1 );
    } else {
	emit dateChanged( year->value() - 1, 12 );
	// we have a signal set to a changed value in year so we only need to change
	// year to get the result...
	month->setCurrentItem( 11 );
	year->setValue( year->value() - 1 );
    }
}

void DateBookMonthHeader::monthForward()
{
    if ( month->currentItem() < 11 ) {
	emit dateChanged( year->value(), month->currentItem() + 2 );
	month->setCurrentItem( month->currentItem() + 1 );
    } else {
	// we have a signal set to a changed value in year so we only need to change
	// year to get the result...
	month->setCurrentItem( 0 );
	year->setValue( year->value() + 1 );
    }
}

void DateBookMonthHeader::setDate( int y, int m )
{
    year->setValue( y );
    month->setCurrentItem( m - 1 );
}

//---------------------------------------------------------------------------

class DateBookMonthTablePrivate
{
public:
    DateBookMonthTablePrivate() {};
    ~DateBookMonthTablePrivate() { mMonthEvents.clear(); };

    QValueList<EffectiveEvent> mMonthEvents;
    bool onMonday;
};

DateBookMonthTable::DateBookMonthTable( QWidget *parent, const char *name,
                                        DateBookDB *newDb  )
    : QTable( 6, 7, parent, name ),
      db( newDb )
{
    d = new DateBookMonthTablePrivate();
    selYear = -1;
    selMonth = -1;
    selDay = -1;

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

    connect( this, SIGNAL( clicked( int, int, int, const QPoint & ) ),
	     this, SLOT( dayClicked( int, int ) ) );
    connect( this, SIGNAL( currentChanged( int, int ) ),
             this, SLOT( dragDay( int, int ) ) );
    setVScrollBarMode( AlwaysOff );
    setHScrollBarMode( AlwaysOff );
}

DateBookMonthTable::~DateBookMonthTable()
{
    monthsEvents.clear();
    delete d;
}

void DateBookMonthTable::setDate(int y, int m, int d)
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

void DateBookMonthTable::redraw()
{
    setupLabels();
    setupTable();
}

void DateBookMonthTable::setWeekStart( bool onMonday )
{
    d->onMonday = onMonday;
    setupLabels();
    setupTable();
}

void DateBookMonthTable::setupTable()
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

void DateBookMonthTable::findDay( int day, int &row, int &col )
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

void DateBookMonthTable::dayClicked( int row, int col )
{
    changeDaySelection( row, col );
    emit dateClicked( selYear, selMonth,  selDay );
}

void DateBookMonthTable::dragDay( int row, int col )
{
    changeDaySelection( row, col );
}

void DateBookMonthTable::changeDaySelection( int row, int col )
{
    DayItemMonth *i = (DayItemMonth*)item( row, col );
    if ( !i )
	return;
    switch ( i->type() ) {
	case Calendar::Day::ThisMonth:
	    selMonth = month;
	    break;
	case Calendar::Day::PrevMonth:
	    selMonth = month-1;
	    break;
	default:
	    selMonth = month+1;
    }

    selYear = year;
    if ( selMonth <= 0 ) {
	selMonth = 12;
	selYear--;
    } else if ( selMonth > 12 ) {
	selMonth = 1;
	selYear++;
    }
    selDay = i->day();
}


void DateBookMonthTable::viewportMouseReleaseEvent( QMouseEvent * )
{
    dayClicked( currentRow(), currentColumn() );
}

void DateBookMonthTable::getEvents()
{
    if ( !db )
	return;

    QDate dtStart( year, month, 1 );
    d->mMonthEvents = db->getEffectiveEvents( dtStart,
					      QDate( year, month,
						     dtStart.daysInMonth() ) );
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
	findDay( e.date().day(), row, col );
	DayItemMonth* w = static_cast<DayItemMonth*>( item( row, col ) );
	w->setEvents( dayEvent );
	updateCell( row, col );
	dayEvent.clear();
    }
}


void DateBookMonthTable::setupLabels()
{
    for ( int i = 0; i < 7; ++i ) {
// 	horizontalHeader()->resizeSection( i, 30 );
// 	setColumnStretchable( i, TRUE );
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

DateBookMonth::DateBookMonth( QWidget *parent, const char *name, bool ac,
                              DateBookDB *data )
    : QVBox( parent, name ),
      autoClose( ac )
{
    setFocusPolicy(StrongFocus);
    year = QDate::currentDate().year();
    month = QDate::currentDate().month();
    day = QDate::currentDate().day();
    header = new DateBookMonthHeader( this, "DateBookMonthHeader" );
    table = new DateBookMonthTable( this, "DateBookMonthTable", data );
    header->setDate( year, month );
    table->setDate( year, month, QDate::currentDate().day() );
    header->setFocusPolicy(NoFocus);
    table->setFocusPolicy(NoFocus);
    connect( header, SIGNAL( dateChanged( int, int ) ),
	     this, SLOT( setDate( int, int ) ) );
    connect( table, SIGNAL( dateClicked( int, int, int ) ),
	     this, SLOT( finalDate(int, int, int) ) );
    connect( qApp, SIGNAL(weekChanged(bool)), this,
	     SLOT(slotWeekChange(bool)) );
    table->setFocus();
}

DateBookMonth::~DateBookMonth()
{

}

void DateBookMonth::setDate( int y, int m )
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

void DateBookMonth::setDate( int y, int m, int d )
{
    header->setDate( y, m);
    table->setDate( y, m, d);
    year = y;
    month = m;
    day = d;
}

/* called when we wish to close or pass back the date */
void DateBookMonth::finalDate(int y, int m, int d)
{
    setDate( y, m, d );

    emit dateClicked(y, m, d);
    //    emit dateClicked(QDate(y, m, d).toString());

    if ( autoClose && parentWidget() )
	parentWidget()->close();
}

void DateBookMonth::setDate( QDate d)
{
    setDate(d.year(), d.month(), d.day());
}

void DateBookMonth::redraw()
{
    table->setDate( year, month, day );
    table->redraw();
}

QDate  DateBookMonth::selectedDate() const
{
    if ( !table )
	return QDate::currentDate();
    int y, m, d;
    table->getDate( y, m, d );
    return QDate( y, m, d );
}

void DateBookMonth::slotWeekChange( bool startOnMonday )
{
    table->setWeekStart( startOnMonday );
}

void DateBookMonth::keyPressEvent( QKeyEvent *e )
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
class DayItemMonthPrivate
{
public:
    DayItemMonthPrivate() {};
    ~DayItemMonthPrivate() { mDayEvents.clear(); };
    QValueList<EffectiveEvent> mDayEvents;
};

DayItemMonth::DayItemMonth( QTable *table, EditType et, const QString &t )
	: QTableItem( table, et, t )
{
    d = new DayItemMonthPrivate();
}

DayItemMonth::~DayItemMonth()
{
    daysEvents.clear();
    delete d;
}

void DayItemMonth::setEvents( const QValueList<EffectiveEvent> &effEv )
{
    d->mDayEvents = effEv;
}

void DayItemMonth::clearEffEvents()
{
    d->mDayEvents.clear();
}

void DayItemMonth::paint( QPainter *p, const QColorGroup &cg,
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

    QValueListIterator<EffectiveEvent> itDays = d->mDayEvents.begin();

    for ( ; itDays != d->mDayEvents.end(); ++itDays ) {
	int w = cr.width();
	Event ev = (*itDays).event();

	int f = (*itDays).start().hour(); // assume Effective event
	int t = (*itDays).end().hour(); 	 // is truncated.

	if (ev.isAllDay()) {
	    if (!ev.hasRepeat())
		normalAllDay = TRUE;
	    else
		repeatAllDay = TRUE;
	} else {
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
	    if (!ev.hasRepeat()) {
		normalLine.push(sLine);
		normalLine.push(eLine);
	    } else {
		repeatLine.push(sLine);
		repeatLine.push(eLine);
	    }
	}
    }

    // draw the background
    if (normalAllDay || repeatAllDay || travelAllDay) {
	p->save();

	if (normalAllDay)
	    if (repeatAllDay) {
		p->fillRect( 0, 0, cr.width(), cr.height() / 2,
			colorNormalLight );
		p->fillRect( 0, cr.height() / 2, cr.width(), cr.height() / 2,
			colorRepeatLight );
	    } else
		p->fillRect( 0, 0, cr.width(), cr.height(),
			colorNormalLight );
	    else if (repeatAllDay)
		p->fillRect( 0, 0, cr.width(), cr.height(),
			colorRepeatLight );
    } else {
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
    f.setPointSize( ( f.pointSize() / 3 ) * 2 );
    p->setFont( f );
    QFontMetrics fm( f );
    p->drawText( 1, 1 + fm.ascent(), QString::number( day() ) );

    p->restore();
}



void DayItemMonth::setType( Calendar::Day::Type t )
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



DateButton::DateButton( bool longDate, QWidget *parent, const char * name )
    :QPushButton( parent, name )
{
    longFormat = longDate;
    df = DateFormat('/', DateFormat::MonthDayYear, DateFormat::MonthDayYear);
    setDate( QDate::currentDate() );

    connect(this,SIGNAL(pressed()),this,SLOT(pickDate()));


}


void DateButton::pickDate()
{
    static QPopupMenu *m1 = 0;
    static DateBookMonth *picker = 0;
    if ( !m1 ) {
	m1 = new QPopupMenu( this );
	picker = new DateBookMonth( m1, 0, TRUE );
	m1->insertItem( picker );
	connect( picker, SIGNAL( dateClicked( int, int, int ) ),
		 this, SLOT( setDate( int, int, int ) ) );
	connect( picker, SIGNAL( dateClicked( int, int, int ) ),
		 this, SIGNAL( dateSelected( int, int, int ) ) );
	connect( m1, SIGNAL( aboutToHide() ),
		 this, SLOT( gotHide() ) );
    }
    picker->slotWeekChange( weekStartsMonday );
    picker->setDate( currDate.year(), currDate.month(), currDate.day() );
    m1->popup(mapToGlobal(QPoint(0,height())));
    picker->setFocus();
}


void DateButton::gotHide()
{
    // we have to redo the button...
    setDown( false );
}


//    void dateSelected( int year, int month, int day );

void DateButton::setWeekStartsMonday( int b )
{
    weekStartsMonday = b;
}

void DateButton::setDate( int y, int m, int d )
{
    setDate( QDate( y,m,d) );
}

void DateButton::setDate( QDate d )
{
    currDate = d;
    setText( longFormat ? TimeString::longDateString( d, df ) :
	     TimeString::shortDate( d, df ) );

}

void DateButton::setDateFormat( DateFormat f )
{
    df = f;
    setDate( currDate );
}

bool DateButton::customWhatsThis() const
{
    return TRUE;
}

