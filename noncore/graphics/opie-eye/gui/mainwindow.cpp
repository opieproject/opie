/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */
#include "mainwindow.h"

#include "iconview.h"
#include "filesystem.h"
#include "imageinfoui.h"
#include "imagescrollview.h"

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

#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qmap.h>
#include <qtimer.h>




OPIE_EXPORT_APP( Opie::Core::OApplicationFactory<PMainWindow> )

PMainWindow::PMainWindow(QWidget* wid, const char* name, WFlags style)
    : QMainWindow( wid, name, style ), m_info( 0 ), m_disp( 0 )
{
    setCaption( QObject::tr("Opie Eye Caramba" ) );
    m_cfg = new Opie::Core::OConfig("phunkview");
    m_cfg->setGroup("Zecke_view" );


    /*
     * Initialize ToolBar and IconView
     * And Connect Them
     */
    QToolBar *bar = new QToolBar( this );
    bar->setHorizontalStretchable( true );
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

    QToolButton *btn = new QToolButton( bar );
    btn->setIconSet(  Resource::loadIconSet( "up" ) );
    connect( btn, SIGNAL(clicked()),
             m_view, SLOT(slotDirUp()) );

    btn = new PFileSystem( bar );
    connect( btn, SIGNAL( changeDir( const QString& ) ),
             m_view, SLOT(slotChangeDir( const QString& ) ) );

    btn = new QToolButton( bar );
    btn->setIconSet( Resource::loadIconSet( "edit" ) );
    connect( btn, SIGNAL(clicked()),
             m_view, SLOT(slotRename()) );

    if ( Ir::supported() ) {
        btn = new QToolButton( bar );
        btn->setIconSet( Resource::loadIconSet( "beam" ) );
        connect( btn, SIGNAL(clicked()),
                 m_view, SLOT(slotBeam()) );
    }

    btn = new QToolButton( bar );
    btn->setIconSet( Resource::loadIconSet( "trash" ) );
    connect( btn, SIGNAL(clicked() ),
             m_view, SLOT(slotTrash() ) );

    btn = new QToolButton( bar );
    btn->setIconSet( Resource::loadIconSet( "SettingsIcon" ) );
    connect( btn, SIGNAL(clicked() ),
             this, SLOT(slotConfig() ) );

}

PMainWindow::~PMainWindow() {
    odebug << "Shutting down" << oendl;
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
    *ptr = new T( m_stack, name );
    m_stack->addWidget( *ptr, id );

    connect(*ptr, SIGNAL(sig_return()),
            this,SLOT(slotReturn()));

}
void PMainWindow::initInfo() {
    initT<imageinfo>( "Image Info", &m_info, ImageInfo );
}
void PMainWindow::initDisp() {
    initT<ImageScrollView>( "Image ScrollView", &m_disp, ImageDisplay );
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
    if ( !m_info )
        initInfo();
    m_info->setPath( inf );
    m_stack->raiseWidget( ImageInfo );
}

void PMainWindow::slotDisplay( const QString& inf ) {
    if ( !m_disp )
        initDisp();
    m_disp->setImage( inf );
    m_stack->raiseWidget( ImageDisplay );
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
        raiseIconView();
        ev->ignore();
        return;
    }
    ev->accept();
    QTimer::singleShot(0, qApp, SLOT(closeAllWindows()));
}

void PMainWindow::raiseIconView() {
    m_stack->raiseWidget( IconView );
}
