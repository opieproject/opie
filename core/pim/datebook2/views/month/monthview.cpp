#include <qtimer.h>
#include <qlayout.h>

#include <qpe/qpeapplication.h>
#include <opie2/oresource.h>

#include "bookmanager.h"
#include "mainwindow.h"
#include "odatebookmonth.h"

#include "monthview.h"

using namespace Opie;
using namespace Opie::Datebook;

MonthView::MonthView( MainWindow* window, QWidget* parent )
    : View(window, parent)
{
    initUI(parent);
}

QPixmap MonthView::pixmap() const  {
    return Opie::Core::OResource::loadPixmap( "month", Opie::Core::OResource::SmallIcon );
}

QCString MonthView::type() const  {
    return QCString("MonthView");
}

QString MonthView::name() const  {
    return QString(QObject::tr("Month view"));
}

QString MonthView::description() const {
    return QString(QObject::tr("Block style calendar view"));
}

const OPimOccurrence* MonthView::currentItem() const {
    // FIXME
    return NULL;
}

void MonthView::currentRange( const QDateTime& src, const QDateTime& from) {
}

void MonthView::clockChanged() {
}

void MonthView::dayChanged() {
}

void MonthView::timeChanged() {
}

void MonthView::showDay( const QDate& date )  {
    setDate(date);
}

QWidget* MonthView::widget() {
    return m_widget;
}

void MonthView::reschedule() {
    m_widget->redraw();
}

void MonthView::doLoadConfig( Config* ) {
}

void MonthView::doSaveConfig( Config* ) {
}

void MonthView::initUI(QWidget *parent) {
    m_widget = new ODateBookMonth( parent, "month view", FALSE );
    connect( m_widget, SIGNAL( dateClicked(int,int,int) ), this, SLOT( dateClicked(int,int,int) ) );
    connect( m_widget, SIGNAL( populateEvents() ), this, SLOT( slotPopulateEvents() ) );
//X    connect( this, SIGNAL( newEvent() ), monthView, SLOT( redraw() ) );
}

void MonthView::getEvents() {
    QDate dtStart, dtEnd;
    m_widget->visibleDateRange( dtStart, dtEnd );
    OPimOccurrence::List eventList = events( dtStart, dtEnd );    
    m_widget->setEvents( eventList );
}

QDate MonthView::date() const
{
    return m_widget->date();
}

void MonthView::setDate( const QDate &newdate )
{
    m_widget->setDate( newdate );
}

void MonthView::slotPopulateEvents()
{
    getEvents();
}

void MonthView::dateClicked( int y, int m, int d ) 
{
    showDayView();
}
