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
#include <qmessagebox.h>

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

#include <qtimer.h>

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
	//setRowHeight( row, 40);
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
	p->setPen( colorGroup().dark() );
	p->drawLine( x2, 0, x2, y2 );
	p->drawLine( 0, y2, x2, y2 );

	p->setPen( colorGroup().midlight() );
	p->drawLine( 0, y2 - h/2, x2, y2 - h/2);

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

void DateBookDayView::setRowStyle( int style )
{
	if (style<0) style = 0;

	for (int i=0; i<numRows(); i++)
		setRowHeight(i, style*10+20);
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

    QTimer *timer = new QTimer( this );

    connect( timer, SIGNAL(timeout()),
             this, SLOT(updateView()) );	//connect timer for updating timeMarker & daywidgetcolors
    timer->start( 1000*60*5, FALSE ); //update every 5min

    selectedWidget = 0;

    timeMarker =  new DateBookDayTimeMarker( this );
    timeMarker->setTime( QTime::currentTime() );
    rowStyle = -1; // initialize with bogus values
}

void DateBookDay::setJumpToCurTime( bool bJump )
{
	jumpToCurTime = bJump;
}

void DateBookDay::setRowStyle( int style )
{
	if (rowStyle != style) view->setRowStyle( style );
	rowStyle = style;
}

void DateBookDay::updateView( void )
{
	timeMarker->setTime( QTime::currentTime() );
	//need to find a way to update all DateBookDayWidgets
}

void DateBookDay::setSelectedWidget( DateBookDayWidget *w )
{
	selectedWidget = w;
}

DateBookDayWidget * DateBookDay::getSelectedWidget( void )
{
	return selectedWidget;
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

    selectedWidget = 0;
}

void DateBookDay::setDate( QDate d)
{
    header->setDate( d.year(), d.month(), d.day() );

    selectedWidget = 0;
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

    if (jumpToCurTime && this->date() == QDate::currentDate())
    {
    	ts.init( QTime::currentTime().hour(), 0);
	ts.expandTo( QTime::currentTime().hour(), 0);
    } else
    {
	ts.init( startTime, 0 );
	ts.expandTo( startTime, 0 );
    }

    dayView()->addSelection( ts );

    selectedWidget = 0;

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

    widgetList.sort();
    //sorts the widgetList by the heights of the widget so that the tallest widgets are at the beginning
    //this is needed for the simple algo below to work correctly, otherwise some widgets would be drawn outside the view

    int wCount = widgetList.count();
    int wid = view->columnWidth(0)-1;
    int wd;
    int n = 1;

    QArray<int> anzIntersect(wCount); //this stores the number of maximal intersections of each widget

    for (int i = 0; i<wCount; anzIntersect[i] = 1, i++);

    if ( wCount < 20 ) {

	QArray<QRect> geometries(wCount);
	for (int i = 0; i < wCount; geometries[i] = widgetList.at(i)->geometry(), i++);	//stores geometry for each widget in vector

	for ( int i = 0; i < wCount; i++)
	{
		QValueList<int> intersectedWidgets;

		//find all widgets intersecting with widgetList.at(i)
		for ( int j = 0; j < wCount; j++)
			if (i != j)
				if (geometries[j].intersects(geometries[i]))
					intersectedWidgets.append(j);

		//for each of these intersecting widgets find out how many widgets are they intersecting with
		for ( uint j = 0; j < intersectedWidgets.count(); j++)
		{
			QArray<int> inter(wCount);
			inter[j]=1;

			if (intersectedWidgets[j] != -1)
				for ( uint k = j; k < intersectedWidgets.count(); k++)
					if (j != k && intersectedWidgets[k] != -1)
						if (geometries[intersectedWidgets[k]].intersects(geometries[intersectedWidgets[j]]))
						{
							inter[j]++;
							intersectedWidgets[k] = -1;
						}
			if (inter[j] > anzIntersect[i]) anzIntersect[i] = inter[j] + 1;
		}

		if (anzIntersect[i] == 1 && intersectedWidgets.count()) anzIntersect[i]++;
	}


	for ( int i = 0; i < wCount; i++) {
	    DateBookDayWidget *w = widgetList.at(i);
	    QRect geom = w->geometry();

	    geom.setX( 0 );

	    wd = (view->columnWidth(0)-1) / anzIntersect[i] - (anzIntersect[i]>1?2:0);

	    geom.setWidth( wd );

	    while ( intersects( w, geom ) ) {
		geom.moveBy( wd + 2 + 1, 0 );
	    }
	    w->setGeometry( geom );
	}

	if (jumpToCurTime && this->date() == QDate::currentDate())
		view->setContentsPos( 0, QTime::currentTime().hour() * view->rowHeight(0) ); //set listview to current hour
	else
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
		geom.setX( xp*(wid+2) );
		geom.setWidth( wid );
		w->setGeometry( geom );
	    }
	}

	if (jumpToCurTime && this->date() == QDate::currentDate())
		view->setContentsPos( 0, QTime::currentTime().hour() * view->rowHeight(0) ); //set listview to current hour
	else
		view->setContentsPos( 0, startTime * view->rowHeight(0) );
    }

    timeMarker->setTime( QTime::currentTime() );	//display timeMarker
    timeMarker->raise();				//on top of all widgets
     if (this->date() == QDate::currentDate())		//only show timeMarker on current day
    	timeMarker->show(); else timeMarker->hide();

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

    if (jumpToCurTime && this->date() == QDate::currentDate())	//this should probably be in datebook.cpp where it's called?
    {
    	ts.init( QTime::currentTime().hour(), 0);
	ts.expandTo( QTime::currentTime().hour(), 0);
    } else
    {
	ts.init( startTime, 0 );
	ts.expandTo( startTime, 0 );
    }

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
    geom.setX( 0 );
    geom.setWidth(dateBook->dayView()->columnWidth(0)-1);

}

DateBookDayWidget::~DateBookDayWidget()
{
}

void DateBookDayWidget::paintEvent( QPaintEvent *e )
{
    QPainter p( this );

    if (dateBook->getSelectedWidget() == this)
    {
       	    p.setBrush( QColor( 155, 240, 230 ) ); // selected item
    } else
    {
    	if (dateBook->date() == QDate::currentDate())
	{
    	    QTime curTime = QTime::currentTime();

	    if (ev.end() < curTime)
	    {
	    	  p.setBrush( QColor( 180, 180, 180 ) ); // grey, inactive
	    } else
	    {
	    	//change color in dependence of the time till the event starts
 	          int duration = curTime.secsTo(ev.start());
		  if (duration < 0) duration = 0;
		  int colChange = duration*160/86400; //86400: secs per day, 160: max color shift

	    	  p.setBrush( QColor( 200-colChange, 200-colChange, 255 ) ); //blue
	    }
	 } else
	 {
                  p.setBrush( QColor( 220, 220, 220 ) ); //light grey, inactive (not current date)
		  //perhaps make a distinction between future/past dates
	 }
    }

    p.setPen( QColor(100, 100, 100) );
    p.drawRect(rect());

 //   p.drawRect(0,0, 5, height());

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
    rt.draw( &p, 7, 0, e->region(), colorGroup() );
}

void DateBookDayWidget::mousePressEvent( QMouseEvent *e )
{
	DateBookDayWidget *item;

	item = dateBook->getSelectedWidget();
	if (item) item->update();

	dateBook->setSelectedWidget(this);
	update();
	dateBook->repaint();

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


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------


DateBookDayTimeMarker::DateBookDayTimeMarker( DateBookDay *db )
    : QWidget( db->dayView()->viewport() ), dateBook( db )
{
	setBackgroundMode( PaletteBase );
}

DateBookDayTimeMarker::~DateBookDayTimeMarker()
{
}

void DateBookDayTimeMarker::paintEvent( QPaintEvent */*e*/ )
{
	QPainter p( this );
	p.setBrush( QColor( 255, 0, 0 ) );

	QPen pen;
	pen.setStyle(NoPen);

	p.setPen( pen );
	p.drawRect(rect());
}

void DateBookDayTimeMarker::setTime( const QTime &t )
{
	int y = t.hour()*60+t.minute();
	int rh = dateBook->dayView()->rowHeight(0);
	y = y*rh/60;

	geom.setX( 0 );

	int x = dateBook->dayView()->columnWidth(0)-1;
	geom.setWidth( x );

	geom.setY( y );
	geom.setHeight( 1 );

	setGeometry( geom );

	time = t;
}

void DateBookDayTimeMarker::setGeometry( const QRect &r )
{
    geom = r;
    setFixedSize( r.width()+1, r.height()+1 );
    dateBook->dayView()->moveChild( this, r.x(), r.y()-1 );
    show();
}
