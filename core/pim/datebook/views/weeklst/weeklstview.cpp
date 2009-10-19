#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtoolbutton.h>

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
    m_timeDisplay = 0;
    m_dbl = false;
    initUI(parent);
}

QPixmap WeekLstView::pixmap() const  {
    return Opie::Core::OResource::loadPixmap( "week", Opie::Core::OResource::SmallIcon );
}

QCString WeekLstView::type() const  {
    return QCString("WeekLstView");
}

QString WeekLstView::name() const  {
    return QString(QObject::tr("Week List"));
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

void WeekLstView::doLoadConfig( Config *config ) {
    m_timeDisplay = config->readNumEntry("weeklistviewconfig", 0);
    m_dbl = config->readBoolEntry("weeklst_dbl", false);
    m_header->dbl->setOn(m_dbl);
}

void WeekLstView::doSaveConfig( Config *config ) {
    config->writeEntry("weeklistviewconfig", m_timeDisplay);
    config->writeEntry("weeklst_dbl", m_dbl);
}

QWidget *WeekLstView::createConfigWidget( QWidget *owner ) {
    QGroupBox *GroupBox5 = new QGroupBox( owner, "GroupBox5" );
    GroupBox5->setTitle( tr( "Week List" ) );
    GroupBox5->setColumnLayout(0, Qt::Vertical );
    GroupBox5->layout()->setSpacing( 0 );
    GroupBox5->layout()->setMargin( 0 );
    QVBoxLayout *GroupBox5Layout = new QVBoxLayout( GroupBox5->layout() );
    GroupBox5Layout->setAlignment( Qt::AlignTop );
    GroupBox5Layout->setSpacing( 6 );
    GroupBox5Layout->setMargin( 11 );

    QHBoxLayout *Layout6 = new QHBoxLayout;
    Layout6->setSpacing( 6 );
    Layout6->setMargin( 0 );

    QLabel *TextLabel2 = new QLabel( GroupBox5, "TextLabel2" );
    TextLabel2->setText( tr( "Time display" ) );
    Layout6->addWidget( TextLabel2 );

    QComboBox *comboWeekListView = new QComboBox( FALSE, GroupBox5, "comboWeekListView" );
    comboWeekListView->insertItem( tr( "None" ) );
    comboWeekListView->insertItem( tr( "Start" ) );
    comboWeekListView->insertItem( tr( "Start-End" ) );
    Layout6->addWidget( comboWeekListView );
    GroupBox5Layout->addLayout( Layout6 );

    comboWeekListView->setCurrentItem( m_timeDisplay );

    return GroupBox5;
}

void WeekLstView::readConfigWidget( QWidget *widget ) {
    QComboBox *comboWeekListView = (QComboBox *)widget->child( "comboWeekListView" );
    if( comboWeekListView )
        m_timeDisplay = comboWeekListView->currentItem();
    reschedule();
}

void WeekLstView::initUI(QWidget *parent) {
    m_widget = new WeekLstViewParentWidget(parent, "weeklstviewwidget");

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

    m_widget->setScroll(m_scroll);
    m_widget->setHeader(m_header);

    m_CurrentView=NULL;

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

    if (m_dbl) {
        startWeek2 = startWeek.addDays(7);
        endWeek = startWeek2.addDays(6);
        eventList2 = events(startWeek2, endWeek);
    }
    if (!m_CurrentView) {
        if (m_dbl) {
            m_CurrentView = new DateBookWeekLstDblView(this, eventList, eventList2, startWeek, weekStartOnMonday(), m_timeDisplay, isAP(), m_scroll);
        } else {
            m_CurrentView = new DateBookWeekLstDblView(this, eventList, startWeek, weekStartOnMonday(), m_timeDisplay, isAP(), m_scroll);
        }
        m_CurrentView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
        connect (m_CurrentView, SIGNAL(redraw()), this, SLOT(redraw()));
        connect (m_CurrentView, SIGNAL(showDate(int,int,int)), this, SLOT(showDate(int,int,int)));
        connect (m_CurrentView, SIGNAL(addEvent(const QDateTime&,const QDateTime&)),
            this, SLOT(addEvent(const QDateTime&,const QDateTime&)));
        connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(slotClockChanged(bool)));
        m_scroll->addChild(m_CurrentView);
    } else {
        if (m_dbl) {
            m_CurrentView->setEvents(eventList, eventList2, startWeek, weekStartOnMonday(), m_timeDisplay);
        }
        else {
            m_CurrentView->setEvents(eventList, startWeek, weekStartOnMonday(), m_timeDisplay);
        }
    }
    m_scroll->updateScrollBars();
}

void WeekLstView::setDbl(bool on) {
    m_dbl = on;
    bool displayed = false;
    if (m_CurrentView) {
        displayed = m_CurrentView->toggleDoubleView(on);
    }
    if (!displayed || m_dbl) {
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
    add(start, end, "");
}

//////////////////////////////////////////////////////////

WeekLstViewParentWidget::WeekLstViewParentWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget(parent, name, fl)
    , m_scroll(0)
    , m_header(0)
{
}

void WeekLstViewParentWidget::setScroll( QScrollView *scroll )
{
    m_scroll = scroll;
}

void WeekLstViewParentWidget::setHeader( DateBookWeekLstHeader *header )
{
    m_header = header;
}

void WeekLstViewParentWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()) {
        case Key_Up:
            m_scroll->scrollBy(0, -20);
            break;
        case Key_Down:
            m_scroll->scrollBy(0, 20);
            break;
        case Key_Left:
            m_header->prevWeek();
            break;
        case Key_Right:
            m_header->nextWeek();
            break;
        default:
            e->ignore();
    }
}

