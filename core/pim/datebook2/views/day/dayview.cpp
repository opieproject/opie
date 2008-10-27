#include <qtimer.h>

#include <qpe/qpeapplication.h>
#include <opie2/oresource.h>

#include "bookmanager.h"
#include "mainwindow.h"
#include "datebookday.h"
#include "datebookdayallday.h"
#include "datebookdayheaderimpl.h"

#include "dayview.h"

using namespace Opie;
using namespace Opie::Datebook;

DayView::DayView( MainWindow* window, QWidget* parent )
    : View(window, parent), currDate( QDate::currentDate() ), startTime( 0 )
{
    rowStyle = -1; // initialize with bogus values
    jumpToCurTime = false;
    selectedWidget = 0;

    initUI(parent);
}

QPixmap DayView::pixmap() const  {
    return Opie::Core::OResource::loadPixmap( "day", Opie::Core::OResource::SmallIcon );
}

QCString DayView::type() const  {
    return QCString("DayView");
}

QString DayView::name() const  {
    return QString(QObject::tr("Day view"));
}

QString DayView::description() const {
    return QString(QObject::tr("View a single day's events"));
}

const OPimOccurrence* DayView::currentItem() const {
    if( selectedWidget )
        return &selectedWidget->event();
    else
        return m_allDays->selectedEvent();
}

void DayView::currentRange( const QDateTime& src, const QDateTime& from) {
}

void DayView::clockChanged() {
}

void DayView::dayChanged() {
    m_header->setStartOfWeek( weekStartOnMonday() );
//    redraw();
}

void DayView::timeChanged() {
}

void DayView::showDay( const QDate& date )  {
    m_header->setDate( date.year(), date.month(), date.day() );
}

QWidget* DayView::widget() {
    return m_box;
}

void DayView::reschedule() {
    relayoutPage();
}

void DayView::doLoadConfig( Config *config ) {
    startTime = config->readNumEntry("startviewtime", 8);
    jumpToCurTime = config->readBoolEntry("jumptocurtime");
    rowStyle = config->readNumEntry("rowstyle");
}

void DayView::doSaveConfig( Config* ) {
}

void DayView::initUI(QWidget *parent) {
    widgetList = new WidgetListClass();
    widgetList->setAutoDelete( true );

    m_box = new QVBox(parent);

    m_header = new DateBookDayHeader( weekStartOnMonday(), m_box, "day header" );
    m_header->setDate( currDate.year(), currDate.month(), currDate.day() );

    m_allDays = new DatebookdayAllday(this, m_box, "all day event list" );
    m_allDays->hide();

    m_view = new DateBookDayView( isAP(), this, m_box, "day view" );

    connect( m_header, SIGNAL( dateChanged(int,int,int) ), this, SLOT( dateChanged(int,int,int) ) );
    connect( m_header, SIGNAL( dateChanged(int,int,int) ), m_view, SLOT( slotDateChanged(int,int,int) ) );
    connect( m_view, SIGNAL( sigColWidthChanged() ), this, SLOT( slotColWidthChanged() ) );
    connect( m_view, SIGNAL(sigCapturedKey(const QString&)), this, SIGNAL(sigNewEvent(const QString&)) );

    QTimer *timer = new QTimer( this );

    connect( timer, SIGNAL(timeout()), this, SLOT(updateView()) );  //connect timer for updating timeMarker & daywidgetcolors
    timer->start( 1000*60*5, FALSE ); //update every 5min

    timeMarker = new DateBookDayTimeMarker( m_view );
    timeMarker->setTime( QTime::currentTime() );
}

void DayView::setJumpToCurTime( bool bJump )
{
    jumpToCurTime = bJump;
}

void DayView::setRowStyle( int style )
{
    if (rowStyle != style) 
        m_view->setRowStyle( style );
    rowStyle = style;
}

void DayView::updateView( void )
{
    timeMarker->setTime( QTime::currentTime() );
    //need to find a way to update all DateBookDayWidgets
}

void DayView::insertEvent(const Opie::OPimEvent &ev) {
    add(ev);
}

void DayView::dateChanged( int y, int m, int d )
{
    QDate date( y, m, d );
    if ( currDate == date )
        return;
    currDate.setYMD( y, m, d );
    relayoutPage();
    dayViewWidget()->clearSelection();
    QTableSelection ts;

    if (jumpToCurTime && this->date() == QDate::currentDate())
    {
        ts.init( QTime::currentTime().hour(), 0);
        ts.expandTo( QTime::currentTime().hour(), 0);
    } else {
        ts.init( startTime, 0 );
        ts.expandTo( startTime, 0 );
    }

    dayViewWidget()->addSelection( ts );
    selectedWidget = 0;
}

void DayView::setSelectedWidget( DateBookDayWidget *w )
{
    selectedWidget = w;
}

DateBookDayWidget * DayView::getSelectedWidget( void )
{
    return selectedWidget;
}

static int place( const DateBookDayWidget *item, bool *used, int maxn )
{
    int place = 0;
    int start = item->event().startTime().hour();
    QTime e = item->event().endTime();
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
        if ( free )
            break;
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


void DayView::relayoutPage( bool fromResize )
{
    m_box->setUpdatesEnabled( FALSE );
    if ( !fromResize ) {
        getEvents();    // no need we already have them!

        if (m_allDays->items() > 0 )
            m_allDays->show();
        /*
            * else if ( m_allDays->items() ==  0 ) already hide in getEvents
            */
    }

    widgetList->sort();
    //sorts the widgetList by the heights of the widget so that the tallest widgets are at the beginning
    //this is needed for the simple algo below to work correctly, otherwise some widgets would be drawn outside the view

    int wCount = widgetList->count();
    int wid = m_view->columnWidth(0)-1;
    int wd;
    int n = 1;

    QArray<int> anzIntersect(wCount); //this stores the number of maximal intersections of each widget

    for (int i = 0; i<wCount; anzIntersect[i] = 1, i++);

    if ( wCount < 20 ) {

        QArray<QRect> geometries(wCount);
        for (int i = 0; i < wCount; geometries[i] = widgetList->at(i)->geometry(), i++); //stores geometry for each widget in vector

        for ( int i = 0; i < wCount; i++) {
            QValueList<int> intersectedWidgets;

            //find all widgets intersecting with widgetList->at(i)
            for ( int j = 0; j < wCount; j++) {
                if (i != j)
                    if (geometries[j].intersects(geometries[i]))
                        intersectedWidgets.append(j);
            }

            //for each of these intersecting widgets find out how many widgets are they intersecting with
            for ( uint j = 0; j < intersectedWidgets.count(); j++)
            {
                QArray<int> inter(wCount);
                inter[j]=1;

                if (intersectedWidgets[j] != -1)
                    for ( uint k = j; k < intersectedWidgets.count(); k++) {
                        if (j != k && intersectedWidgets[k] != -1)
                            if (geometries[intersectedWidgets[k]].intersects(geometries[intersectedWidgets[j]])) {
                                inter[j]++;
                                intersectedWidgets[k] = -1;
                            }
                        if (inter[j] > anzIntersect[i]) anzIntersect[i] = inter[j] + 1;
                    }
                }
            if (anzIntersect[i] == 1 && intersectedWidgets.count()) anzIntersect[i]++;
        }


        for ( int i = 0; i < wCount; i++) {
            DateBookDayWidget *w = widgetList->at(i);
            QRect geom = w->geometry();
            geom.setX( 0 );
            wd = (m_view->columnWidth(0)-1) / anzIntersect[i] - (anzIntersect[i]>1?2:0);
            geom.setWidth( wd );
            while ( intersects( w, geom ) ) {
                geom.moveBy( wd + 2 + 1, 0 );
            }
            w->setGeometry( geom );
        }

        if (jumpToCurTime && this->date() == QDate::currentDate()) {
            m_view->setContentsPos( 0, QTime::currentTime().hour() * m_view->rowHeight(0) ); //set listview to current hour
        } else {
            m_view->setContentsPos( 0, startTime * m_view->rowHeight(0) );
        }
    } else {
        int hours[24];
        memset( hours, 0, 24*sizeof( int ) );
        bool overFlow = FALSE;
        for ( int i = 0; i < wCount; i++ ) {
            DateBookDayWidget *w = widgetList->at(i);
            int start = w->event().startTime().hour();
            QTime e = w->event().endTime();
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
        wid = ( m_view->columnWidth(0)-1 ) / n;

        bool used[24*10];
        memset( used, FALSE, 24*10*sizeof( bool ) );

        for ( int i = 0; i < wCount; i++ ) {
            DateBookDayWidget *w = widgetList->at(i);
            int xp = place( w, used, n );
            if ( xp != -1 ) {
                QRect geom = w->geometry();
                geom.setX( xp*(wid+2) );
                geom.setWidth( wid );
                w->setGeometry( geom );
            }
        }

        if (jumpToCurTime && this->date() == QDate::currentDate()) {
            m_view->setContentsPos( 0, QTime::currentTime().hour() * m_view->rowHeight(0) ); //set listview to current hour
        } else {
            m_view->setContentsPos( 0, startTime * m_view->rowHeight(0) );
        }
    }

    timeMarker->setTime( QTime::currentTime() );    //display timeMarker
    timeMarker->raise();                //on top of all widgets
    if (this->date() == QDate::currentDate()) {     //only show timeMarker on current day
        timeMarker->show();
    } else {
        timeMarker->hide();
    }
    m_box->setUpdatesEnabled( TRUE );
}

DateBookDayWidget *DayView::intersects( const DateBookDayWidget *item, const QRect &geom )
{
    int i = 0;
    DateBookDayWidget *w = widgetList->at(i);
    int wCount = widgetList->count();
    while ( i < wCount && w != item ) {
        if ( w->geometry().intersects( geom ) ) {
            return w;
        }
        w = widgetList->at(++i);
    }

    return 0;
}

void DayView::getEvents()
{
    widgetList->clear();

    QObject* object = 0;

    // clear the AllDay List
    m_allDays->hide(); // just in case
    m_allDays->removeAllEvents();

/*X
    QStringList hdays = _holiday_db->holidaylist(currDate);
    QStringList::Iterator sit;

    // this way we make sure that holiday dummy events are always
    //   on top of the widgetlist.
    for (sit=hdays.begin();sit!=hdays.end();++sit) {
        m_allDays->addHoliday(*sit);
    }
*/

    OPimOccurrence::List eventList = events(currDate, currDate);
    OPimOccurrence::List::ConstIterator it;

    for ( it = eventList.begin(); it != eventList.end(); ++it ) {
        OPimOccurrence ev=*it;
//X        if(!((ev.endTime().hour()==0) && (ev.endTime().minute()==0) && (ev.startDate()!=ev.date()))) {  // Skip events ending at 00:00 starting at another day.
            if ( ev.isAllDay() ) {
                object = m_allDays->addEvent( ev );
                if (!object)
                    continue;
            }
            else {
                DateBookDayWidget* w = new DateBookDayWidget( ev, this );
                widgetList->append( w );
                object = w;
            }
//X        }
    }
}

QDate DayView::date() const
{
    return currDate;
}

DateBookDayView *DayView::dayViewWidget()
{
    return m_view;
}

void DayView::popup( const QPoint &pt )
{
    View::popup( *currentItem(), pt );
}
