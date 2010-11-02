/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
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

#include "datebookweek.h"
#include "datebookweekheaderimpl.h"

#include <qpe/qpeapplication.h>
#include <qpe/calendar.h>

#include <opie2/odebug.h>

#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>

using namespace Opie;

//-----------------------------------------------------------------

DatebookWeekItemPreview::DatebookWeekItemPreview( QWidget * parent, const char * name, WFlags f )
    : QLabel( parent, name, f )
{
    setFrameStyle( QFrame::Plain | QFrame::Box );
    setBackgroundColor( QColor( 255, 255, 120 ) );
}

void DatebookWeekItemPreview::mousePressEvent( QMouseEvent * )
{
    hide();
}

//-----------------------------------------------------------------

DateBookWeekItem::DateBookWeekItem( const OPimOccurrence e, Opie::Datebook::WeekView *weekView )
    : ev( e )
{
    // with the current implementation change the color for all day events
    OPimEvent event = e.toEvent();
    c = weekView->eventVisualiser()->eventColour( ev );
}

void DateBookWeekItem::setGeometry( int x, int y, int w, int h )
{
    r.setRect( x, y, w, h );
}


//------------------=---------------------------------------------

DateBookWeekView::DateBookWeekView( Opie::Datebook::WeekView *weekView,
                    bool ap, bool startOnMonday,
                    QWidget *parent, const char *name )
    : QScrollView( parent, name ), m_weekView( weekView ), ampm( ap ), bOnMonday( startOnMonday ),
      showingEvent( false )
{
    items.setAutoDelete( true );

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
            for ( int i = 1; i<=7; i++  )  {
                header->addLabel( Calendar::nameOfDay( i ) );
            }
        }
        else {
            header->addLabel( Calendar::nameOfDay( 7 ) );
            for ( int i = 1; i<7; i++  )  {
                header->addLabel( Calendar::nameOfDay( i ) );
            }
        }
        bFirst = false;
    }
    else {
        // we are change things...
        if ( bOnMonday ) {
            for ( int i = 1; i<=7; i++  )  {
                header->setLabel( i, Calendar::nameOfDay( i ) );
            }
        }
        else {
            header->setLabel( 1, Calendar::nameOfDay( 7 ) );
            for ( int i = 1; i<7; i++  )  {
                header->setLabel( i+1, Calendar::nameOfDay( i ) );
            }
        }
    }
}

void DateBookWeekView::showEvents( QValueList<OPimOccurrence> &ev )
{
    items.clear();
    QValueListIterator<OPimOccurrence> it;
    for ( it = ev.begin(); it != ev.end(); ++it ) {
        DateBookWeekItem *i = new DateBookWeekItem( *it, m_weekView );
// FIXME when this is reinstated it needs to stop leaking DateBookWeekItems also
//X        if(!((i->event().end().hour()==0) && (i->event().end().minute()==0) && (i->event().startDate()!=i->event().date()))) {  // Skip events ending at 00:00 starting at another day.
            positionItem( i );
            items.append( i );
//X        }
    }
    viewport()->update();
}

void DateBookWeekView::moveToHour( int h )
{
    int offset = h*rowHeight;
    setContentsPos( 0, offset );
}

void DateBookWeekView::keyPressEvent( QKeyEvent *e )
{
    odebug << "key!!!" << oendl;
    switch(e->key()) {
    case Key_Up:
        scrollBy(0, -20);
        break;
    case Key_Down:
        scrollBy(0, 20);
        break;
    case Key_Left:
        m_date = m_date.addDays(-7);
        emit signalDateChanged( m_date );
        break;
    case Key_Right:
        m_date = m_date.addDays(7);
        emit signalDateChanged( m_date );
        break;
    default:
        e->ignore();
    }
}

void DateBookWeekView::slotDateChanged( QDate &date )
{
    m_date = date;
}

void DateBookWeekView::slotChangeClock( bool c )
{
    ampm = c;
    resizeEvent(NULL); // force resize to relayout columns
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

void DateBookWeekView::positionItem( DateBookWeekItem *i )
{
    const OPimOccurrence ev = i->event();

    // 30 minute intervals
    int y = ev.startTime().hour() * 2;
    y += db_round30min( ev.startTime().minute() );
    int y2 = ev.endTime().hour() * 2;
    y2 += db_round30min( ev.endTime().minute() );
    if ( y > 47 ) y = 47;
    if ( y2 > 48 ) y2 = 48;
    y = (y * rowHeight) / 2;
    y2 = (y2 * rowHeight) / 2;

    int h;
    if ( ev.isAllDay() ) {
        h = (48 * rowHeight) / 2;
        y = 0;
    }
    else {
        h=y2-y;
        if ( h < (1*rowHeight)/2 ) h = (1*rowHeight)/2;
    }

    int dow = ev.date().dayOfWeek();
    if ( !bOnMonday ) {
        if ( dow == 7 )
            dow = 1;
        else
            dow++;
    }
    int x = header->sectionPos( dow ) - 1;
    int xlim = header->sectionPos( dow ) + header->sectionSize( dow );
    int itemwidth = header->sectionSize( dow ) / 4;
    DateBookWeekItem *isect = 0;
    do {
        i->setGeometry( x, y, itemwidth, h );
        isect = intersects( i );
        x += itemwidth - 1;
    } while ( isect && x < xlim );
}

DateBookWeekItem *DateBookWeekView::intersects( const DateBookWeekItem *item )
{
    QRect geom = item->geometry();

    // We allow the edges to overlap
    geom.moveBy( 1, 1 );
    geom.setSize( geom.size()-QSize(2,2) );

    QListIterator<DateBookWeekItem> it(items);
    for ( ; it.current(); ++it ) {
        DateBookWeekItem *i = it.current();
        if ( i != item ) {
            if ( i->geometry().intersects( geom ) ) {
                return i;
            }
        }
    }

    return 0;
}

void DateBookWeekView::contentsMousePressEvent( QMouseEvent *e )
{
    QListIterator<DateBookWeekItem> it(items);
    for ( ; it.current(); ++it ) {
        DateBookWeekItem *i = it.current();
        if ( i->geometry().contains( e->pos() ) ) {
            showingEvent = true;
            emit signalShowEvent( i->event() );
            break;
        }
    }
}

void DateBookWeekView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    if ( showingEvent ) {
        showingEvent = false;
        emit signalHideEvent();
    } else {
        int d = header->sectionAt( e->pos().x() );
        if ( d > 0 ) {
//            if ( !bOnMonday )
//                d--;
            emit showDay( d );
        }
    }
}

void DateBookWeekView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    QRect ur( cx, cy, cw, ch );
    p->setPen( lightGray );
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
                }
                else {
                    if ( 12 - t < 3 )
                        s = "";
                    else
                        s = " ";
                    s += QString::number( t );
                }
            }
            else {
                s = QString::number( t );
                if ( s.length() == 1 )
                    s.prepend( "0" );
            }
            p->drawText( 1, y+p->fontMetrics().ascent()+1, s );
        }
    }

    QListIterator<DateBookWeekItem> it(items);
    for ( ; it.current(); ++it ) {
        DateBookWeekItem *i = it.current();
        if ( i->geometry().intersects( ur ) ) {
            p->setBrush( i->color() );
            p->drawRect( i->geometry() );
        }
    }
}

void DateBookWeekView::resizeEvent( QResizeEvent *e )
{
    QScrollView::resizeEvent( e );

    int hourWidth;
    QFontMetrics fm( font() );
    if ( ampm )
        hourWidth = fm.width("12p") * 1.25;
    else
        hourWidth = fm.width("20") * 1.25;
    
    //HEAD
    /*
    int avail = visibleWidth();
    header->setGeometry( leftMargin()+frameWidth()+frameRect().left() , frameWidth(),
              visibleWidth(), header->sizeHint().height() );
    setMargins( 0, header->sizeHint().height(), 0, 0 );
    */
    //BRANCH_1_0
    int avail = width()-qApp->style().scrollBarExtent().width()-1;
    header->setGeometry( 0, 0, avail, header->sizeHint().height() );
    setMargins( 0, header->height(), 0, 0 );


    header->resizeSection( 0, hourWidth );
    int sw = (avail - hourWidth) / 7;
    for ( int i = 1; i < 7; i++ )
        header->resizeSection( i, sw );
    header->resizeSection( 7, avail - hourWidth - sw*6 );

    // Move all the items out of the way so we don't get spurious intersections
    for ( QListIterator<DateBookWeekItem> it(items); it.current(); ++it ) {
        DateBookWeekItem *i = it.current();
        i->setGeometry(0,0,0,0);
    }

    for (  QListIterator<DateBookWeekItem> it(items); it.current(); ++it ) {
        DateBookWeekItem *i = it.current();
        positionItem( i );
    }
    viewport()->update();
}

void DateBookWeekView::setStartOfWeek( bool bStartOnMonday )
{
    bOnMonday = bStartOnMonday;
    initNames();
}
