/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */
#include "mainwindow.h"
#include "imageview.h"

#include "iconview.h"
#include "filesystem.h"
#include "imageinfoui.h"
#include "viewmodebutton.h"
#include "basesetup.h"

#include <iface/ifaceinfo.h>
#include <iface/dirview.h>

#include <opie2/odebug.h>
#include <opie2/owidgetstack.h>
#include <opie2/oapplicationfactory.h>
#include <opie2/otabwidget.h>
#include <opie2/okeyconfigwidget.h>

#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/ir.h>
#include <qpe/storage.h>
#include <qpe/applnk.h>

#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qmap.h>
#include <qtimer.h>
#include <qframe.h>
#include <qmenubar.h>
#include <qaction.h>

//OPIE_EXPORT_APP_V2( Opie::Core::OApplicationFactory<PMainWindow>,"Opie Eye" )
OPIE_EXPORT_APP( Opie::Core::OApplicationFactory<PMainWindow>)

PMainWindow::PMainWindow(QWidget* wid, const char* name, WFlags style)
    : QMainWindow( wid, name, style ), m_info( 0 ), m_disp( 0 )
{
    setCaption( QObject::tr("Opie Eye" ) );
    m_cfg = new Opie::Core::OConfig("opie-eye");
    m_cfg->setGroup("main" );
    readConfig();
    m_setDocCalled = false;
    m_polishDone = false;
    m_SmallWindow = QApplication::desktop()->size().width()<330;

    m_storage = new StorageInfo();
    connect(m_storage, SIGNAL(disksChanged() ),
            this, SLOT( dirChanged() ) );

    m_stack = new Opie::Ui::OWidgetStack( this );
    setCentralWidget( m_stack );

    m_view = new PIconView( m_stack, m_cfg );
    m_stack->addWidget( m_view, IconView );
    m_stack->raiseWidget( IconView );

    connect(m_view, SIGNAL(sig_display(const QString&)),
            this, SLOT(slotDisplay(const QString&)));
    connect(m_view, SIGNAL(sig_showInfo(const QString&)),
            this, SLOT(slotShowInfo(const QString&)) );
    connect(this,SIGNAL(changeListMode(int)),m_view,SLOT(slotChangeMode(int)));

    m_stack->forceMode(Opie::Ui::OWidgetStack::NoForce);

    listviewMenu = 0;
    /* setup menu and toolbar */
    setupActions();
    setupToolbar();
    setupMenu();
    m_aHideToolbar->setOn(m_cfg->readBoolEntry("showtoolbar",true));
    m_aAutoRotate->setEnabled(!m_aUnscaled->isOn());
    if (m_aForceSmall) {
        m_aForceSmall->setOn(m_cfg->readBoolEntry("dontshowseperate",true));
    }
    odebug << "mainwindow constructor done" << oendl;
}

PMainWindow::~PMainWindow() {
}

void PMainWindow::slotToggleZoomer()
{
    m_aZoomer->setOn(!m_aZoomer->isOn());
}

void PMainWindow::slotZoomerToggled(bool how)
{
    if (m_disp) {
        m_disp->setShowZoomer(how);
    }
    if (autoSave) {
        m_cfg->writeEntry("zoomeron",how);
    }
}

void PMainWindow::slotToggleAutorotate()
{
    if (!m_aAutoRotate->isEnabled()) return;
    m_aAutoRotate->setOn(!m_aAutoRotate->isOn());
}

void PMainWindow::slotToggleAutoscale()
{
    m_aUnscaled->setOn(!m_aUnscaled->isOn());
}

void PMainWindow::slotRotateToggled(bool how)
{
   if (autoSave) {
        m_cfg->writeEntry("autorotate",how);
    }
    if (m_disp) {
        m_disp->setAutoScaleRotate(!m_aUnscaled->isOn(),how);
    }
}

void PMainWindow::slotScaleToggled(bool how)
{
   if (autoSave) {
        m_cfg->writeEntry("unscaled",how);
    }
    odebug << "Unscaled: " << m_aUnscaled->isOn() << oendl;
    odebug << "How: " << how << oendl;
    if (how) {
        m_aAutoRotate->setOn(false);
    }
    if (m_disp) {
        m_disp->setAutoScaleRotate(!m_aUnscaled->isOn(),m_aAutoRotate->isOn());
    }
    m_aAutoRotate->setEnabled(!how);
    odebug << "Autorotate: " << m_aAutoRotate->isOn() << oendl;
}

void PMainWindow::slotConfig() {
   /*
    * have a tab with the possible views
    * a tab for globals image cache size.. scaled loading
    * and one tab for the  KeyConfigs
    */
    QDialog dlg(this, 0, true);
    dlg.setCaption( tr("Opie Eye - Config" ) );

    QHBoxLayout *lay = new QHBoxLayout(&dlg);
    Opie::Ui::OTabWidget *wid = new Opie::Ui::OTabWidget(&dlg );
    lay->addWidget( wid );

    BaseSetup*bSetup = new BaseSetup(m_cfg,wid);
    wid->addTab(bSetup,"SettingsIcon","Basics setup");

    ViewMap *vM = viewMap();
    ViewMap::Iterator _it = vM->begin();
    QMap<PDirView*, QWidget*> lst;

    for( ; _it != vM->end(); ++_it ) {
        PDirView *view = (_it.data())(*m_cfg);
        PInterfaceInfo *inf =  view->interfaceInfo();
        QWidget *_wid = inf->configWidget( *m_cfg );
        if (!_wid) continue;
        _wid->reparent(wid, QPoint() );
        lst.insert( view, _wid );
        wid->addTab( _wid, "fileopen", inf->name() );
    }

/*
 * Add the KeyConfigWidget
 */
    Opie::Ui::OKeyConfigWidget* keyWid = new Opie::Ui::OKeyConfigWidget( wid, "key config" );
    keyWid->setChangeMode( Opie::Ui::OKeyConfigWidget::Queue );
    keyWid->insert( tr("Browser Keyboard Actions"), m_view->manager() );
    QWidget*w = m_stack->visibleWidget();

    bool reminfo = false;
    if ( !m_info ) {
        reminfo = true;
        initInfo();
        m_info->hide();
    }
    keyWid->insert( tr("Imageinfo Keyboard Actions"), m_info->manager() );

    bool remdisp = false;
    if ( !m_disp ) {
        remdisp = true;
        initDisp();
        m_disp->hide();
    }
    keyWid->insert( tr("Imageview Keyboard Actions"), m_disp->manager() );

    keyWid->load();
    wid->addTab( keyWid, QString::fromLatin1("AppsIcon" ), tr("Keyboard Configuration") );
    wid->setCurrentTab(0);
    bool act = ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted );

/*
 * clean up
 *apply changes
 */

    QMap<PDirView*, QWidget*>::Iterator it;
    for ( it = lst.begin(); it != lst.end(); ++it ) {
        if ( act )
            it.key()->interfaceInfo()->writeConfig(it.data(),  *m_cfg);
        delete it.key();
    }


    if ( act ) {
        keyWid->save();
        m_disp->manager()->save();
        m_info->manager()->save();
        m_view->manager()->save();
        bSetup->save_values();
        m_view->resetView();
        readConfig();
    }
    delete keyWid;

    m_stack->raiseWidget(w);
    if (remdisp) {
        m_disp->hide();
    }
    if (reminfo) {
        m_info->hide();
    }
}

/*
 * create a new image info component
 * and detach the current one
 * we will make the other delete on exit
 */
template<class T>
void PMainWindow::initT( const char* name, T** ptr, int id) {
    if ( *ptr ) {
        (*ptr)->disconnect(this, SLOT(slotReturn()));
        (*ptr)->setDestructiveClose();
        m_stack->removeWidget( *ptr );
    }
    *ptr = new T(m_cfg, m_stack, name );
    m_stack->addWidget( *ptr, id );

    connect(*ptr, SIGNAL(sig_return()),
            this,SLOT(slotReturn()));

}

void PMainWindow::initInfo() {
    initT<imageinfo>( "Image Info", &m_info, ImageInfo );
    connect(m_info,SIGNAL(dispImage(const QString&)),this,SLOT(slotDisplay(const QString&)));
}

void PMainWindow::initDisp() {
    initT<ImageView>( "Image ScrollView", &m_disp, ImageDisplay );
    if (m_disp) {
//        if (m_stack->mode() != Opie::Ui::OWidgetStack::SmallScreen) {
            //m_disp->setMinimumSize(QApplication::desktop()->size()/2);
//        }
        m_disp->setMenuActions(m_hGroup,m_gPrevNext,m_gDisplayType);
        m_disp->setAutoScale(!m_aUnscaled->isOn());
        m_disp->setAutoRotate(m_aAutoRotate->isOn());
        m_disp->setShowZoomer(m_aZoomer->isOn());
        m_disp->setBackgroundColor(white);
        connect(m_disp,SIGNAL(dispImageInfo(const QString&)),this,SLOT(slotShowInfo(const QString&)));
        connect(m_disp,SIGNAL(dispNext()),m_view,SLOT(slotShowNext()));
        connect(m_disp,SIGNAL(dispPrev()),m_view,SLOT(slotShowPrev()));
        connect(m_disp,SIGNAL(toggleFullScreen()),this,SLOT(slotToggleFullScreen()));
        connect(m_disp,SIGNAL(hideMe()),this,SLOT(raiseIconView()));
        connect(m_disp,SIGNAL(toggleZoomer()),this,SLOT(slotToggleZoomer()));
        connect(m_disp,SIGNAL(toggleAutoscale()),this,SLOT(slotToggleAutoscale()));
        connect(m_disp,SIGNAL(toggleAutorotate()),this,SLOT(slotToggleAutorotate()));
        connect(m_view,SIGNAL(sig_startslide(int)),m_disp,SLOT(startSlide(int)));
        slotFullScreenToggled(m_aFullScreen->isOn());
    }
}

void PMainWindow::slotToggleFullScreen()
{
    bool current = !m_aFullScreen->isOn();
    m_aFullScreen->setOn(current);
}

void PMainWindow::slotFullScreenButton(bool current)
{
    if (autoSave) {
        m_cfg->writeEntry("fullscreen",current);
    }
    if (!m_disp) return;
    if (m_disp->isVisible()) {
        setupViewWindow(current, true);
    }
}

void PMainWindow::setupViewWindow(bool current, bool forceDisplay)
{
    if (!m_disp) {
        return;
    }
    if (current) {
        m_disp->setBackgroundColor(black);
        m_disp->reparent(0, WStyle_Customize | WStyle_NoBorder, QPoint(0,0));
        m_disp->setVScrollBarMode(QScrollView::AlwaysOff);
        m_disp->setHScrollBarMode(QScrollView::AlwaysOff);
        m_disp->resize(qApp->desktop()->width(), qApp->desktop()->height());
        m_disp->setFullScreen(current,forceDisplay);
    } else {
        setUpdatesEnabled(false);
#if 0
        if (m_stack->mode() != Opie::Ui::OWidgetStack::SmallScreen) {

            //m_disp->setMinimumSize(QApplication::desktop()->size()/2);
        } else {
            //m_disp->setMinimumSize(10,10);
        }
#endif
        m_disp->setBackgroundColor(white);
        m_stack->addWidget(m_disp,ImageDisplay);
        m_disp->setVScrollBarMode(QScrollView::Auto);
        m_disp->setHScrollBarMode(QScrollView::Auto);
        if (m_stack->mode() != Opie::Ui::OWidgetStack::SmallScreen) {
            m_disp->setGeometry(30,30,QApplication::desktop()->width()-60,QApplication::desktop()->height()-60);
        }
        if (forceDisplay || m_disp->isVisible()) {
            m_stack->raiseWidget(m_disp);
            m_disp->setFocus();
        }
        setUpdatesEnabled(true);
    }
    m_disp->setFullScreen(current,forceDisplay);
}

void PMainWindow::slotFullScreenToggled(bool current)
{
    setupViewWindow(current,true);
}

/**
 * With big Screen the plan could be to 'detach' the image
 * window if visible and to create a ne wone
 * init* already supports it but I make no use of it for
 * now. We set filename and raise
 *
 * ### FIXME and talk to alwin
 */
void PMainWindow::slotShowInfo( const QString& inf ) {
    if (m_disp && m_disp->fullScreen() && m_disp->isVisible()) {
        return;
    }
    if ( !m_info ) {
        initInfo();
    }
    m_info->setPath( inf );
    if (m_SmallWindow) {
        m_aNext->removeFrom(toolBar);
        m_aPrevious->removeFrom(toolBar);
        fsButton->hide();
    }
    m_aNext->setEnabled(false);
    m_aPrevious->setEnabled(false);
    m_aDirUp->setEnabled(false);
    m_aShowInfo->setEnabled(false);
    m_aViewfile->setEnabled(true);
    m_aStartSlide->setEnabled(false);
    m_stack->raiseWidget( ImageInfo );
}

void PMainWindow::slotDisplay( const QString& inf ) {
    bool nwindow = false;
    if ( !m_disp ) {
        nwindow = true;
        initDisp();
    }
    m_disp->setImage( inf );
    if (m_SmallWindow) {
        if (m_gPrevNext->isEnabled()==false) {
            m_gPrevNext->addTo(toolBar);
            fsButton->hide();
        }
    }
    m_gPrevNext->setEnabled(true);
    m_aDirUp->setEnabled(false);
    m_aShowInfo->setEnabled(true);
    m_aViewfile->setEnabled(false);
    m_aStartSlide->setEnabled(false);

    if (!nwindow && m_disp->fullScreen()!=m_aFullScreen->isOn()) {
        slotFullScreenToggled(m_aFullScreen->isOn());
    }
    if (m_disp->fullScreen()) {
        qwsDisplay()->requestFocus( m_disp->winId(), TRUE);
    } else {
        m_stack->raiseWidget( ImageDisplay );
    }
}

void PMainWindow::raiseIconView() {
    setUpdatesEnabled(false);
    if (m_SmallWindow) {
        m_gPrevNext->removeFrom(toolBar);
        fsButton->show();
    }
    m_gPrevNext->setEnabled(false);
    m_aDirUp->setEnabled(true);
    m_aShowInfo->setEnabled(true);
    m_aViewfile->setEnabled(true);
    m_aStartSlide->setEnabled(true);

    if (m_disp && m_disp->fullScreen() && m_disp->isVisible()) {
        m_disp->stopSlide();
        m_disp->hide();
    }
    m_stack->raiseWidget( IconView );
    setUpdatesEnabled(true);
    repaint();
}

void PMainWindow::slotReturn() {
    raiseIconView();
}


void PMainWindow::closeEvent( QCloseEvent* ev ) {
    /*
     * return from view
     * or properly quit
     */
    if (!m_setDocCalled) {
        if ( m_stack->visibleWidget() == m_info ||
             m_stack->visibleWidget() == m_disp ) {
            ev->ignore();
            raiseIconView();
            return;
        }
    }
    if (m_disp && m_disp->fullScreen()) {
        /* otherwise opie-eye crashes in bigscreen mode! */
        m_disp->reparent(0,QPoint(0,0));
        m_stack->addWidget(m_disp,ImageDisplay);
    }
    ev->accept();
    QTimer::singleShot(0, qApp, SLOT(closeAllWindows()));
}

void PMainWindow::setDocument( const QString& showImg )
{
    QString file = showImg;
    DocLnk lnk(showImg);
    if (lnk.isValid() )
        file = lnk.file();
    slotDisplay( file );
#if 0
    if (!m_polishDone) {
        QTimer::singleShot(0,this,SLOT(check_view_fullscreen()));
    }
#endif
}

void PMainWindow::check_view_fullscreen()
{
    if (!m_view) return;
    if (!m_view->hasFocus()&&m_aFullScreen->isOn()) {
        qwsDisplay()->requestFocus( m_disp->winId(), TRUE);
    }
}

void PMainWindow::slotSelectDir(int id)
{
    emit changeDir( m_dev[fsMenu->text(id )] );
}

void PMainWindow::dirChanged()
{
    fsMenu->clear();
    m_dev.clear();

    /* home dir, too */
    QString f = getenv( "HOME" );
    if (!f.isEmpty()) {
        m_dev.insert("Home directory",f);
        fsMenu->insertItem("Home directory");
    }
    const QList<FileSystem> &fs = m_storage->fileSystems();
    QListIterator<FileSystem> it(fs );
    for ( ; it.current(); ++it ) {
        const QString disk = (*it)->name();
        const QString path = (*it)->path();
        m_dev.insert( disk, path );
        fsMenu->insertItem( disk );
    }
}

void PMainWindow::showToolbar(bool how)
{
    if (!how) toolBar->hide();
    else toolBar->show();
    if (autoSave) {
        m_cfg->writeEntry("showtoolbar",how);
    }
}

void PMainWindow::setupActions()
{
    m_aDirUp = new QAction( tr( "Go dir up" ), Resource::loadIconSet( "up" ), 0, 0, this, 0, true );
    m_aDirUp->setToggleAction(false);
    connect(m_aDirUp,SIGNAL(activated()),m_view,SLOT(slotDirUp()));

    if ( Ir::supported() ) {
        m_aBeam = new QAction( tr( "Beam file" ), Resource::loadIconSet( "beam" ),0, 0, this, 0, true );
        m_aBeam->setToggleAction(false);
        connect(m_aBeam,SIGNAL(activated()),m_view,SLOT(slotBeam()));
    } else {
        m_aBeam = 0;
    }

    m_aShowInfo = new QAction( tr( "Show imageinfo" ), Resource::loadIconSet( "edit" ), 0, 0, this, 0, true );
    m_aShowInfo->setToggleAction(false);
    connect(m_aShowInfo,SIGNAL(activated()),m_view,SLOT(slotImageInfo()));

    m_aTrash = new QAction( tr( "Delete file" ), Resource::loadIconSet("trash"), 0, 0, this, 0, true );
    m_aTrash->setToggleAction(false);
    connect(m_aTrash,SIGNAL(activated()),m_view,SLOT(slotTrash()));

    m_aViewfile = new QAction( tr( "Display image" ), Resource::loadIconSet("mag"), 0, 0, this, 0, true );
    m_aViewfile->setToggleAction(false);
    connect(m_aViewfile,SIGNAL(activated()),m_view,SLOT(slotShowImage()));

    m_aStartSlide = new QAction( tr( "Start slideshow" ), Resource::loadIconSet("play"),0, 0, this, 0, true );
    m_aStartSlide->setToggleAction(false);
    connect(m_aStartSlide,SIGNAL(activated()),m_view,SLOT(slotStartSlide()));

    m_aHideToolbar = new QAction( tr( "Show toolbar" ), Resource::loadIconSet( "UtilsIcon" ), 0, 0, this, 0, true );
    m_aHideToolbar->setOn (true);
    connect(m_aHideToolbar,SIGNAL(toggled(bool)),this,SLOT(showToolbar(bool)));

    m_aSetup = new QAction( tr( "Settings" ), Resource::loadIconSet("SettingsIcon"), 0, 0, this, 0, true );
    m_aSetup->setToggleAction(false);
    connect(m_aSetup,SIGNAL(activated()),this,SLOT(slotConfig()));

    m_gListViewMode = new QActionGroup(this,"Select listmode",true);
    connect(m_gListViewMode,SIGNAL(selected(QAction*)),this,SLOT(listviewselected(QAction*)));

    m_aDirLong = new QAction( tr( "Thumbnail and Imageinfo" ),Resource::loadIconSet("opie-eye/opie-eye-thumb"), 0, 0, this, 0, true );
    m_aDirLong->setToggleAction(true);
    m_aDirShort = new QAction( tr( "Thumbnail and name" ),Resource::loadIconSet("opie-eye/opie-eye-thumbonly"), 0, 0, this, 0, true );
    m_aDirShort->setToggleAction(true);
    m_aDirName = new QAction( tr( "Name only" ), Resource::loadIconSet("opie-eye/opie-eye-textview"),0, 0, this, 0, true );
    m_aDirName->setToggleAction(true);
    int mode = m_cfg->readNumEntry("ListViewMode", 1);
    if (mode < 1 || mode>3) mode = 1;
    switch (mode) {
        case 3:
            m_aDirName->setOn(true);
            break;
        case 2:
            m_aDirShort->setOn(true);
            break;
        case 1:
        default:
            m_aDirLong->setOn(true);
    }
    m_gListViewMode->insert(m_aDirLong);
    m_gListViewMode->insert(m_aDirShort);
    m_gListViewMode->insert(m_aDirName);

    m_gPrevNext = new QActionGroup(this,"imageprevnext",false);
    m_aNext = new QAction( tr( "Next image" ), Resource::loadIconSet("forward"), 0, 0, this, 0, true );
    m_aNext->setToggleAction(false);
    connect(m_aNext,SIGNAL(activated()),m_view,SLOT(slotShowNext()));
    m_aPrevious = new QAction( tr( "Previous image" ), Resource::loadIconSet("back"), 0, 0, this, 0, true );
    m_aPrevious->setToggleAction(false);
    connect(m_aPrevious,SIGNAL(activated()),m_view,SLOT(slotShowPrev()));
    m_gPrevNext->insert(m_aPrevious);
    m_gPrevNext->insert(m_aNext);

    m_aFullScreen = new QAction( tr( "Show images fullscreen" ),
        Resource::loadIconSet("fullscreen"), 0, 0, this, 0, true );
    m_aFullScreen->setToggleAction(true);
    if (autoSave) {
        m_aFullScreen->setOn(m_cfg->readBoolEntry("fullscreen",false));
    } else {
        m_aFullScreen->setOn(false);
    }
    connect(m_aFullScreen,SIGNAL(toggled(bool)),this,SLOT(slotFullScreenButton(bool)));

    m_gDisplayType = new QActionGroup(this,"imagedisplaytype",false);
    m_aAutoRotate = new QAction( tr( "Auto rotate images" ), Resource::loadIconSet( "rotate" ), 0, 0, this, 0, true );
    m_aAutoRotate->setToggleAction(true);

    if (m_stack->mode() == Opie::Ui::OWidgetStack::SmallScreen) {
        m_aAutoRotate->setOn(true);
    } else {
        m_aAutoRotate->setOn(false);
    }
    if (autoSave) {
        m_aAutoRotate->setOn(m_cfg->readBoolEntry("autorotate",m_aAutoRotate->isOn()));
    }
    connect(m_aAutoRotate,SIGNAL(toggled(bool)),this,SLOT(slotRotateToggled(bool)));

    m_aUnscaled = new QAction( tr( "Show images unscaled" ), Resource::loadIconSet( "1to1" ), 0, 0, this, 0, true );
    m_aUnscaled->setToggleAction(true);
    connect(m_aUnscaled,SIGNAL(toggled(bool)),this,SLOT(slotScaleToggled(bool)));
    if (autoSave) {
        m_aUnscaled->setOn(m_cfg->readBoolEntry("unscaled",false));
    } else {
        m_aUnscaled->setOn(false);
    }

    m_aZoomer = new QAction( tr( "Show zoomer window when unscaled" ), Resource::loadIconSet( "mag" ), 0, 0, this, 0, true );
    m_aZoomer->setToggleAction(true);
    if (autoSave) {
        m_aZoomer->setOn(m_cfg->readBoolEntry("zoomeron",true));
    } else {
        m_aZoomer->setOn (true);
    }
    connect(m_aZoomer,SIGNAL(toggled(bool)),this,SLOT(slotZoomerToggled(bool)));
    m_gDisplayType->insert(m_aAutoRotate);
    m_gDisplayType->insert(m_aUnscaled);
    m_gDisplayType->insert(m_aZoomer);

    m_hGroup = new QActionGroup(this,"actioncollection",false);
    m_hGroup->insert(m_aFullScreen);

    if (!m_SmallWindow) {
        m_aForceSmall = new QAction(tr("Dont show seperate windows"),Resource::loadIconSet( "AppsIcon" ), 0, 0, this, 0, true);
        m_aForceSmall->setToggleAction(true);
        connect(m_aForceSmall,SIGNAL(toggled(bool)),this,SLOT(slotForceSmall(bool)));
    } else {
        m_aForceSmall = 0;
    }
}

void PMainWindow::setupToolbar()
{
    toolBar = new QToolBar( this );
    addToolBar(toolBar);
    toolBar->setHorizontalStretchable( true );
    setToolBarsMovable( false );
    m_aDirUp->addTo( toolBar );

    fsButton = new PFileSystem( toolBar );
    connect( fsButton, SIGNAL( changeDir( const QString& ) ),
             m_view, SLOT(slotChangeDir( const QString& ) ) );
    connect( this, SIGNAL( changeDir( const QString& ) ),
             m_view, SLOT(slotChangeDir( const QString& ) ) );

    if (m_aBeam) {
        m_aBeam->addTo( toolBar );
    }
    m_aShowInfo->addTo(toolBar);
    m_aTrash->addTo(toolBar);
//    m_aSetup->addTo(toolBar);

    m_gDisplayType->addTo(toolBar);

    if (!m_SmallWindow) {
        m_gPrevNext->addTo(toolBar);
    } else {
        m_gPrevNext->setEnabled(false);
    }
}

void PMainWindow::setupMenu()
{
    fileMenu = new QPopupMenu( menuBar() );
    menuBar()->insertItem( tr( "File" ), fileMenu );
    dispMenu = new QPopupMenu( menuBar() );
    menuBar()->insertItem( tr( "Show" ), dispMenu );
    settingsMenu = new QPopupMenu( menuBar() );
    menuBar()->insertItem( tr( "Settings" ), settingsMenu );

    m_aViewfile->addTo(fileMenu);
    m_aShowInfo->addTo(fileMenu);
    m_aStartSlide->addTo(fileMenu);

    fileMenu->insertSeparator();
    m_aDirUp->addTo( fileMenu );

    fsMenu = new QPopupMenu(fileMenu);
    fileMenu->insertItem(Resource::loadIconSet( "cardmon/pcmcia" ),tr("Select filesystem"),fsMenu);
    connect( fsMenu, SIGNAL( activated( int ) ), this, SLOT(slotSelectDir( int ) ) );
    dirChanged();

    if ( m_aBeam ) {
        fileMenu->insertSeparator();
        m_aBeam->addTo( fileMenu );
    }
    fileMenu->insertSeparator();
    m_aTrash->addTo(fileMenu);

    listviewMenu = new QPopupMenu(dispMenu);
    dispMenu->insertItem(Resource::loadIconSet("opie-eye/opie-eye-thumb"),tr("Listview mode"),listviewMenu);
    m_gListViewMode->addTo(listviewMenu);
    dispMenu->insertSeparator();
    m_aFullScreen->addTo(dispMenu);
    m_gDisplayType->addTo(dispMenu);
    dispMenu->insertSeparator();
    m_gPrevNext->addTo(dispMenu);
    if (m_aForceSmall) {
        dispMenu->insertSeparator();
        m_aForceSmall->addTo(dispMenu);
    }

    m_aSetup->addTo(settingsMenu);
    m_aHideToolbar->addTo(settingsMenu);
}

void PMainWindow::listviewselected(QAction*which)
{
    if (!which || which->isOn()==false) return;
    int val = 1;

    if (which==m_aDirName) {
        val = 3;
    } else if (which==m_aDirShort) {
        val = 2;
    } else if (which==m_aDirLong) {
        val = 1;
    }
    emit changeListMode(val);
}

void PMainWindow::readConfig()
{
    autoSave =m_cfg->readBoolEntry("savestatus",true);
}

void PMainWindow::polish()
{
    if (m_disp) {
        odebug << "======================\n"
               << "Called via setdocument\n"
               << "======================" << oendl;
        m_setDocCalled = true;
        m_view->setDoccalled(true);
        m_disp->setCloseIfHide(true);
    } else {
        m_setDocCalled = false;
        m_view->setDoccalled(false);
    }
    m_polishDone = true;
    QMainWindow::polish();
    if (m_setDocCalled) {
        if (m_aFullScreen->isOn()) {
            QTimer::singleShot(0,this,SLOT(check_view_fullscreen()));
        } else if (m_stack->mode() != Opie::Ui::OWidgetStack::SmallScreen) {
        }
    }
}

void PMainWindow::slotForceSmall(bool how)
{
    odebug << "Disable separate windows: " << how << oendl;
    if (m_stack) {
        if (how) {
            m_stack->forceMode(Opie::Ui::OWidgetStack::SmallScreen);
        } else {
            m_stack->forceMode(Opie::Ui::OWidgetStack::NoForce);
        }
    }
    if (autoSave) {
        m_cfg->writeEntry("dontshowseperate",how);
    }
}
