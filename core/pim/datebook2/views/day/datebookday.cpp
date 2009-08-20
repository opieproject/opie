/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
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

#include "datebookday.h"
#include "datebookdayallday.h"
#include "view.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/ir.h>
#include <qpe/qpeapplication.h>

#include <qsimplerichtext.h>
#include <qpopupmenu.h>


#include <qtimer.h>

using namespace Opie;
using namespace Opie::Datebook;

DateBookDayView::DateBookDayView( bool whichClock, DayView *dv, QWidget *parent, const char *name )
    : QTable( 24, 1, parent, name ), ampm( whichClock ), currDate( QDate::currentDate() )
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

    dayview = dv;

    // get rid of being able to edit things...
    QTableItem *tmp;
    int row;
    for ( row = 0; row < numRows(); row++ ) {
        tmp = new QTableItem( this, QTableItem::Never, QString::null);
        setItem( row, 0, tmp );
        //setRowHeight( row, 40);
    }

    initHeader();
    QObject::connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(slotChangeClock(bool)) );
}

void DateBookDayView::initHeader()
{
    QString strTmp;
    int preferredWidth = 0;
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

        // Compute correct width for current Font (Add some space right)
        int actWidth = QFontMetrics( QFont::defaultFont() ).width( strTmp + QString("  ") );
        if ( preferredWidth < actWidth )
            preferredWidth = actWidth;
        setRowStretchable( i, FALSE );
    }

    // It seems as if the header has a bug. It does not resize
    // correct horizontally if it is used vertical..
    // Thus, we do it manually..
    setLeftMargin( preferredWidth );
}

void DateBookDayView::slotDateChanged( int y, int m, int d )
{
    currDate.setYMD(y,m,d);
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
    }
    else {
        switch(e->key()) {
            case Key_Up:
                moveUp();
                break;
            case Key_Down:
                moveDown();
                break;
            case Key_Left:
                currDate = currDate.addDays(-1);
                emit sigDateChanged( currDate.year(), currDate.month(), currDate.day() );
                break;
            case Key_Right:
                currDate = currDate.addDays(1);
                emit sigDateChanged( currDate.year(), currDate.month(), currDate.day() );
                break;
            default:
                e->ignore();
        }        // I don't know what this key is, do you?
    }
}

void DateBookDayView::setRowStyle( int style )
{
    if (style<0) style = 0;

    for (int i=0; i<numRows(); i++)
        setRowHeight(i, style*10+20);
}

void DateBookDayView::selectedHours( int &sh, int &eh )
{
    sh=99;
    eh=-1;

    int n = numSelections();

    for (int i=0; i<n; i++) {
        QTableSelection sel = this->selection( i );
        sh = QMIN(sh,sel.topRow());
        eh = QMAX(sh,sel.bottomRow()+1);
    }

    if (sh > 23 || eh < 1) {
        sh=8;
        eh=9;
    }
}

void DateBookDayView::contentsMouseReleaseEvent( QMouseEvent* e )
{
    if( e->button() != RightButton ) {
        int sh=99,eh=-1;
        selectedHours(sh, eh);
        QDateTime start(currDate, QTime(sh,0,0,0));
        QDateTime end(currDate, QTime(eh,0,0,0));
        quickLineEdit=new DateBookDayViewQuickLineEdit( start, end, dayview, "quickedit");
        quickLineEdit->setGeometry(0,0,this->columnWidth(0)-1,this->rowHeight(0));
        this->moveChild(quickLineEdit,0,sh*this->rowHeight(0));
        quickLineEdit->setFocus();
        quickLineEdit->show();
    }
}

void DateBookDayView::selectedDateTimes( QDateTime &start, QDateTime &end )
{
    start.setDate( currDate );
    end.setDate( currDate );

    int sh=99,eh=-1;
    selectedHours(sh, eh);

    start.setTime( QTime( sh, 0, 0 ) );
    end.setTime( QTime( eh, 0, 0 ) );
}


//===========================================================================

DateBookDayViewQuickLineEdit::DateBookDayViewQuickLineEdit(const QDateTime &start, const QDateTime &end, DayView * dv, const char *name) : QLineEdit(dv->dayViewWidget()->viewport(), name)
{
    active=1;
    quickEvent.setStartDateTime(start);
    quickEvent.setEndDateTime(end);
    dayview = dv;
    connect(this,SIGNAL(returnPressed()),this,SLOT(slotReturnPressed()));
}

void DateBookDayViewQuickLineEdit::slotReturnPressed()
{
    if(active && (!this->text().isEmpty())) {   // Fix to avoid having this event beeing added multiple times.
        quickEvent.setDescription(this->text());
        quickEvent.assignUid(); // FIXME should this be being done elsewhere instead?
        connect(this, SIGNAL(insertEvent(const Opie::OPimEvent&)), dayview, SLOT(insertEvent(const Opie::OPimEvent&)));
        emit(insertEvent(quickEvent));
        active=0;
    }
        /* we need to return to this object.. */
    QTimer::singleShot(500, this, SLOT(finallyCallClose())  );  // Close and also delete this widget
}

void DateBookDayViewQuickLineEdit::finallyCallClose() {
    close(true); // also deletes this widget...
}

void DateBookDayViewQuickLineEdit::focusOutEvent ( QFocusEvent* /* e */)
{
    slotReturnPressed(); // Reuse code to add event and close this widget.
}

void DateBookDayViewQuickLineEdit::mousePressEvent( QMouseEvent *e )
{
    // Disable the popup menu (which causes focusOut, resulting in the widget
    // being destroyed and then a crash occurring)
    if( e->button() != RightButton )
        QLineEdit::mousePressEvent(e);
}

//===========================================================================

DateBookDayWidget::DateBookDayWidget( const OPimOccurrence &e, DayView *dv )
    : QWidget( dv->dayViewWidget()->viewport() ), ev( e ), dayView( dv )
{
    // why would someone use "<"?  Oh well, fix it up...
    // I wonder what other things may be messed up...
    QString strDesc = View::occurrenceDesc( ev );
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
//  strCat.remove( where, 1 );
//  strCat.insert( where, "&#60;" );
//  where = strCat.find( "<", where );
//     }

    QString strNote = ev.note();
    where = strNote.find( "<" );
    while ( where != -1 ) {
        strNote.remove( where, 1 );
        strNote.insert( where, "&#60;" );
        where = strNote.find( "<", where );
    }

    text = "<b>" + strDesc + "</b><br>" + "<i>";
    if ( !strCat.isEmpty() )  {
        text += strCat + "</i><br>";
    }
    if (ev.isAllDay() )
        setAllDayText( text );
    else
        setEventText( text );


    text += "<br><br>" + strNote;

    setBackgroundMode( PaletteBase );

    QTime start = ev.startTime();
    QTime end = ev.endTime();
    int y = start.hour()*60+start.minute();
    int h = end.hour()*60+end.minute()-y;
    int rh = dayView->dayViewWidget()->rowHeight(0);
    y = y*rh/60;
    h = h*rh/60;

    if ( h < 12 ) h = 12;   // Make sure the widget is no smaller than 12 pixels high, so that it's possible to read atleast the first line.
    if ( y > ((24*rh)-12) ) y=(24*rh)-12;   // Make sure the widget fits inside the dayview.
    geom.setY( y );
    geom.setHeight( h );
    geom.setX( 0 );
    geom.setWidth(dayView->dayViewWidget()->columnWidth(0)-1);

}

void DateBookDayWidget::setAllDayText( QString &text ) {
    text += "<b>" + tr("This is an all day event.") + "</b>";
}

void DateBookDayWidget::setEventText( QString& text ) {
    bool whichClock = dayView->dayViewWidget()->whichClock();
/*X    if ( ev.startDate() != ev.endDate() ) {
        text += "<b>" + tr("Start") + "</b>: ";
        text += TimeString::timeString( ev.toEvent().startDateTime().time(), whichClock, FALSE );
        text += " - " + TimeString::longDateString( ev.startDate() ) + "<br>";
        text += "<b>" + tr("End") + "</b>: ";
        text += TimeString::timeString( ev.toEvent().endDateTime().time(), whichClock, FALSE );
        text += " - " + TimeString::longDateString( ev.endDate() );
    } else {*/
        text += "<b>" + tr("Time") + "</b>: ";
        text += TimeString::timeString( ev.startTime(), whichClock, FALSE );
        text += "<b>" + tr(" - ") + "</b>";
        text += TimeString::timeString( ev.endTime(), whichClock, FALSE );
//X    }
}

DateBookDayWidget::~DateBookDayWidget()
{
}

void DateBookDayWidget::paintEvent( QPaintEvent *e )
{
    QPainter p( this );

    if (dayView->getSelectedWidget() == this) {
        p.setBrush( QColor( 155, 240, 230 ) ); // selected item
    } else {
        if (dayView->date() == QDate::currentDate()) {
            QTime curTime = QTime::currentTime();
            if (ev.endTime() < curTime) {
                p.setBrush( QColor( 180, 180, 180 ) ); // grey, inactive
            } else {
                //change color in dependence of the time till the event starts
                int duration = curTime.secsTo(ev.startTime());
                if (duration < 0) duration = 0;
                int colChange = duration*160/86400; //86400: secs per day, 160: max color shift
                p.setBrush( QColor( 200-colChange, 200-colChange, 255 ) ); //blue
            }
        } else {
            p.setBrush( QColor( 220, 220, 220 ) ); //light grey, inactive (not current date)
            //perhaps make a distinction between future/past dates
        }
    }

    p.setPen( QColor(100, 100, 100) );
    p.drawRect(rect());

 //   p.drawRect(0,0, 5, height());

    int y = 0;
    int d = 0;

    if ( ev.toEvent().hasNotifiers() ) {
        p.drawPixmap( width() - AppLnk::smallIconSize(), 0,
                      Opie::Core::OResource::loadPixmap( "bell", Opie::Core::OResource::SmallIcon ) );
        y = AppLnk::smallIconSize() + 2;
        d = 20;
    }

    if ( ev.toEvent().hasRecurrence() ) {
        p.drawPixmap( width() - AppLnk::smallIconSize(), y,
                      Opie::Core::OResource::loadPixmap( "repeat", Opie::Core::OResource::SmallIcon ) );
        d = 20;
        y += AppLnk::smallIconSize() + 2;
    }

    QSimpleRichText rt( text, font() );
    rt.setWidth( geom.width() - d - 6 );
    rt.draw( &p, 7, 0, e->region(), colorGroup() );
}

void DateBookDayWidget::mousePressEvent( QMouseEvent *e )
{
    DateBookDayWidget *item;

    item = dayView->getSelectedWidget();
    if (item)
        item->update();

    dayView->setSelectedWidget(this);
    update();
    dayView->dayViewWidget()->repaint();

    dayView->popup( e->globalPos() );
}

void DateBookDayWidget::setGeometry( const QRect &r )
{
    geom = r;
    setFixedSize( r.width()+1, r.height()+1 );
    dayView->dayViewWidget()->moveChild( this, r.x(), r.y()-1 );
    show();
}


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------


DateBookDayTimeMarker::DateBookDayTimeMarker( DateBookDayView *dv )
    : QWidget( dv->viewport() ), dayView( dv )
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
    int rh = dayView->rowHeight(0);
    y = y*rh/60;

    geom.setX( 0 );

    int x = dayView->columnWidth(0)-1;
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
    dayView->moveChild( this, r.x(), r.y()-1 );
    show();
}
