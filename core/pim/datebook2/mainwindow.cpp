
#include <qcopchannel_qws.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include <qpe/qpeapplication.h>
#include <qpe/ir.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>

#include "show.h"
#include "templatemanager.h"
#include "bookmanager.h"
#include "mainwindow.h"


using namespace Datebook;

MainWindow::MainWindow()
    : OPimMainWindow( "Datebook", 0, 0 ) {
    setIcon( Resource::loadPixmap( "datebook_icon" ) );
    initUI();
    initManagers();
    initView();
    initConfig();

    raiseCurrentView();
    QTimer::singleShot(0, this, SLOT(populate() ) );

    QCopChannel* chan = new QCopChannel( "QPE/System", this );
    connect( chan, SIGNAL( received(const QCString&, const QByteArray& ) ),
             this, SLOT( slotReceive( const QCString&, const QByteArray& ) ) );

    chan  = new QCopChannel( "QPE/Datebook", this );
    connect( chan, SIGNAL( received(const QCString&, const QByteArray& ) ),
             this, SLOT( slotReceive( const QCString&, const QByteArray& ) ) );
}
MainWindow::~MainWindow() {
}
void MainWindow::doSetDocument( const QString& str ) {

}
void MainWindow::flush() {
    manager()->save();
}
void MainWindow::reload() {
    manager()->reload();
}
int MainWindow::create() {
    return 0;
}
bool MainWindow::remove( int uid ) {
    manager()->remove( uid );
    return true;
}
void MainWindow::beam( int uid ) {

}
void MainWindow::show( int uid ) {

    eventShow()->show( manager()->event( uid ) );
}
void MainWindow::add( const OPimRecord& ad) {
    manager()->add( ad );
}
void MainWindow::edit() {
    edit ( currentView()->currentItem() );
}
void MainWindow::edit( int uid ) {

}
/*
 * init tool bars layout and so on
 */
void MainWindow::initUI() {
    setToolBarsMovable( false );

    m_stack = new QWidgetStack( this );
    setCentralWidget( m_stack );

    m_toolBar = new QPEToolBar( this );
    m_toolBar->setHorizontalStretchable( TRUE );

    QPEMenuBar* mb = new QPEMenuBar( m_toolBar );

    m_popView    = new QPopupMenu( this );
    m_popSetting = new QPopupMenu( this );

    mb->insertItem( tr("View"), m_popView );
    mb->insertItem( tr("Settings" ), m_popSetting );

    m_popTemplate = new QPopupMenu( this );
    m_popView->insertItem(tr("New from template"), m_popTemplate, -1, 0);

    QAction* a = new QAction( tr("New Event"), Resource::loadPixmap("new"),
                              QString::null, 0, this, 0 );
    a->addTo( m_toolBar );
    a->addTo( m_popView );
    connect(a, SIGNAL( activated() ), this, SLOT( create() ) );

    a = new QAction( tr("Edit Event"), Resource::loadPixmap("edit"),
                     QString::null, 0, this, 0 );
    a->addTo( m_popView );
    connect(a, SIGNAL( activated() ), this, SLOT( edit() ) );

    a = new QAction( tr("Today" ), Resource::loadPixmap( "datebook/to_day"),
                     QString::null, 0, this, 0 );
    a->addTo( m_toolBar );
    connect(a, SIGNAL( activated() ), this, SLOT( slotGoToNow() ) );

    a = new QAction( tr("Find"), Resource::loadPixmap( "mag" ),
                     QString::null, 0, this, 0 );
    a->addTo( m_toolBar );
    connect(a, SIGNAL( activated() ), this, SLOT( slotFind() ) );

    a = new QAction( tr("Configure"), QString::null, 0, 0 );
    a->addTo( m_popSetting );
    connect(a, SIGNAL( activated() ), this, SLOT( slotConfigure() ) );

    a = new QAction( tr("Configure Locations"), QString::null, 0, 0 );
    a->addTo( m_popSetting );
    connect(a, SIGNAL( activated() ), this, SLOT( slotConfigureLocs() ) );

    a = new QAction( tr("Configure Descriptions"), QString::null, 0, 0 );
    a->addTo( m_popSetting );
    connect(a, SIGNAL( activated() ), this, SLOT(slotConfigureDesc() ) );

    connect( qApp, SIGNAL(clockChanged(bool) ),
             this, SLOT(slotClockChanged(bool) ) );
    connect( qApp, SIGNAL(weekChanged(bool) ),
             this, SLOT(slotWeekChanged(bool) ) );

    connect( qApp, SIGNAL(appMessage(const QCString&, const QByteArray& ) ),
             this, SLOT(slotAppMessage( const QCString&, const QByteArray& ) ) );
}
void MainWindow::initConfig() {

}
void MainWindow::initView() {

}
void MainWindow::initManagers() {
    m_manager = new BookManager;
    m_locMan = new LocationManager( tr("Locations") );
    m_descMan = new DescriptionManager( tr("Descriptions") );
}
void MainWindow::raiseCurrentView() {

}
/*
 * populate the view
 */
void MainWindow::populate() {
    if (!manager()->isLoaded() )
        manager()->load();
}
void MainWindow::slotGoToNow() {

}
View* MainWindow::currentView() {

}
void MainWindow::slotFind() {

}
void MainWindow::slotConfigure() {

}
void MainWindow::slotClockChanged( bool ) {

}
void MainWindow::slotWeekChanged(bool ) {

}
void MainWindow::slotAppMessage( const QCString&, const QByteArray& ) {

}
void MainWindow::slotReceive( const QCString&, const QByteArray& ) {

}
BookManager* MainWindow::manager() {
    return m_manager;
}
TemplateManager* MainWindow::templateManager() {
    return m_tempMan;
}
LocationManager* MainWindow::locationManager() {
    return m_locMan;
}
DescriptionManager* MainWindow::descriptionManager() {
    return m_descMan;
}
Show* MainWindow::eventShow() {
    return m_show;
}
void MainWindow::slotAction( QAction* act ) {

}
void MainWindow::slotConfigureLocs() {

}
void MainWindow::slotConfigureDesc() {

}
void MainWindow::hideShow() {

}
void MainWindow::viewPopup(int ) {

}
void MainWindow::viewAdd(const QDate& ) {

}
void MainWindow::viewAdd( const QDateTime&, const QDateTime& ) {

}
bool MainWindow::viewAP()const{
}
bool MainWindow::viewStartMonday()const {

}
