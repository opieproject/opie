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

#include "datebookday.h"
#include "datebookdayheaderimpl.h"

#include <qpe/datebookdb.h>
#include <qpe/resource.h>
#include <qpe/event.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/qpedebug.h>

#include <qheader.h>
#include <qdatetime.h>
#include <qpainter.h>
#include <qsimplerichtext.h>
#include <qpopupmenu.h>
#include <qtextcodec.h>
#include <qpalette.h>

DateBookDayView::DateBookDayView( bool whichClock, QWidget *parent,
                                  const char *name )
    : QTable( 24, 1, parent, name ),
      ampm( whichClock )
{
    enableClipper(TRUE);
    setTopMargin( 0 );
    horizontalHeader()->hide();
    setLeftMargin(38);
    setColumnStretchable( 0, TRUE );
    setHScrollBarMode( QScrollView::AlwaysOff );
    verticalHeader()->setPalette(white);
    verticalHeader()->setResizeEnabled(FALSE);
    setSelectionMode( Single );

    // get rid of being able to edit things...
    QTableItem *tmp;
    int row;
    for ( row = 0; row < numRows(); row++ ) {
	tmp = new QTableItem( this, QTableItem::Never, QString::null);
	setItem( row, 0, tmp );
    }
    initHeader();
    QObject::connect( qApp, SIGNAL(clockChanged(bool)),
                      this, SLOT(slotChangeClock(bool)) );
}

void DateBookDayView::initHeader()
{
    QString strTmp;
    for ( int i = 0; i < 24; ++i ) {
	if ( ampm ) {
	    if ( i == 0 )
		strTmp = QString::number(12) + ":00";
	    else if ( i == 12 )
		strTmp = QString::number(12) + tr(":00p");
	    else if ( i > 12 )
		strTmp = QString::number( i - 12 ) + tr(":00p");
	    else
		strTmp = QString::number(i) + ":00";
	} else {
	    if ( i < 10 )
		strTmp = "0" + QString::number(i) + ":00";
	    else
		strTmp = QString::number(i) + ":00";
	}
	strTmp = strTmp.rightJustify( 6, ' ' );
	verticalHeader()->setLabel( i, strTmp );
        setRowStretchable( i, FALSE );
    }
}

void DateBookDayView::slotChangeClock( bool newClock )
{
    ampm = newClock;
    initHeader();
}

bool DateBookDayView::whichClock() const
{
    return ampm;
}

void DateBookDayView::moveUp()
{
    scrollBy(0, -20);
}

void DateBookDayView::moveDown()
{
    scrollBy(0, 20);
}

void DateBookDayView::paintCell( QPainter *p, int, int, const QRect &cr, bool )
{
    int w = cr.width();
    int h = cr.height();
    p->fillRect( 0, 0, w, h, colorGroup().brush( QColorGroup::Base ) );
    if ( showGrid() ) {
	// Draw our lines
	int x2 = w - 1;
	int y2 = h - 1;
	QPen pen( p->pen() );
	p->setPen( colorGroup().mid() );
	p->drawLine( x2, 0, x2, y2 );
	p->drawLine( 0, y2, x2, y2 );
	p->setPen( pen );
    }
}

void DateBookDayView::paintFocus( QPainter *, const QRect & )
{
}


void DateBookDayView::resizeEvent( QResizeEvent *e )
{
    QTable::resizeEvent( e );
    columnWidthChanged( 0 );
    emit sigColWidthChanged();
}

void DateBookDayView::keyPressEvent( QKeyEvent *e )
{
    QString txt = e->text();
    if ( !txt.isNull() && txt[0] > ' ' && e->key() < 0x1000 ) {
	// we this is some sort of thing we know about...
	e->accept();
	emit sigCapturedKey( txt );
    } else {
	// I don't know what this key is, do you?
	e->ignore();
    }
}


//===========================================================================

DateBookDay::DateBookDay( bool ampm, bool startOnMonday,
			  DateBookDB *newDb, QWidget *parent,
                          const char *name )
    : QVBox( parent, name ),
      currDate( QDate::currentDate() ),
      db( newDb ),
      startTime( 0 )
{
    widgetList.setAutoDelete( true );
    header = new DateBookDayHeader( startOnMonday, this, "day header" );
    header->setDate( currDate.year(), currDate.month(), currDate.day() );
    view = new DateBookDayView( ampm, this, "day view" );
    connect( header, SIGNAL( dateChanged( int, int, int ) ),
             this, SLOT( dateChanged( int, int, int ) ) );
    connect( view, SIGNAL( sigColWidthChanged() ),
             this, SLOT( slotColWidthChanged() ) );
    connect( qApp, SIGNAL(weekChanged(bool)),
	     this, SLOT(slotWeekChanged(bool)) );
    connect( view, SIGNAL(sigCapturedKey(const QString &)),
	     this, SIGNAL(sigNewEvent(const QString&)) );
}

void DateBookDay::selectedDates( QDateTime &start, QDateTime &end )
{
    start.setDate( currDate );
    end.setDate( currDate );

    int sh=99,eh=-1;

    int n = dayView()->numSelections();

    for (int i=0; i<n; i++) {
	QTableSelection sel = dayView()->selection( i );
	sh = QMIN(sh,sel.topRow());
	eh = QMAX(sh,sel.bottomRow()+1);
    }
    if (sh > 23 || eh < 1) {
        sh=8;
        eh=9;
    }

    start.setTime( QTime( sh, 0, 0 ) );
    end.setTime( QTime( eh, 0, 0 ) );
}

void DateBookDay::setDate( int y, int m, int d )
{
    header->setDate( y, m, d );
}

void DateBookDay::setDate( QDate d)
{
    header->setDate( d.year(), d.month(), d.day() );
}

void DateBookDay::dateChanged( int y, int m, int d )
{
    QDate date( y, m, d );
    if ( currDate == date )
	return;
    currDate.setYMD( y, m, d );
    relayoutPage();
    dayView()->clearSelection();
    QTableSelection ts;
    ts.init( startTime, 0 );
    ts.expandTo( startTime, 0 );
    dayView()->addSelection( ts );
}

void DateBookDay::redraw()
{
    if ( isUpdatesEnabled() )
	relayoutPage();
}

void DateBookDay::getEvents()
{
    widgetList.clear();

    QValueList<EffectiveEvent> eventList = db->getEffectiveEvents( currDate,
								   currDate );
    QValueListIterator<EffectiveEvent> it;
    for ( it = eventList.begin(); it != eventList.end(); ++it ) {
        DateBookDayWidget* w = new DateBookDayWidget( *it, this );
        connect( w, SIGNAL( deleteMe( const Event & ) ),
                 this, SIGNAL( removeEvent( const Event & ) ) );
        connect( w, SIGNAL( editMe( const Event & ) ),
                 this, SIGNAL( editEvent( const Event & ) ) );
        connect( w, SIGNAL( beamMe( const Event & ) ),
                 this, SIGNAL( beamEvent( const Event & ) ) );
        widgetList.append( w );
    }
}

static int place( const DateBookDayWidget *item, bool *used, int maxn )
{
    int place = 0;
    int start = item->event().start().hour();
    QTime e = item->event().end();
    int end = e.hour();
    if ( e.minute() < 5 )
	end--;
    if ( end < start )
	end = start;
    while ( place < maxn ) {
	bool free = TRUE;
	int s = start;
	while( s <= end ) {
	    if ( used[10*s+place] ) {
		free = FALSE;
		break;
	    }
	    s++;
	}
	if ( free ) break;
	place++;
    }
    if ( place == maxn ) {
	return -1;
    }
    while( start <= end ) {
	used[10*start+place] = TRUE;
	start++;
    }
    return place;
}


void DateBookDay::relayoutPage( bool fromResize )
{
    setUpdatesEnabled( FALSE );
    if ( !fromResize )
        getEvents();    // no need we already have them!

    int wCount = widgetList.count();
    int wid = view->columnWidth(0)-1;
    int n = 1;

    if ( wCount < 20 ) {
	for ( int i = 0; i < wCount; ) {
	    DateBookDayWidget *w = widgetList.at(i);
	    int x = 0;
	    int xp = 0;
	    QRect geom = w->geometry();
	    geom.setX( x );
	    geom.setWidth( wid );
	    while ( xp < n && intersects( w, geom ) ) {
		x += wid;
		xp++;
		geom.moveBy( wid, 0 );
	    }
	    if ( xp >= n ) {
		n++;
		wid = ( view->columnWidth(0)-1 ) / n;
		i = 0;
	    } else {
		w->setGeometry( geom );
		i++;
	    }
	}
	view->setContentsPos( 0, startTime * view->rowHeight(0) );
    } else {
    
    
	int hours[24];
	memset( hours, 0, 24*sizeof( int ) );
	bool overFlow = FALSE;
	for ( int i = 0; i < wCount; i++ ) {
	    DateBookDayWidget *w = widgetList.at(i);
	    int start = w->event().start().hour();
	    QTime e = w->event().end();
	    int end = e.hour();
	    if ( e.minute() < 5 )
		end--;
	    if ( end < start )
		end = start;
	    while( start <= end ) {
		hours[start]++;
		if ( hours[start] >= 10 )
		    overFlow = TRUE;
		++start;
	    }
	    if ( overFlow )
		break;
	}
	for ( int i = 0; i < 24; i++ ) {
	    n = QMAX( n, hours[i] );
	}
	wid = ( view->columnWidth(0)-1 ) / n;
    
	bool used[24*10];
	memset( used, FALSE, 24*10*sizeof( bool ) );
    
	for ( int i = 0; i < wCount; i++ ) {
	    DateBookDayWidget *w = widgetList.at(i);
	    int xp = place( w, used, n );
	    if ( xp != -1 ) {
		QRect geom = w->geometry();
		geom.setX( xp*wid );
		geom.setWidth( wid );
		w->setGeometry( geom );
	    }
	}
	view->setContentsPos( 0, startTime * view->rowHeight(0) );
    }
    setUpdatesEnabled( TRUE );
    return;
}

DateBookDayWidget *DateBookDay::intersects( const DateBookDayWidget *item, const QRect &geom )
{
    int i = 0;
    DateBookDayWidget *w = widgetList.at(i);
    int wCount = widgetList.count();
    while ( i < wCount && w != item ) {
	if ( w->geometry().intersects( geom ) ) {
	    return w;
	}
	w = widgetList.at(++i);
    }

    return 0;
}


QDate DateBookDay::date() const
{
    return currDate;
}

void DateBookDay::setStartViewTime( int startHere )
{
    startTime = startHere;
    dayView()->clearSelection();
    QTableSelection ts;
    ts.init( startTime, 0 );
    ts.expandTo( startTime, 0 );
    dayView()->addSelection( ts );
}

int DateBookDay::startViewTime() const
{
    return startTime;
}

void DateBookDay::slotWeekChanged( bool bStartOnMonday )
{
    header->setStartOfWeek( bStartOnMonday );
    //    redraw();
}

void DateBookDay::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()) {
	case Key_Up:
	    view->moveUp();
	    break;
	case Key_Down:
	    view->moveDown();
	    break;
	case Key_Left:
	    setDate(QDate(currDate).addDays(-1));
	    break;
	case Key_Right:
	    setDate(QDate(currDate).addDays(1));
	    break;
	default:
	    e->ignore();
    }
}

//===========================================================================

DateBookDayWidget::DateBookDayWidget( const EffectiveEvent &e,
				      DateBookDay *db )
    : QWidget( db->dayView()->viewport() ), ev( e ), dateBook( db )
{
    bool whichClock = db->dayView()->whichClock();

    // why would someone use "<"?  Oh well, fix it up...
    // I wonder what other things may be messed up...
    QString strDesc = ev.description();
    int where = strDesc.find( "<" );
    while ( where != -1 ) {
	strDesc.remove( where, 1 );
	strDesc.insert( where, "&#60;" );
	where = strDesc.find( "<", where );
    }

    QString strCat;
    // ### Fix later...
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

    text = "<b>" + strDesc + "</b><br>" + "<i>"
        + strCat + "</i>"
	+ "<br><b>" + tr("Start") + "</b>: ";


    if ( e.startDate() != ev.date() ) {
	// multi-day event.  Show start date
	text += TimeString::longDateString( e.startDate() );
    } else {
	// Show start time.
	text += TimeString::timeString( ev.start(), whichClock, FALSE );
    }

    text += "<br><b>" + tr("End") + "</b>: ";
    if ( e.endDate() != ev.date() ) {
	// multi-day event.  Show end date
	text += TimeString::longDateString( e.endDate() );
    } else {
	// Show end time.
	text += TimeString::timeString( ev.end(), whichClock, FALSE );
    }
    text += "<br><br>" + strNote;
    setBackgroundMode( PaletteBase );

    QTime start = ev.start();
    QTime end = ev.end();
    int y = start.hour()*60+start.minute();
    int h = end.hour()*60+end.minute()-y;
    int rh = dateBook->dayView()->rowHeight(0);
    y = y*rh/60;
    h = h*rh/60;
    if ( h < 3 )
        h = 3;
    geom.setY( y );
    geom.setHeight( h );
}

DateBookDayWidget::~DateBookDayWidget()
{
}

void DateBookDayWidget::paintEvent( QPaintEvent *e )
{
    QPainter p( this );
    p.setPen( QColor(100, 100, 100) );
    p.setBrush( QColor( 255, 240, 230 ) ); // based on priority?
    p.drawRect(rect());

    int y = 0;
    int d = 0;

    if ( ev.event().hasAlarm() ) {
        p.drawPixmap( width() - 16, 0, Resource::loadPixmap( "bell" ) );
        y = 20;
        d = 20;
    }

    if ( ev.event().hasRepeat() ) {
        p.drawPixmap( width() - 16, y, Resource::loadPixmap( "repeat" ) );
        d = 20;
    }

    QSimpleRichText rt( text, font() );
    rt.setWidth( geom.width() - d - 6 );
    rt.draw( &p, 3, 0, e->region(), colorGroup() );
}

void DateBookDayWidget::mousePressEvent( QMouseEvent *e )
{
    QPopupMenu m;
    m.insertItem( tr( "Edit" ), 1 );
    m.insertItem( tr( "Delete" ), 2 );
    m.insertItem( tr( "Beam" ), 3 );
    int r = m.exec( e->globalPos() );
    if ( r == 1 ) {
        emit editMe( ev.event() );
    } else if ( r == 2 ) {
        emit deleteMe( ev.event() );
    } else if ( r == 3 ) {
	emit beamMe( ev.event() );
    }
}

void DateBookDayWidget::setGeometry( const QRect &r )
{
    geom = r;
    setFixedSize( r.width()+1, r.height()+1 );
    dateBook->dayView()->moveChild( this, r.x(), r.y()-1 );
    show();
}
