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

#include "datebookday.h"
#include "datebookdayheaderimpl.h"
#include "layoutmanager.h"

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
#include <qabstractlayout.h>

static const int time_height = 20;
static const int time_width = 45;
static const int day_height = 15;

class DayItem : public LayoutItem
{
public:
    DayItem(const EffectiveEvent &ev, bool ampm = FALSE) : LayoutItem(ev) {

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

	headingText = "<b>" + strDesc + "</b><br>";
	noteText = "<b>" + QObject::tr("Start") + "</b>: ";


	if ( ev.startDate() != ev.date() ) {
	    // multi-day event.  Show start date
	    noteText += TimeString::longDateString( ev.startDate() );
	} else {
	    // Show start time.
	    noteText += TimeString::timeString( ev.start(), ampm, FALSE );
	}

	noteText += "<br><b>" + QObject::tr("End") + "</b>: ";
	if ( ev.endDate() != ev.date() ) {
	    // multi-day event.  Show end date
	    noteText += TimeString::longDateString( ev.endDate() );
	} else {
	    // Show end time.
	    noteText += TimeString::timeString( ev.end(), ampm, FALSE );
	}
	noteText += "<br><br>" + strNote;
	//setBackgroundMode( PaletteBase );

    }
    ~DayItem() { }


    void drawItem( QPainter *p, int x, int y, const QColorGroup &cg )
    {
	p->save();
	x += geometry().x() - 1;
	y += geometry().y() - 1;
	int w = geometry().width() + 1;
	int h = geometry().height() + 1;
	p->setPen( QColor(100, 100, 100) );
	if (event().hasRepeat())
	    p->setBrush( colorRepeatLight ); // based on type.
	else 
	    p->setBrush( colorNormalLight ); // based on type.

	p->drawRect( x, y, w, h );

	int d = 0;

	if ( event().hasAlarm() ) {
	    p->drawPixmap( x + w - 16, y, 
		    Resource::loadPixmap( "bell" ) );
	    d += 20;
	}

	if ( event().hasRepeat() ) {
	    p->drawPixmap( x + w - 16 - d, y, 
		    Resource::loadPixmap( "repeat" ) );
	    d += 20;
	}

	QSimpleRichText rt( headingText, p->font() );
	QSimpleRichText rtNotes( noteText, p->font() );


	// modified clip region for rt.
	// better to clip than to draw over our icons.
	QRect rect(x + 3, y, w - d - 6, h);
	rt.setWidth( w - d - 6 );
	rtNotes.setWidth( w - 6 );

	p->setClipRect(p->xForm(QRect(x,y,w,h)));
	rtNotes.draw( p, x + 3, y + rt.height(), 
		QRegion(x, y, w, h), cg );
	p->setClipRect(p->xForm(rect));
	rt.draw( p, x + 3, y,  QRegion(rect), cg );
	p->restore();
    }

private:
    QString headingText;
    QString noteText;
};

class DayViewLayout : public LayoutManager
{
public:
    DayViewLayout(int w, int h) : LayoutManager(w,h) { }

    void addOccurance(EffectiveEvent &e, bool ampm) 
    {
	DayItem *di = new DayItem(e, ampm);
	initializeGeometry(di);
	addItem(di);
    }
};

class AllDayLayout : public DayViewLayout
{
public:
    AllDayLayout(int w, int h) : DayViewLayout(w,h) { }

    void layoutItems(bool rsmaxsize = FALSE)
    {
	for ( int i = 0; i < count(); i++) {
	    LayoutItem *w = items().at(i);
	    // one under the other, full width.
	    QRect geom = w->geometry();
	    geom.setX( i % 2 ? size().width() / 2 : 0 );
	    geom.setY( day_height * (i / 2) );
	    geom.setWidth( i % 2 == 0 && i == count() - 1 ? 
		    size().width() :
		    size().width() / 2 );
	    geom.setHeight( day_height );
	    w->setGeometry(geom);
	}
    }
};

/*
   // Preserved since we may need to take this algorithm later
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
*/

DateBookDayView::DateBookDayView( Type t, bool whichClock,
	QWidget *parent, const char *name )
    : QScrollView( parent, name ),
      ampm( whichClock ), typ(t), start_of_day(8)
{
    setResizePolicy(Manual);
    startSel = endSel = -1;
    dragging = FALSE;

    setHScrollBarMode(AlwaysOff);
    enableClipper(TRUE); // makes scrolling with child widgets faster.

    if (typ == AllDay) {
	setMargins(0, 3, 0, 0);
	setBackgroundMode(PaletteDark);
	itemList = new AllDayLayout(contentsWidth(), contentsHeight());
    } else {
	itemList = new DayViewLayout(contentsWidth() - time_width, contentsHeight());
    }

    QObject::connect( qApp, SIGNAL(clockChanged(bool)),
                      this, SLOT(slotChangeClock(bool)) );
}

void DateBookDayView::slotChangeClock( bool newClock )
{
    ampm = newClock;
}

bool DateBookDayView::whichClock() const
{
    return ampm;
}

void DateBookDayView::addOccurance(EffectiveEvent &ev)
{
    itemList->addOccurance(ev, ampm);

    if (typ == AllDay) {
	resizeContents(width(), day_height * ((itemList->count() + 1) / 2 ));
	itemList->setSize(contentsWidth() - (typ == AllDay ? 0 : time_width), contentsHeight());
    }
}

void DateBookDayView::clearItems()
{
    itemList->clear();
}

void DateBookDayView::layoutItems()
{
    itemList->layoutItems(TRUE);
    repaintContents(0, 0, contentsWidth(), contentsHeight());
}

void DateBookDayView::startAtTime(int t)
{
    start_of_day = t;
    if (typ != AllDay) 
	setContentsPos(0, posOfHour(t));
}

void DateBookDayView::moveUp()
{
    scrollBy(0, -20);
}

void DateBookDayView::moveDown()
{
    scrollBy(0, 20);
}

void DateBookDayView::resizeEvent( QResizeEvent *e )
{
    // resize children to fit if that is our type.
    switch(typ) {
	case ScrollingDay:
	    resizeContents(
		    e->size().width() - style().scrollBarExtent().width(), 
		    posOfHour(24));
	    setContentsPos(0, posOfHour(start_of_day));
	    break;
	case CompressedDay:
	    break;
	case AllDay:
	    resizeContents(e->size().width(), ((itemList->count() + 1) / 2 ) * day_height);
	    break;
    }
    itemList->setSize(contentsWidth() - (typ == AllDay ? 0 : time_width), contentsHeight());
    itemList->layoutItems();
}

void DateBookDayView::contentsMousePressEvent( QMouseEvent *e )
{
    QVector<LayoutItem> list = itemList->items();
    dragging = TRUE;

    for (uint i = 0; i < list.size(); i++) {
	LayoutItem *itm = list.at(i);
	QRect geo = itm->geometry();
	if (typ != AllDay)
	    geo.moveBy(time_width, 0);
	if (geo.contains( e->pos() ) ) {
	    Event ev = itm->event();
	    QPopupMenu m;
	    m.insertItem( tr( "Edit" ), 1 );
	    m.insertItem( tr( "Delete" ), 2 );
	    m.insertItem( tr( "Beam" ), 3 );
	    int r = m.exec( e->globalPos() );
	    if ( r == 1 ) {
		emit editEvent( ev );
	    } else if ( r == 2 ) {
		emit removeEvent( ev );
	    } else if ( r == 3 ) {
		emit beamEvent( ev );
	    }
	    dragging = FALSE;
	    break;
	}
    }

    if (dragging) {
	int h = hourAtPos(e->pos().y());
	int oldSelMin = QMIN(startSel, endSel);
	int oldSelMax = QMAX(startSel, endSel);
	startSel = endSel = h;

	oldSelMin = oldSelMin == -1 ? startSel : QMIN(oldSelMin, startSel);
	oldSelMax = oldSelMax == -1 ? startSel : QMAX(oldSelMax, startSel);

	repaintContents(0, posOfHour(oldSelMin - 1), 
		time_width, posOfHour(oldSelMax + 1), FALSE);
	dragging = TRUE;
    }
}

void DateBookDayView::contentsMouseMoveEvent( QMouseEvent *e )
{
    if (dragging) {
	int h = hourAtPos(e->pos().y());
	int oldSelMin = QMIN(startSel, endSel);
	int oldSelMax = QMAX(startSel, endSel);
	endSel = h;

	oldSelMin = QMIN(oldSelMin, endSel);
	oldSelMax = QMAX(oldSelMax, endSel);
	repaintContents(0, posOfHour(oldSelMin - 1), 
		time_width, posOfHour(oldSelMax + 1), FALSE);
    }
}

void DateBookDayView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    if (dragging) {
	int h = hourAtPos(e->pos().y());
	int oldSelMin = QMIN(startSel, endSel);
	int oldSelMax = QMAX(startSel, endSel);
	endSel = h;

	oldSelMin = QMIN(oldSelMin, endSel);
	oldSelMax = QMAX(oldSelMax, endSel);
	repaintContents(0, posOfHour(oldSelMin - 1), 
		time_width, posOfHour(oldSelMax + 1), FALSE);
	// XXX should signal new event.
	emit sigCapturedKey(QString::null);
    }
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

void DateBookDayView::drawContents( QPainter *p, int x, int y, int w, int h)
{
    //p->fillRect(time_width, x+w-time_width, y, h, palette().active().color(QColorGroup::Base));
    p->fillRect(time_width, 0, 
	    contentsWidth() - time_width, contentsHeight(), white);

    int firstSel = QMIN(startSel, endSel);
    int lastSel = QMAX(startSel, endSel);

    // draw each hour.
    QFont f = p->font();
    QFontMetrics fm(f);
    int base = QMIN(time_height - fm.height(), 2);
    base = time_height - (base / 2) - (fm.height() - fm.ascent());
    base -= 2;

    QColorGroup cgUp = palette().active();
    cgUp.setColor(QColorGroup::Button, 
	    palette().active().color(QColorGroup::Midlight));
    QFont selectFont = f;
    selectFont.setBold(TRUE);

    // also of possible use, QColorGroup::BrightText.

    switch (typ) {
	case ScrollingDay:
	    {
		int rx = 0;
		int ry = 0;
		int rh = 0;
		int i = 0;
		while (i < 24) {
		    QString strTmp;
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
		    strTmp += " ";
		    ry = posOfHour(i);
		    i++;
		    rh = posOfHour(i) - ry;

		    if (ry > y + h)
			break;
		    if (ry + rh < y)
			continue;

		    // draw this rect.
		    QPen pn = p->pen();
		    //p->setPen(black);
		    p->drawLine( time_width, ry - 1, width(), ry - 1);
		    int offset = fm.width(strTmp);
		    if (ampm &&  i <= 12)
			offset += fm.width("p");
		    // the shift is because i is i++ at this point.
		    style().drawButton(p, rx, ry, time_width, time_height, 
			    cgUp, i > firstSel && i <= lastSel + 1 );
		    if ( i > firstSel && i <= lastSel + 1 )
			p->setFont(selectFont);
		    p->drawText( rx + (time_width - offset), ry + base, strTmp );
		    p->setFont(f);
		    p->setPen(pn);
		}
	    }
	    break;
	case CompressedDay:
	    break;
	case AllDay:
	    break;
    }
    // and now the items.

    for (int k = 0; k < itemList->count(); k++) {
	DayItem *di = (DayItem*)itemList->items().at(k);

	di->drawItem(p, typ == AllDay ? 0 : time_width, 0, palette().active());
    }
}

int DateBookDayView::posOfHour(int h) const {
    switch (typ) {
	case ScrollingDay:
	    // all hours, regardless of height.
	    return h*time_height;
	case CompressedDay:
	    // all hours, in height.
	    break;
	default:
	    return 0;
    }
    return 0;
}
int DateBookDayView::hourAtPos(int h) const {
    switch (typ) {
	case ScrollingDay:
	    // all hours, regardless of height.
	    return h == 0 ? 0 : h/time_height;
	case CompressedDay:
	    // all hours, in height.
	    break;
	default:
	    return 0;
    }
    return 0;
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
    header = new DateBookDayHeader( startOnMonday, this, "day header" );
    header->setDate( currDate.year(), currDate.month(), currDate.day() );
    allView = new DateBookDayView( DateBookDayView::AllDay, ampm, 
	    this, "all day view" );
    allView->setFocusPolicy(NoFocus);
    view = new DateBookDayView( DateBookDayView::ScrollingDay, ampm, 
	    this, "day view" );
    view->setSizePolicy(
	    QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    connect( header, SIGNAL( dateChanged( int, int, int ) ),
             this, SLOT( dateChanged( int, int, int ) ) );
    connect( view, SIGNAL( sigColWidthChanged() ),
             this, SLOT( slotColWidthChanged() ) );
    connect( qApp, SIGNAL(weekChanged(bool)),
	     this, SLOT(slotWeekChanged(bool)) );
    connect( view, SIGNAL(sigCapturedKey(const QString &)),
	     this, SIGNAL(sigNewEvent(const QString&)) );

    connect( view, SIGNAL(editEvent(const Event &)),
	     this, SIGNAL(editEvent(const Event&)) );
    connect( view, SIGNAL(removeEvent(const Event &)),
	     this, SIGNAL(removeEvent(const Event&)) );
    connect( view, SIGNAL(beamEvent(const Event &)),
	     this, SIGNAL(beamEvent(const Event&)) );

    connect( allView, SIGNAL(editEvent(const Event &)),
	     this, SIGNAL(editEvent(const Event&)) );
    connect( allView, SIGNAL(removeEvent(const Event &)),
	     this, SIGNAL(removeEvent(const Event&)) );
    connect( allView, SIGNAL(beamEvent(const Event &)),
	     this, SIGNAL(beamEvent(const Event&)) );
}

void DateBookDay::selectedDates( QDateTime &start, QDateTime &end )
{
    start.setDate( currDate );
    end.setDate( currDate );

    int sh = dayView()->startHour();
    int eh = dayView()->endHour();

    if (sh >= 0) { 
	start.setTime( QTime( sh, 0, 0 ) );
	end.setTime( QTime( eh, 0, 0 ) );
    }
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
}

void DateBookDay::redraw()
{
    if ( isUpdatesEnabled() )
	relayoutPage();
}

void DateBookDay::getEvents()
{
    view->clearItems();
    allView->clearItems();

    QValueList<EffectiveEvent> eventList = db->getEffectiveEvents( currDate,
								   currDate );
    int allDayCount = 0;
    QValueListIterator<EffectiveEvent> it;
    for ( it = eventList.begin(); it != eventList.end(); ++it ) {
	if ((*it).event().type() == Event::AllDay ) {
	    allView->addOccurance(*it);
	    allDayCount++;
	} else { 
	    view->addOccurance(*it);
	}
    }
    view->layoutItems();
    if (allDayCount) {
	allDayCount++;
	allView->setMaximumHeight(QMIN(40, day_height * (allDayCount / 2 ) + 5));
	allView->setMinimumHeight(QMIN(40, day_height * (allDayCount / 2 ) + 5));
	allView->layoutItems();
	allView->show();
    } else {
	allView->hide();
    }
}


void DateBookDay::relayoutPage( bool fromResize )
{
    if ( !fromResize )
        getEvents();    // no need we already have them!
    view->layoutItems();
    allView->layoutItems();

    return;
}

QDate DateBookDay::date() const
{
    return currDate;
}

void DateBookDay::setStartViewTime( int startHere )
{
    startTime = startHere;
    view->startAtTime(startHere);
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
