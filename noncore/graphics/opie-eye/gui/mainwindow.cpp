/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */
#include "mainwindow.h"

#include "iconview.h"
#include "filesystem.h"

#include <iface/ifaceinfo.h>
#include <iface/dirview.h>

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





OPIE_EXPORT_APP( Opie::Core::OApplicationFactory<PMainWindow> )

PMainWindow::PMainWindow(QWidget* wid, const char* name, WFlags style)
    : QMainWindow( wid, name, style )
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

    m_view = new PIconView( this, m_cfg );
    setCentralWidget( m_view );

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
