#include <qlayout.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include "mainwindow.h"
#include "linphoneconfig.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags flags )
: QMainWindow( parent, name, flags ) {

    setCaption( tr( "Sip Phone" ) );
    setToolBarsMovable( false );

    toolBar = new QToolBar( this );
    toolBar->setHorizontalStretchable( true );
    menuBar = new QMenuBar( toolBar );
    prefMenu = new QPopupMenu( menuBar );
    menuBar->insertItem( tr( "Connection" ), prefMenu );

    settings = new QAction( tr("Settings"), QIconSet( Resource::loadPixmap("SettingsIcon") ), 0, 0, this);
    settings->addTo( prefMenu );
    connect( settings, SIGNAL( activated() ),
             SLOT( slotSettings() ) );

    mainWidget = new  QLinphoneMainWidget( this, "qlinphone", WStyle_ContextHelp );
    setCentralWidget( mainWidget );
}


MainWindow::~MainWindow() {}

void MainWindow::slotSettings() {
    LinPhoneConfig settings( this,  0, true,  WStyle_ContextHelp );
    QPEApplication::execDialog( &settings );
    // FIXME - only in OK case
    mainWidget->createLinphoneCore();
}
