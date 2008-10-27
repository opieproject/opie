#include <qpe/qpeapplication.h>
#include <opie2/oresource.h>

#include "bookmanager.h"
#include "namespace_hack.h"
#include "datebookweeklstheader.h"
#include "datebookweeklstview.h"
#include "datebookweeklstdblview.h"

#include "weeklstview.h"
#include "mainwindow.h"

using namespace Opie;
using namespace Opie::Datebook;

WeekLstView::WeekLstView( MainWindow* window, QWidget* parent )
    : View(window, parent)
{
    initUI(parent);
}

QPixmap WeekLstView::pixmap() const  {
    return Opie::Core::OResource::loadPixmap( "week", Opie::Core::OResource::SmallIcon );
}

QCString WeekLstView::type() const  {
    return QCString("WeekLstView");
}

QString WeekLstView::name() const  {
    return QString(QObject::tr("Week List view"));
}

QString WeekLstView::description() const {
    return QString(QObject::tr("View a week's events in a list"));
}

const OPimOccurrence* WeekLstView::currentItem() const {
    return &popupItem;
}

void WeekLstView::currentRange( const QDateTime& src, const QDateTime& from) {
}

void WeekLstView::clockChanged() {
    // FIXME
}

void WeekLstView::dayChanged() {
    // FIXME notify header    
    reschedule();
}

void WeekLstView::timeChanged() {
}

void WeekLstView::showDay( const QDate& date )  {
    setDate(date);
}

QWidget* WeekLstView::widget() {
    return m_widget;
}

void WeekLstView::reschedule() {
    getEvents();
}

void WeekLstView::doLoadConfig( Config* ) {
}

void WeekLstView::doSaveConfig( Config* ) {
}

void WeekLstView::initUI(QWidget *parent) {
    m_widget = new QWidget(parent, "weeklstviewwidget");

    m_widget->setFocusPolicy(QWidget::StrongFocus);
    dateset = false;
    m_layout = new QVBoxLayout( m_widget );
    m_layout->setMargin(0);

    m_header = new DateBookWeekLstHeader( weekStartOnMonday(), m_widget );
    m_layout->addWidget( m_header );
    connect(m_header, SIGNAL(dateChanged(QDate&)), this, SLOT(dateChanged(QDate&)));
    connect(m_header, SIGNAL(setDbl(bool)), this, SLOT(setDbl(bool)));

    m_scroll = new QScrollView(m_widget);
    m_scroll->setResizePolicy(QScrollView::AutoOneFit);
    m_layout->addWidget(m_scroll);

    m_CurrentView=NULL;
//X    Config config("DateBook");
//X    config.setGroup("Main");
//X    dbl=config.readBoolEntry("weeklst_dbl", false);
//X    header->dbl->setOn(dbl);

    bdate = QDate(1900,1,1); // so showDay() gets called by MainWindow
}

void WeekLstView::getEvents()
{
    if (!dateset) 
        return;

    QDate startWeek = weekDate();
    QDate endWeek = startWeek.addDays( 6 );
    OPimOccurrence::List eventList = events(startWeek, endWeek);

    QDate startWeek2;
    OPimOccurrence::List eventList2;

    if (dbl) {
        startWeek2 = startWeek.addDays(7);
        endWeek = startWeek2.addDays(6);
        eventList2 = events(startWeek2, endWeek);
    }
    if (!m_CurrentView) {
        if (dbl) {
            m_CurrentView = new DateBookWeekLstDblView(this, eventList, eventList2, startWeek, weekStartOnMonday(), isAP(), m_scroll);
        } else {
            m_CurrentView = new DateBookWeekLstDblView(this, eventList, startWeek, weekStartOnMonday(), isAP(), m_scroll);
        }
        m_CurrentView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
//X        connect (m_CurrentView, SIGNAL(editEvent(const EffectiveEvent&)), this, SIGNAL(editEvent(const EffectiveEvent&)));
//X        connect (m_CurrentView, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
//X        connect (m_CurrentView, SIGNAL(removeEvent(const EffectiveEvent &)), this, SIGNAL(removeEvent(const EffectiveEvent &)));
//X        connect (m_CurrentView, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
        connect (m_CurrentView, SIGNAL(redraw()), this, SLOT(redraw()));
        connect (m_CurrentView, SIGNAL(showDate(int,int,int)), this, SLOT(showDate(int,int,int)));
        connect (m_CurrentView, SIGNAL(addEvent(const QDateTime&,const QDateTime&)),
            this, SLOT(addEvent(const QDateTime&,const QDateTime&)));
        connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(slotClockChanged(bool)));
        m_scroll->addChild(m_CurrentView);
    } else {
        if (dbl) {
            m_CurrentView->setEvents(eventList, eventList2, startWeek, weekStartOnMonday());
        } 
        else {
            m_CurrentView->setEvents(eventList, startWeek, weekStartOnMonday());
        }
    }
    m_scroll->updateScrollBars();
}

void WeekLstView::setDbl(bool on) {
    dbl = on;
    bool displayed = false;
    if (m_CurrentView) {
        displayed = m_CurrentView->toggleDoubleView(on);
    }
    if (!displayed||dbl) {
        getEvents();
    }
}

QDate WeekLstView::date() const
{
    return bdate;
}

// return the date at the beginning of the week...
QDate WeekLstView::weekDate() const
{
    QDate d=bdate;

    // Calculate offset to first day of week.
    int dayoffset=d.dayOfWeek();
    if( weekStartOnMonday() ) 
        dayoffset--;
    else if( dayoffset == 7 )
        dayoffset = 0;

    return d.addDays(-dayoffset);
}

void WeekLstView::setDate( int y, int m, int d )
{
    setDate( QDate(y, m, d) );
}

void WeekLstView::setDate( const QDate &newdate )
{
    bdate = newdate;
    dow = newdate.dayOfWeek();
    m_header->setDate( newdate );
}

void WeekLstView::dateChanged( QDate &newdate )
{
    bdate = newdate;
    dateset = true;
    reschedule();
}

void WeekLstView::popup( const OPimOccurrence &occ, const QPoint &pt )
{
    popupItem = occ;
    View::popup(occ, pt);
}

void WeekLstView::showDate( int y, int m, int d )
{
    setDate(y,m,d);
    showDayView();
}

void WeekLstView::addEvent( const QDateTime &start, const QDateTime &end )
{
    add(start,end);
}
