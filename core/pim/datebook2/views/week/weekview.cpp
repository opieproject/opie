#include <qtimer.h>
#include <qlayout.h>

#include <qpe/qpeapplication.h>
#include <opie2/oresource.h>

#include "bookmanager.h"
#include "mainwindow.h"
#include "datebookweek.h"
#include "datebookweekheaderimpl.h"

#include "weekview.h"

using namespace Opie;
using namespace Opie::Datebook;

WeekView::WeekView( MainWindow* window, QWidget* parent )
    : View(window, parent), startTime( 0 )
{
    initUI(parent);
}

QPixmap WeekView::pixmap() const  {
    return Opie::Core::OResource::loadPixmap( "week", Opie::Core::OResource::SmallIcon );
}

QCString WeekView::type() const  {
    return QCString("WeekView");
}

QString WeekView::name() const  {
    return QString(QObject::tr("Week"));
}

QString WeekView::description() const {
    return QString(QObject::tr("View a single week's events"));
}

const OPimOccurrence* WeekView::currentItem() const {
    // FIXME
    return NULL;
}

void WeekView::currentRange( const QDateTime& src, const QDateTime& from) {
}

void WeekView::clockChanged() {
}

void WeekView::dayChanged() {
    bool startMonday = weekStartOnMonday();
    m_view->setStartOfWeek( startMonday );
    m_header->setStartOfWeek( startMonday );
    reschedule();
}

void WeekView::timeChanged() {
}

void WeekView::showDay( const QDate& date )  {
    setDate(date);
}

QWidget* WeekView::widget() {
    return m_widget;
}

void WeekView::reschedule() {
    getEvents();
}

void WeekView::doLoadConfig( Config *config ) {
    startTime = config->readNumEntry("startviewtime", 8);
}

void WeekView::doSaveConfig( Config* ) {
}

QWidget *WeekView::createConfigWidget( QWidget *owner ) {
    return NULL;
}

void WeekView::readConfigWidget( QWidget *widget ) {
}

void WeekView::initUI(QWidget *parent) {
    m_widget = new QWidget(parent, "weekviewwidget");

    QVBoxLayout *vb = new QVBoxLayout( m_widget );
    m_header = new DateBookWeekHeader( weekStartOnMonday(), m_widget );
    m_view = new DateBookWeekView( this, isAP(), weekStartOnMonday(), m_widget );
    m_view->setFocusPolicy(QWidget::StrongFocus);
    vb->addWidget( m_header );
    vb->addWidget( m_view );

    m_preview = new DatebookWeekItemPreview( m_widget, "event label" );
    m_preview->hide();

    tHide = new QTimer( this );

    connect( m_view, SIGNAL( showDay(int) ), this, SLOT( showDay(int) ) );
    connect( m_view, SIGNAL(signalShowEvent(const Opie::OPimOccurrence&)), this, SLOT(slotShowEvent(const Opie::OPimOccurrence&)) );
    connect( m_view, SIGNAL(signalHideEvent()), this, SLOT(slotHideEvent()) );
    connect( m_view, SIGNAL(signalDateChanged(QDate&)), this, SLOT(slotViewDateChanged(QDate&) ) );
    connect( m_header, SIGNAL( dateChanged(QDate&) ), this, SLOT( dateChanged(QDate&) ) );
    connect( m_header, SIGNAL( dateChanged(QDate&) ), m_view, SLOT( slotDateChanged(QDate&) ) );
    connect( tHide, SIGNAL( timeout() ), m_preview, SLOT( hide() ) );
    connect( qApp, SIGNAL(weekChanged(bool)), this, SLOT(slotWeekChanged(bool)) );
    connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(slotClockChanged(bool)));

    bdate = QDate(1900,1,1); // so showDay() gets called by MainWindow
}

void WeekView::getEvents()
{
    QDate startWeek = weekDate();
    QDate endWeek = startWeek.addDays( 6 );
    OPimOccurrence::List eventList = events(startWeek, endWeek);
    m_view->showEvents( eventList );
    m_view->moveToHour( startTime );
}

QDate WeekView::date() const
{
    return bdate;
}

// return the date at the beginning of the week...
QDate WeekView::weekDate() const
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

void WeekView::setDate( int y, int m, int d )
{
    setDate( QDate(y, m, d) );
}

void WeekView::setDate( const QDate &newdate )
{
    bdate = newdate;
    dow = newdate.dayOfWeek();
    m_header->setDate( newdate );
}

DateBookWeekView *WeekView::weekViewWidget()
{
    return m_view;
}

void WeekView::showDay( int day )
{
    QDate d = bdate;

    // Calculate offset to first day of week.
    int dayoffset;
    if( weekStartOnMonday() )
        dayoffset = d.dayOfWeek() - 1;
    else
        dayoffset = d.dayOfWeek() % 7;

    day--;
    d = d.addDays( day - dayoffset );

    setDate( d );
    showDayView();
}

void WeekView::dateChanged( QDate &newdate )
{
    bdate = newdate;
    reschedule();
}

// FIXME slotYearChanged isn't being connected to anything...
void WeekView::slotYearChanged( int y )
{
    int totWeek;
    QDate d( y, 12, 31 );
    int throwAway;
    calcWeek( d, totWeek, throwAway, weekStartOnMonday() );
    while ( totWeek == 1 ) {
        d = d.addDays( -1 );
        calcWeek( d, totWeek, throwAway, weekStartOnMonday() );
    }
}

void WeekView::slotViewDateChanged( QDate &date )
{
    setDate( date );
}

void WeekView::generateAllDayTooltext( QString& text ) {
    text += "<b>" + tr("This is an all day event.") + "</b><br>";
}

void WeekView::generateNormalTooltext( QString& str, const OPimOccurrence &ev ) {
    str += "<b>" + QObject::tr("Start") + "</b>: ";
    str += TimeString::timeString( ev.toEvent().startDateTime().time(), isAP(), FALSE );
//X    if( ev.startDate()!=ev.endDate() ) {
//X        str += " <i>" + TimeString::longDateString( ev.date() )+"</i>";
//X    }
    str += "<br>";
    str += "<b>" + QObject::tr("End") + "</b>: ";
    str += TimeString::timeString( ev.toEvent().endDateTime().time(), isAP(), FALSE );
//X    if( ev.startDate()!=ev.endDate() ) {
//X        str += " <i>" + TimeString::longDateString( ev.endDate() ) + "</i>";
//X    }
}

void WeekView::slotShowEvent( const OPimOccurrence &ev )
{
    if ( tHide->isActive() )
        tHide->stop();

    // why would someone use "<"?  Oh well, fix it up...
    // I wonder what other things may be messed up...
    QString strDesc = occurrenceDesc( ev );
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
//  strCat.remove( where, 1 );
//  strCat.insert( where, "&#60;" );
//  where = strCat.find( "<", where );
//     }

    QString strLocation = ev.location();
    where = strLocation.find( "<" );
    while ( where != -1 ) {
        strLocation.remove( where, 1 );
        strLocation.insert( where, "&#60;" );
        where = strLocation.find( "<", where );
    }

    QString strNote = ev.note();
    where = strNote.find( "<" );
    while ( where != -1 ) {
        strNote.remove( where, 1 );
        strNote.insert( where, "&#60;" );
        where = strNote.find( "<", where );
    }

    QString str = "<b>" + strDesc + "</b><br>"
                  + strLocation + "<br>"
                  + "<i>" + strCat + "</i>"
                  + "<br>" + TimeString::longDateString( ev.date() )
              + "<br>";

    if ( ev.isAllDay() )
        generateAllDayTooltext( str );
    else
        generateNormalTooltext( str, ev );

    str += "<br><br>" + strNote;

    m_preview->setText( str );
    m_preview->resize( m_preview->sizeHint() );
    // move the label so it is "centered" horizontally...
    m_preview->move( QMAX(0,(m_widget->width() - m_preview->width()) / 2), 0 );
    m_preview->show();
}

void WeekView::slotHideEvent()
{
    tHide->start( 2000, true );
}
