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
    setCaption( QObject::tr("Opie Eye Caramba" ) );
    m_cfg = new Opie::Core::OConfig("phunkview");
    m_cfg->setGroup("Zecke_view" );

    m_storage = new StorageInfo();
    connect(m_storage, SIGNAL(disksChanged() ),
            this, SLOT( dirChanged() ) );

    /*
     * Initialize ToolBar and IconView
     * And Connect Them
     */
    toolBar = new QToolBar( this );
    menuBar = new QMenuBar( this );
    fileMenu = new QPopupMenu( menuBar );
    menuBar->insertItem( tr( "File" ), fileMenu );
    dispMenu = new QPopupMenu( menuBar );
    menuBar->insertItem( tr( "Show" ), dispMenu );

    addToolBar(toolBar);
    toolBar->setHorizontalStretchable( true );
    setToolBarsMovable( false );

    m_stack = new Opie::Ui::OWidgetStack( this );
    setCentralWidget( m_stack );

    m_view = new PIconView( m_stack, m_cfg );
    m_stack->addWidget( m_view, IconView );
    m_stack->raiseWidget( IconView );

    connect(m_view, SIGNAL(sig_display(const QString&)),
            this, SLOT(slotDisplay(const QString&)));
    connect(m_view, SIGNAL(sig_showInfo(const QString&)),
            this, SLOT(slotShowInfo(const QString&)) );

    m_stack->forceMode(Opie::Ui::OWidgetStack::NoForce);

    m_aDirUp = new QAction( tr( "Go dir up" ), Resource::loadIconSet( "up" ),
                               0, 0, this, 0, true );
    m_aDirUp->setToggleAction(false);
    connect(m_aDirUp,SIGNAL(activated()),m_view,SLOT(slotDirUp()));
    m_aDirUp->addTo( toolBar );

    QToolButton*btn;

    fsButton = new PFileSystem( toolBar );
    connect( fsButton, SIGNAL( changeDir( const QString& ) ),
             m_view, SLOT(slotChangeDir( const QString& ) ) );
    connect( this, SIGNAL( changeDir( const QString& ) ),
             m_view, SLOT(slotChangeDir( const QString& ) ) );

    m_aShowInfo = new QAction( tr( "Show imageinfo" ), Resource::loadIconSet( "edit" ),
                               0, 0, this, 0, true );
    m_aShowInfo->setToggleAction(false);
    connect(m_aShowInfo,SIGNAL(activated()),m_view,SLOT(slotImageInfo()));
    m_aShowInfo->addTo( toolBar );

    if ( Ir::supported() ) {
        m_aBeam = new QAction( tr( "Beam file" ), Resource::loadIconSet( "beam" ),
                               0, 0, this, 0, true );
        m_aBeam->setToggleAction(false);
        connect(m_aBeam,SIGNAL(activated()),m_view,SLOT(slotBeam()));
        m_aBeam->addTo( toolBar );
    }
    m_aTrash = new QAction( tr( "Delete file" ), Resource::loadIconSet( "trash" ),
                               0, 0, this, 0, true );
    m_aTrash->setToggleAction(false);
    connect(m_aTrash,SIGNAL(activated()),m_view,SLOT(slotTrash()));
    m_aTrash->addTo( toolBar );

    m_aViewfile = new QAction( tr( "Display image" ), Resource::loadIconSet( "mag" ),
                               0, 0, this, 0, true );
    m_aViewfile->setToggleAction(false);
    connect(m_aViewfile,SIGNAL(activated()),m_view,SLOT(slotShowImage()));

    m_aStartSlide = new QAction( tr( "Start slideshow" ), Resource::loadIconSet( "play" ),
                               0, 0, this, 0, true );
    m_aStartSlide->setToggleAction(false);

    connect(m_aStartSlide,SIGNAL(activated()),m_view,SLOT(slotStartSlide()));


    int mode = m_cfg->readNumEntry("ListViewMode", 1);
    if (mode < 1 || mode>3) mode = 1;
    viewModeButton = new ViewModeButton( toolBar,mode );
    connect( viewModeButton, SIGNAL(changeMode(int)),
             m_view, SLOT(slotChangeMode(int)));

    btn = new QToolButton( toolBar );
    btn->setIconSet( Resource::loadIconSet( "SettingsIcon" ) );
    connect( btn, SIGNAL(clicked() ),
             this, SLOT(slotConfig() ) );

    prevButton = new QToolButton(toolBar);
    prevButton->setIconSet( Resource::loadIconSet( "back" ) );
    connect(prevButton,SIGNAL(clicked()),m_view,SLOT(slotShowPrev()));

    nextButton = new QToolButton(toolBar);
    nextButton->setIconSet( Resource::loadIconSet( "forward" ) );
    connect(nextButton,SIGNAL(clicked()),m_view,SLOT(slotShowNext()));

/* filemenu start */
    m_aViewfile->addTo(fileMenu);
    m_aShowInfo->addTo(fileMenu);
    m_aStartSlide->addTo(fileMenu);

    fileMenu->insertSeparator();
    m_aDirUp->addTo( fileMenu );

    fsMenu = new QPopupMenu(fileMenu);
    fileMenu->insertItem(Resource::loadIconSet( "cardmon/pcmcia" ),tr("Select filesystem"),fsMenu);
    connect( fsMenu, SIGNAL( activated( int ) ),
             this, SLOT(slotSelectDir( int ) ) );
    dirChanged();

    fileMenu->insertSeparator();
    if ( Ir::supported() ) {
        m_aBeam->addTo( fileMenu );
    }
    fileMenu->insertSeparator();
    m_aTrash->addTo( fileMenu );
/* filemenu end */

    rotateButton = new QToolButton(toolBar);
    rotateButton->setIconSet( Resource::loadIconSet( "rotate" ) );
    rotateButton->setToggleButton(true);
    if (m_stack->mode() == Opie::Ui::OWidgetStack::SmallScreen) {
        rotateButton->setOn(true);
        autoRotate = true;
        prevButton->hide();
        nextButton->hide();
    } else {
        rotateButton->setOn(false);
        autoRotate = false;
    }
    connect(rotateButton,SIGNAL(toggled(bool)),this,SLOT(slotRotateToggled(bool)));

    scaleButton = new QToolButton(toolBar);
    scaleButton->setIconSet( Resource::loadIconSet( "1to1" ) );
    scaleButton->setToggleButton(true);
    scaleButton->setOn(false);
    connect(scaleButton,SIGNAL(toggled(bool)),this,SLOT(slotScaleToggled(bool)));
    autoScale = true;

    zoomButton = new QToolButton(toolBar);
    zoomButton->setIconSet( Resource::loadIconSet( "mag" ) );
    zoomButton->setToggleButton(true);
    zoomButton->setOn(true);
    connect(zoomButton,SIGNAL(toggled(bool)),this,SLOT(slotZoomerToggled(bool)));
    zoomerOn = true;

/* showmenu */
    m_aHideToolbar = new QAction( tr( "Show toolbar" ), Resource::loadIconSet( "UtilsIcon" ),
            0, 0, this, 0, true );
    m_aHideToolbar->setOn (true);
    m_aHideToolbar->addTo(dispMenu);
    connect(m_aHideToolbar,SIGNAL(toggled(bool)),this,SLOT(showToolbar(bool)));
/* showmenu end*/

}

PMainWindow::~PMainWindow() {
}

void PMainWindow::slotToggleZoomer()
{
    if (!m_disp) return;
    bool cur = zoomButton->isOn();
    zoomButton->setOn(!cur);
}

void PMainWindow::slotZoomerToggled(bool how)
{
    zoomerOn = how;
    if (m_disp) {
        m_disp->setShowZoomer(zoomerOn);
    }
}

void PMainWindow::slotToggleAutorotate()
{
    if (!m_disp) return;
    if (!rotateButton->isEnabled()) return;
    bool cur = rotateButton->isOn();
    rotateButton->setOn(!cur);
}

void PMainWindow::slotToggleAutoscale()
{
    if (!m_disp) return;
    bool cur = scaleButton->isOn();
    scaleButton->setOn(!cur);
}

void PMainWindow::slotRotateToggled(bool how)
{
    odebug << "Autorotate: " << how << oendl;
    autoRotate = how;
    if (m_disp) {
        m_disp->setAutoScaleRotate(autoScale,autoRotate);
    }
}

void PMainWindow::slotScaleToggled(bool how)
{
    autoScale = !how;
    if (!how) {
        autoRotate = how;
    }
    if (m_disp) {
        m_disp->setAutoScaleRotate(autoScale,autoRotate);
    }
    if (!autoScale) {
        rotateButton->setOn(false);
    }
    rotateButton->setEnabled(!how);
}

void PMainWindow::slotConfig() {
   /*
    * have a tab with the possible views
    * a tab for globals image cache size.. scaled loading
    * and one tab for the  KeyConfigs
    */
    QDialog dlg(this, 0, true);
    dlg.setCaption( tr("Phunk View - Config" ) );

    QHBoxLayout *lay = new QHBoxLayout(&dlg);
    Opie::Ui::OTabWidget *wid = new Opie::Ui::OTabWidget(&dlg );
    lay->addWidget( wid );
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

    if ( !m_info ) {
        initInfo();
    }
    keyWid->insert( tr("Imageinfo Keyboard Actions"), m_info->manager() );

    if ( !m_disp ) {
        initDisp();
    }
    keyWid->insert( tr("Imageview Keyboard Actions"), m_disp->manager() );

    keyWid->load();
    wid->addTab( keyWid, QString::fromLatin1("AppsIcon" ), tr("Keyboard Configuration") );


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
        m_view->resetView();
        keyWid->save();
        m_disp->manager()->save();
        m_info->manager()->save();
        m_view->manager()->save();
    }
    delete keyWid;
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
        if (m_stack->mode() != Opie::Ui::OWidgetStack::SmallScreen) {
            m_disp->setMinimumSize(QApplication::desktop()->size()/2);
        }
        m_disp->setAutoScale(autoScale);
        m_disp->setAutoRotate(autoRotate);
        m_disp->setShowZoomer(zoomerOn);
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
    }
}

void PMainWindow::slotToggleFullScreen()
{
    odebug << "Toggle full " << oendl;
    if (!m_disp) return;
    bool current = !m_disp->fullScreen();
    odebug << "Current = " << current << oendl;
    if (current) {
        odebug << "full" << oendl;
        m_disp->setBackgroundColor(black);
        m_disp->reparent(0, WStyle_Customize | WStyle_NoBorder, QPoint(0,0));
        m_disp->setVScrollBarMode(QScrollView::AlwaysOff);
        m_disp->setHScrollBarMode(QScrollView::AlwaysOff);
        m_disp->resize(qApp->desktop()->width(), qApp->desktop()->height());
        //m_disp->showFullScreen();
        //qwsDisplay()->requestFocus( m_disp->winId(), TRUE);
    } else {
        setUpdatesEnabled(false);
        odebug << "window" << oendl;
        m_disp->reparent(0,QPoint(0,0));
        m_disp->showNormal();
        m_disp->setBackgroundColor(white);
        m_stack->addWidget(m_disp,ImageDisplay);
        m_disp->setVScrollBarMode(QScrollView::Auto);
        m_disp->setHScrollBarMode(QScrollView::Auto);
        m_stack->raiseWidget(m_disp);
        if (m_stack->mode() != Opie::Ui::OWidgetStack::SmallScreen) {
            m_disp->resize(m_disp->minimumSize());
        }
        setUpdatesEnabled(true);
    }
    m_disp->setFullScreen(current);
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
    if (m_stack->mode() == Opie::Ui::OWidgetStack::SmallScreen) {
        prevButton->hide();
        nextButton->hide();
        m_aDirUp->setEnabled(false);
        m_aShowInfo->setEnabled(false);
        m_aViewfile->setEnabled(true);
        m_aStartSlide->setEnabled(false);
        fsButton->hide();
        viewModeButton->hide();
    }
    m_stack->raiseWidget( ImageInfo );
}

void PMainWindow::slotDisplay( const QString& inf ) {
    odebug << "slotDisplay: " << inf << oendl;
    if ( !m_disp ) {
        initDisp();
    }
    m_disp->setImage( inf );
    if (m_stack->mode() == Opie::Ui::OWidgetStack::SmallScreen) {
        prevButton->show();
        nextButton->show();
        m_aDirUp->setEnabled(false);
        m_aShowInfo->setEnabled(true);
        m_aViewfile->setEnabled(false);
        m_aStartSlide->setEnabled(false);
        fsButton->hide();
        viewModeButton->hide();
    }
    if (m_disp->fullScreen()) {
        //m_disp->showFullScreen();
        qwsDisplay()->requestFocus( m_disp->winId(), TRUE);
    } else {
        m_stack->raiseWidget( ImageDisplay );
    }
}

void PMainWindow::slotReturn() {
    raiseIconView();
}


void PMainWindow::closeEvent( QCloseEvent* ev ) {
    /*
     * return from view
     * or properly quit
     */
    if ( m_stack->visibleWidget() == m_info ||
         m_stack->visibleWidget() == m_disp ) {
        ev->ignore();
        raiseIconView();
        return;
    }
    if (m_disp && m_disp->fullScreen()) {
        /* otherwise opie-eye crashes in bigscreen mode! */
        m_disp->reparent(0,QPoint(0,0));
        m_stack->addWidget(m_disp,ImageDisplay);
    }
    ev->accept();
    QTimer::singleShot(0, qApp, SLOT(closeAllWindows()));
}

void PMainWindow::raiseIconView() {
    setUpdatesEnabled(false);
    if (m_stack->mode() == Opie::Ui::OWidgetStack::SmallScreen) {
        prevButton->hide();
        nextButton->hide();
        m_aDirUp->setEnabled(true);
        m_aShowInfo->setEnabled(true);
        m_aViewfile->setEnabled(true);
        m_aStartSlide->setEnabled(true);
        fsButton->show();
        viewModeButton->show();
    }
    if (m_disp && m_disp->fullScreen() && m_disp->isVisible()) {
        m_disp->hide();
    }
    m_stack->raiseWidget( IconView );
    setUpdatesEnabled(true);
    repaint();
}

void PMainWindow::setDocument( const QString& showImg ) {
    QString file = showImg;
    DocLnk lnk(showImg);
    if (lnk.isValid() )
        file = lnk.file();

    slotDisplay( file );
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
}