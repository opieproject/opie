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
#include "datebookweek.h"
#include "datebookweekheaderimpl.h"

#include <qpe/calendar.h>
#include <qpe/datebookdb.h>
#include <qpe/event.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>

#include <qdatetime.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qspinbox.h>
#include <qstyle.h>

static const int allDayHeight = 8;

DateBookWeekView::DateBookWeekView( bool ap, bool startOnMonday,
				    QWidget *parent, const char *name )
    : QScrollView( parent, name ), ampm( ap ), bOnMonday( startOnMonday ),
      showingEvent( false )
{
    items.setAutoDelete( true );
    dayItems.setAutoDelete( true );

    viewport()->setBackgroundMode( PaletteBase );

    header = new QHeader( this );
    header->addLabel( "" );

    header->setMovingEnabled( false );
    header->setResizeEnabled( false );
    header->setClickEnabled( false, 0 );
    initNames();


    connect( header, SIGNAL(clicked(int)), this, SIGNAL(showDay(int)) );

    QObject::connect(qApp, SIGNAL(clockChanged(bool)),
	    this, SLOT(slotChangeClock(bool)));

    QFontMetrics fm( font() );
    rowHeight = fm.height()+2;

    resizeContents( width(), 24*rowHeight );
}

void DateBookWeekView::initNames()
{
    static bool bFirst = true;
    if ( bFirst ) {
	if ( bOnMonday ) {
	    header->addLabel( tr("M", "Monday" ) );
	    header->addLabel( tr("T", "Tuesday") );
	    header->addLabel( tr("W", "Wednesday" ) );
	    header->addLabel( tr("T", "Thursday" ) );
	    header->addLabel( tr("F", "Friday" ) );
	    header->addLabel( tr("S", "Saturday" ) );
	    header->addLabel( tr("S", "Sunday" ) );
	} else {
	    header->addLabel( tr("S", "Sunday" ) );
	    header->addLabel( tr("M", "Monday") );
	    header->addLabel( tr("T", "Tuesday") );
	    header->addLabel( tr("W", "Wednesday" ) );
	    header->addLabel( tr("T", "Thursday" ) );
	    header->addLabel( tr("F", "Friday" ) );
	    header->addLabel( tr("S", "Saturday" ) );
	}
	bFirst = false;
    } else {
	// we are change things...
	if ( bOnMonday ) {
	    header->setLabel( 1, tr("M", "Monday") );
	    header->setLabel( 2, tr("T", "Tuesday") );
	    header->setLabel( 3, tr("W", "Wednesday" ) );
	    header->setLabel( 4, tr("T", "Thursday" ) );
	    header->setLabel( 5, tr("F", "Friday" ) );
	    header->setLabel( 6, tr("S", "Saturday" ) );
	    header->setLabel( 7, tr("S", "Sunday" ) );
	} else {
	    header->setLabel( 1, tr("S", "Sunday" ) );
	    header->setLabel( 2, tr("M", "Monday") );
	    header->setLabel( 3, tr("T", "Tuesday") );
	    header->setLabel( 4, tr("W", "Wednesday" ) );
	    header->setLabel( 5, tr("T", "Thursday" ) );
	    header->setLabel( 6, tr("F", "Friday" ) );
	    header->setLabel( 7, tr("S", "Saturday" ) );
	}
    }
}



void DateBookWeekView::showEvents( QValueList<EffectiveEvent> &ev )
{
    items.clear();
    items.resize(7);
    for (int i = 0; i < 7; i++)
	items.insert(i, new LayoutManager(header->sectionSize(i+1), rowHeight*24));
    dayItems.clear();

    QValueListIterator<EffectiveEvent> it;
    for ( it = ev.begin(); it != ev.end(); ++it ) {
	if ((*it).event().isAllDay()) {
	    LayoutItem *i = new LayoutItem(*it);
	    positionItem(i);
	    dayItems.append(i);
	} else {
	    int dow = (*it).date().dayOfWeek();
	    if ( !bOnMonday ) {
		if ( dow == 7 )
		    dow = 1;
		else 
		    dow++;
	    }
	    items.at(dow - 1)->addOccurance( *it );
	}
    }

    int mWidth = contentsWidth();
    for (int i = 0; i < 7; i++) {
	items.at(i)->layoutItems(FALSE);
	mWidth = QMIN(mWidth, items.at(i)->maximumColumnWidth());
    }
    for (int i = 0; i < 7; i++) {
	items.at(i)->setMaximumColumnWidth(mWidth);
	items.at(i)->layoutItems(FALSE);
    }

    viewport()->update();
    update();
}

void DateBookWeekView::moveToHour( int h )
{
    int offset = h*rowHeight;
    setContentsPos( 0, offset );
}

void DateBookWeekView::keyPressEvent( QKeyEvent *e )
{
    e->ignore();
}

void DateBookWeekView::slotChangeClock( bool c )
{
    ampm = c;
    viewport()->update();
}

static inline int db_round30min( int m )
{
    if ( m < 15 )
	m = 0;
    else if ( m < 45 )
	m = 1;
    else
	m = 2;

    return m;
}

void DateBookWeekView::alterDay( int day )
{
    if ( !bOnMonday ) {
	    day--;
    }
    emit showDay( day );
}

void DateBookWeekView::positionItem( LayoutItem *i )
{
    const int Width = 8;
    const EffectiveEvent ev = i->occurance();

    // 30 minute intervals
    int y = ev.start().hour() * 2;
    y += db_round30min( ev.start().minute() );
    if ( y > 47 )
	y = 47;
    y = y * rowHeight / 2;

    int h;
    if ( ev.event().type() == Event::AllDay ) {
	h = 48;
	y = 0;
    } else {
	h = ( ev.end().hour() - ev.start().hour() ) * 2;
	h += db_round30min( ev.end().minute() - ev.start().minute() );
	if ( h < 1 ) h = 1;
    }
    h = h * rowHeight / 2;

    int dow = ev.date().dayOfWeek();
    if ( !bOnMonday ) {
	if ( dow == 7 )
	    dow = 1;
	else
	    dow++;
    }
    int x = header->sectionPos( dow );
    int hwidth = header->sectionSize( dow );
    int xlim = header->sectionPos( dow ) + hwidth;
    LayoutItem *inter = 0;
    if (ev.event().type() != Event::AllDay) {
	do {
	    i->setGeometry( x, y, Width, h );
	    inter = intersects( i );
	    x += Width;
	} while ( inter && x < xlim );
    } else {
	i->setGeometry(x, header->height(), hwidth, allDayHeight * 2);
	inter = intersects( i );
	if (inter) {
	    // need to change geom of both;
	    i->setGeometry( x, header->height() + 
		    (ev.event().hasRepeat() ? allDayHeight : 0), 
		    hwidth , allDayHeight);
	    inter->setGeometry( x, header->height() +
		    (inter->event().hasRepeat() ? allDayHeight : 0), 
		    hwidth, allDayHeight);
	}
    }
}

LayoutItem *DateBookWeekView::intersects( const LayoutItem *item )
{
    QRect geom = item->geometry();

    // We allow the edges to overlap
    geom.moveBy( 1, 1 );
    geom.setSize( geom.size()-QSize(2,2) );

    const Event itemEvent = item->event();

    QListIterator<LayoutItem> it(dayItems);
    for ( ; it.current(); ++it ) {
	LayoutItem *i = it.current();
	const Event iEvent = i->event();
	if (i != item && iEvent.hasRepeat() != itemEvent.hasRepeat()) {
	    if (i->geometry().intersects( geom ) )
		return i;
	}
    }

    return 0;
}

void DateBookWeekView::contentsMousePressEvent( QMouseEvent *e )
{
    QValueList<EffectiveEvent> list;
    for (int j = 0; j < 7; j++) {
	QVector<LayoutItem> v = items.at(j)->items();
	int x = header->sectionPos(j+1)-2;
	for ( uint k = 0; k < v.size(); k++) {
	    LayoutItem *i = v.at(k);
	    // Change to
	    QRect geo = i->geometry();
	    geo.moveBy(x,0);
	    if ( geo.contains( e->pos() ) ) {
		showingEvent = true;
		list.append(i->occurance());
	    }
	}
    }
    if (list.count())
	emit signalShowEvent( list );
}

void DateBookWeekView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    if ( showingEvent ) {
	showingEvent = false;
	emit signalHideEvent();
    } else {
	int d = header->sectionAt( e->pos().x() );
	if ( d > 0 ) {
// 	    if ( !bOnMonday )
// 		d--;
	    emit showDay( d );
	}
    }
}

void DateBookWeekView::mousePressEvent( QMouseEvent *e )
{
    QValueList<EffectiveEvent> list;
    QListIterator<LayoutItem> it(dayItems);
    for ( ; it.current(); ++it ) {
	LayoutItem *i = it.current();
	if ( i->geometry().contains( e->pos() ) ) {
	    showingEvent = true;
	    list.append(i->occurance());
	}
    }
    if (list.count())
	emit signalShowEvent( list );
}

void DateBookWeekView::mouseReleaseEvent( QMouseEvent *e )
{
    if ( showingEvent ) {
	showingEvent = false;
	emit signalHideEvent();
    } else {
	int d = header->sectionAt( e->pos().x() );
	if ( d > 0 ) {
// 	    if ( !bOnMonday )
// 		d--;
	    emit showDay( d );
	}
    }
}


void DateBookWeekView::drawFrame( QPainter *p ) 
{
    // May or may not be needed. (I don't actually think it is needed though.
    //QScrollView::drawFrame(p);

    p->fillRect(0, header->height(), header->width(), allDayHeight * 2, 
	    white);
	    //palette().color(QPalette::Normal , QColorGroup::Dark ));

    // draw day events.  
    QListIterator<LayoutItem> it(dayItems);
    for ( ; it.current(); ++it ) {
	LayoutItem *i = it.current();
	if ( i->event().isAllDay() ) {
	    p->fillRect( i->geometry(), 
		   i->event().hasRepeat() 
		   ? colorRepeatLight : colorNormalLight);
	}
    }

    QPen pen = p->pen();
    p->setPen( lightGray );

    // header->sectionPos(i)-1 because we want the end of the last header,
    // not the start of this one.
    for ( int i = 1; i <= 7; i++ )
	p->drawLine( header->sectionPos(i)-1, header->height(), 
		header->sectionPos(i)-1, header->height() + 2 * allDayHeight );

    p->drawLine( 0, header->height() + 2 * allDayHeight, 
	    header->width(), header->height() + 2 * allDayHeight );
    p->setPen(pen);
}

void DateBookWeekView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    QRect ur( cx, cy, cw, ch );
    p->setPen( lightGray );

    // -2 because end of last header, not this, and because contents are over by
    // one any way.
    for ( int i = 1; i <= 7; i++ )
	p->drawLine( header->sectionPos(i)-2, cy, header->sectionPos(i)-2, cy+ch );

    p->setPen( black );
    for ( int t = 0; t < 24; t++ ) {
	int y = t*rowHeight;
	if ( QRect( 1, y, 20, rowHeight ).intersects( ur ) ) {
	    QString s;
	    if ( ampm ) {
		if ( t == 0 )
		    s = QString::number( 12 );
		else if ( t == 12 )
		    s = QString::number(12) + tr( "p" );
		else if ( t > 12 ) {
		    if ( t - 12 < 10 )
			s = " ";
		    else
			s = "";
		    s += QString::number( t - 12 ) + tr("p");
		} else {
		    if ( 12 - t < 3 )
			s = "";
		    else
			s = " ";
		    s += QString::number( t );
		}
	    } else {
		s = QString::number( t );
		if ( s.length() == 1 )
		    s.prepend( "0" );
	    }
	    p->drawText( 1, y+p->fontMetrics().ascent()+1, s );
	}
    }

    for (int j = 0; j < 7; j++) {
	QVector<LayoutItem> v = items.at(j)->items();
	int x = header->sectionPos(j+1)-1; // 1 for end of last, 1 for contenst shift.
	for ( uint k = 0; k < v.size(); k++) {
	    LayoutItem *i = v.at(k);
	    // Change to
	    QRect geo = i->geometry();
	    geo.moveBy(x-1,-1); // up and to the left
	    geo.setSize(geo.size() + QSize(1,1));
	    if ( geo.intersects( ur ) ) {
		p->setBrush( i->event().hasRepeat() ? colorRepeat : colorNormal );
		p->drawRect( geo );
	    }
	}
    }
}

void DateBookWeekView::resizeEvent( QResizeEvent *e )
{
    const int hourWidth = 20;
    QScrollView::resizeEvent( e );
    int avail = width()-qApp->style().scrollBarExtent().width();
    header->setGeometry( 0, 0, avail, header->sizeHint().height() );
    setMargins( 0, header->height() + allDayHeight * 2, 0, 0 );
    header->resizeSection( 0, hourWidth );
    int sw = (avail - hourWidth) / 7;
    for ( int i = 1; i < 7; i++ )
	header->resizeSection( i, sw );
    header->resizeSection( 7, avail - hourWidth - sw*6 );
}

void DateBookWeekView::setStartOfWeek( bool bStartOnMonday )
{
    bOnMonday = bStartOnMonday;
    initNames();
}

//-------------------------------------------------------------------

DateBookWeek::DateBookWeek( bool ap, bool startOnMonday, DateBookDB *newDB,
			    QWidget *parent, const char *name )
    : QWidget( parent, name ),
      db( newDB ),
      startTime( 0 ),
      ampm( ap ),
      bStartOnMonday( startOnMonday )
{
    setFocusPolicy(StrongFocus);
    QVBoxLayout *vb = new QVBoxLayout( this );
    header = new DateBookWeekHeader( bStartOnMonday, this );
    view = new DateBookWeekView( ampm, startOnMonday, this );
    vb->addWidget( header );
    vb->addWidget( view );

    lblDesc = new QLabel( this, "event label" );
    lblDesc->setFrameStyle( QFrame::Plain | QFrame::Box );
    lblDesc->setBackgroundColor( yellow );
    lblDesc->hide();

    tHide = new QTimer( this );

    connect( view, SIGNAL( showDay( int ) ),
             this, SLOT( showDay( int ) ) );
    connect( view, SIGNAL(signalShowEvent(QValueList<EffectiveEvent>&)),
	     this, SLOT(slotShowEvent(QValueList<EffectiveEvent>&)) );
    connect( view, SIGNAL(signalHideEvent()),
	     this, SLOT(slotHideEvent()) );
    connect( header, SIGNAL( dateChanged( int, int ) ),
             this, SLOT( dateChanged( int, int ) ) );
    connect( tHide, SIGNAL( timeout() ),
             lblDesc, SLOT( hide() ) );
    connect( header->spinYear, SIGNAL(valueChanged(int)),
	     this, SLOT(slotYearChanged(int)) );
    connect( qApp, SIGNAL(weekChanged(bool)),
	     this, SLOT(slotWeekChanged(bool)) );
    connect( qApp, SIGNAL(clockChanged(bool)),
	    this, SLOT(slotClockChanged(bool)));
    setDate(QDate::currentDate());

}

void DateBookWeek::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()) {
	case Key_Up:
	    view->scrollBy(0, -20);
	    break;
	case Key_Down:
	    view->scrollBy(0, 20);
	    break;
	case Key_Left:
	    setDate(date().addDays(-7));
	    break;
	case Key_Right:
	    setDate(date().addDays(7));
	    break;
	default:
	    e->ignore();
    }
}

void DateBookWeek::showDay( int day )
{
    QDate d;
    d = dateFromWeek( _week, year, bStartOnMonday );
    day--;
    d = d.addDays( day );
    emit showDate( d.year(), d.month(), d.day() );
}

void DateBookWeek::setDate( int y, int m, int d )
{
    QDate date;
    date.setYMD( y, m, d );
    setDate(QDate(y, m, d));
}

void DateBookWeek::setDate(QDate date)
{
    dow = date.dayOfWeek();
    int w, y;
    calcWeek( date, w, y, bStartOnMonday );
    header->setDate( y, w );
}

void DateBookWeek::dateChanged( int y, int w )
{
    year = y;
    _week = w;
    getEvents();
}

QDate DateBookWeek::date() const
{
    QDate d;
    d = dateFromWeek( _week - 1, year, bStartOnMonday );
    if ( bStartOnMonday )
	d = d.addDays( 7 + dow - 1 );
    else {
	if ( dow == 7 )
	    d = d.addDays( dow );
	else
	    d = d.addDays( 7 + dow );
    }
    return d;
}

void DateBookWeek::getEvents()
{
    QDate startWeek = weekDate();

    QDate endWeek = startWeek.addDays( 6 );
    QValueList<EffectiveEvent> eventList = db->getEffectiveEvents(startWeek,
								  endWeek);
    view->showEvents( eventList );
    view->moveToHour( startTime );
}

void DateBookWeek::slotShowEvent( QValueList<EffectiveEvent> &events )
{
    if ( tHide->isActive() )
        tHide->stop();

    QString str = "";

    // why would someone use "<"?  Oh well, fix it up...
    // I wonder what other things may be messed up...
    QValueListIterator<EffectiveEvent> it;
    for(it = events.begin(); it != events.end(); ++it) {
	EffectiveEvent ev = (*it);
	QString strDesc = ev.description();
	int where = strDesc.find( "<" );
	while ( where != -1 ) {
	    strDesc.remove( where, 1 );
	    strDesc.insert( where, "&#60;" );
	    where = strDesc.find( "<", where );
	}

	QString strCat;
	// ### FIX later...
	//     QString strCat = ev.category();
	//     where = strCat.find( "<" );
	//     while ( where != -1 ) {
	// 	strCat.remove( where, 1 );
	// 	strCat.insert( where, "&#60;" );
	// 	where = strCat.find( "<", where );
	//     }

	QString strNote = ev.notes();
	where = strNote.find( "<" );
	while ( where != -1 ) {
	    strNote.remove( where, 1 );
	    strNote.insert( where, "&#60;" );
	    where = strNote.find( "<", where );
	}

	str += "<b>" + strDesc + "</b><br>" + "<i>"
	    + strCat + "</i>"
	    + "<br>" + TimeString::longDateString( ev.date() )
	    + "<br><b>" + QObject::tr("Start") + "</b>: ";

	if ( ev.startDate() != ev.date() ) {
	    // multi-day event.  Show start date
	    str += TimeString::longDateString( ev.startDate() );
	} else {
	    // Show start time.
	    str += TimeString::timeString(ev.start(), ampm, FALSE );
	}

	str += "<br><b>" + QObject::tr("End") + "</b>: ";
	if ( ev.endDate() != ev.date() ) {
	    // multi-day event.  Show end date
	    str += TimeString::longDateString( ev.endDate() );
	} else {
	    // Show end time.
	    str += TimeString::timeString( ev.end(), ampm, FALSE );
	}
	str += "<br><br>" + strNote;
    }

    lblDesc->setText( str );
    lblDesc->resize( lblDesc->sizeHint() );
    // move the label so it is "centerd" horizontally...
    lblDesc->move( QMAX(0,(width() - lblDesc->width()) / 2), 0 );
    lblDesc->show();
}

void DateBookWeek::slotHideEvent()
{
    tHide->start( 2000, true );
}

void DateBookWeek::setStartViewTime( int startHere )
{
    startTime = startHere;
    view->moveToHour( startTime );
}

int DateBookWeek::startViewTime() const
{
    return startTime;
}

void DateBookWeek::redraw()
{
    getEvents();
}

void DateBookWeek::slotYearChanged( int y )
{
    int totWeek;
    QDate d( y, 12, 31 );
    int throwAway;
    calcWeek( d, totWeek, throwAway, bStartOnMonday );
    while ( totWeek == 1 ) {
	d = d.addDays( -1 );
	calcWeek( d, totWeek, throwAway, bStartOnMonday );
    }
    if ( totWeek != totalWeeks() )
	setTotalWeeks( totWeek );
}


void DateBookWeek::setTotalWeeks( int numWeeks )
{
    header->spinWeek->setMaxValue( numWeeks );
}

int DateBookWeek::totalWeeks() const
{
    return header->spinWeek->maxValue();
}

void DateBookWeek::slotWeekChanged( bool onMonday )
{
    bStartOnMonday = onMonday;
    view->setStartOfWeek( bStartOnMonday );
    header->setStartOfWeek( bStartOnMonday );
    redraw();
}

void DateBookWeek::slotClockChanged( bool ap )
{
    ampm = ap;
}

// return the date at the beginning of the week...
QDate DateBookWeek::weekDate() const
{
    return dateFromWeek( _week, year, bStartOnMonday );
}

// this used to only be needed by datebook.cpp, but now we need it inside
// week view since
// we need to be able to figure out our total number of weeks on the fly...
// this is probably the best place to put it..

// For Weeks that start on Monday... (EASY!)
// At the moment we will use ISO 8601 method for computing
// the week.  Granted, other countries use other methods,
// bet we aren't doing any Locale stuff at the moment.  So,
// this should pass.  This Algorithim is public domain and
// available at:
// http://personal.ecu.edu/mccartyr/ISOwdALG.txt
// the week number is return, and the year number is returned in year
// for Instance 2001/12/31 is actually the first week in 2002.
// There is a more mathematical definition, but I will implement it when
// we are pass our deadline.

// For Weeks that start on Sunday... (ahh... home rolled)
// okay, if Jan 1 is on Friday or Saturday,
// it will go to the pervious
// week...

bool calcWeek( const QDate &d, int &week, int &year,
	       bool startOnMonday = false )
{
    int weekNumber;
    int yearNumber;

    // remove a pesky warning, (Optimizations on g++)
    weekNumber = -1;
    int jan1WeekDay = QDate(d.year(), 1, 1).dayOfWeek();
    int dayOfWeek = d.dayOfWeek();

    if ( !d.isValid() )
	return false;

    if ( startOnMonday ) {
	// find the Jan1Weekday;
	if ( d.dayOfYear() <= ( 8 - jan1WeekDay) && jan1WeekDay > 4 ) {
	    yearNumber = d.year() - 1;
	    if ( jan1WeekDay == 5 || ( jan1WeekDay == 6 && QDate::leapYear(yearNumber) ) )
		weekNumber = 53;
	    else
		weekNumber = 52;
	} else
	    yearNumber = d.year();
	if ( yearNumber == d.year() ) {
	    int totalDays = 365;
	    if ( QDate::leapYear(yearNumber) )
		totalDays++;
	    if ( ((totalDays - d.dayOfYear()) < (4 - dayOfWeek) )
		 || (jan1WeekDay == 7) && (totalDays - d.dayOfYear()) < 3) {
		yearNumber++;
		weekNumber = 1;
	    }
	}
	if ( yearNumber == d.year() ) {
	    int j = d.dayOfYear() + (7 - dayOfWeek) + ( jan1WeekDay - 1 );
	    weekNumber = j / 7;
	    if ( jan1WeekDay > 4 )
		weekNumber--;
	}
    } else {
	// it's better to keep these cases separate...
	if ( d.dayOfYear() <= (7 - jan1WeekDay) && jan1WeekDay > 4
	     && jan1WeekDay != 7 ) {
	    yearNumber = d.year() - 1;
	    if ( jan1WeekDay == 6
		 || (jan1WeekDay == 7 && QDate::leapYear(yearNumber) ) ) {
		weekNumber = 53;
	    }else
		weekNumber = 52;
	} else
	    yearNumber = d.year();
	if ( yearNumber == d.year() ) {
	    int totalDays = 365;
	    if ( QDate::leapYear( yearNumber ) )
		totalDays++;
	    if ( ((totalDays - d.dayOfYear()) < (4 - dayOfWeek % 7)) ) {
		yearNumber++;
		weekNumber = 1;
	    }
	}
	if ( yearNumber == d.year() ) {
	    int j = d.dayOfYear() + (7 - dayOfWeek % 7) + ( jan1WeekDay - 1 );
	    weekNumber = j / 7;
	    if ( jan1WeekDay > 4 ) {
		weekNumber--;
	    }
	}
    }
    year = yearNumber;
    week = weekNumber;
    return true;
}

