#include <qiconset.h>
#include <qdialog.h>
#include <qpixmap.h>
#include <qdom.h>
#include <qaction.h>
#include <qpe/qpemenubar.h>
#include <qstatusbar.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qpe/qpetoolbar.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qpe/qpeapplication.h>
#include <qpe/global.h>
#include <assert.h>

#include "kvnc.h"
#include "krfbcanvas.h"
#include "kvncoptionsdlg.h"
#include "krfbconnection.h"


/* XPM */
static char * menu_xpm[] = {
"12 12 5 1",
" 	c None",
".	c #000000",
"+	c #FFFDAD",
"@	c #FFFF00",
"#	c #E5E100",
"            ",
"            ",
"  ......... ",
"  .+++++++. ",
"  .+@@@@#.  ",
"  .+@@@#.   ",
"  .+@@#.    ",
"  .+@#.     ",
"  .+#.      ",
"  .+.       ",
"  ..        ",
"            "};

const int StatusTextId = 0;

KVNC::KVNC( const char *name ) : QMainWindow( 0, name )
{
    setCaption( tr("VNC Viewer") );
    fullscreen = false;

    canvas = new KRFBCanvas( this, "canvas" );
    setCentralWidget( canvas );

    connect( canvas->connection(), SIGNAL(statusChanged(const QString &)),
	    this, SLOT(statusMessage(const QString &)) );
    connect( canvas->connection(), SIGNAL(error(const QString &)),
	    this, SLOT(error(const QString &)) );
    connect( canvas->connection(), SIGNAL(connected()), this, SLOT(connected()) );
    connect( canvas->connection(), SIGNAL(loggedIn()), this, SLOT(loggedIn()) );
    connect( canvas->connection(), SIGNAL(disconnected()), this, SLOT(disconnected()) );

    setupActions();

    cornerButton = new QPushButton( this );
    cornerButton->setPixmap( QPixmap( (const char**)menu_xpm ) );
    connect( cornerButton, SIGNAL(pressed()), this, SLOT(showMenu()) );
    canvas->setCornerWidget( cornerButton );

    QTimer::singleShot( 0, canvas, SLOT(openConnection()) );
}

KVNC::~KVNC()
{
}

void KVNC::openURL( const QUrl &url )
{
    canvas->openURL( url );
}

void KVNC::setupActions()
{
    cornerMenu = new QPopupMenu( this );

    fullScreenAction = new QAction( tr("Full Screen"), QString::null, 0, 0 );
    connect( fullScreenAction, SIGNAL(activated()),
	    this, SLOT( toggleFullScreen() ) );
    fullScreenAction->addTo( cornerMenu );
    fullScreenAction->setEnabled( false );

    optionsAction = new QAction( tr("Settings"), QString::null, 0, 0 );
    connect( optionsAction, SIGNAL(activated()), this, SLOT( showOptions() ) );
    optionsAction->addTo( cornerMenu );

    connectAction = new QAction( tr("Connect..."), QString::null, 0, 0 );
    connect( connectAction, SIGNAL(activated()),
	    canvas, SLOT( openConnection() ) );
    connectAction->addTo( cornerMenu );

    disconnectAction = new QAction( tr("Disconnect"), QString::null, 0, 0 );
    connect( disconnectAction, SIGNAL(activated()),
	    this, SLOT( closeConnection() ) );
    disconnectAction->addTo( cornerMenu );
    disconnectAction->setEnabled( false );
}

void KVNC::toggleFullScreen()
{
    if ( fullscreen ) {
	canvas->releaseKeyboard();
	canvas->reparent( this, 0, QPoint(0,0), false );
	canvas->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	setCentralWidget( canvas );
	canvas->show();
	fullScreenAction->setText( tr("Full Screen") );
    } else {
	canvas->setFrameStyle( QFrame::NoFrame );
	canvas->reparent( 0,WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop,
		QPoint(0,0),false);
	canvas->resize(qApp->desktop()->width(), qApp->desktop()->height());
	canvas->raise();
	canvas->setFocus();
	canvas->grabKeyboard();
	canvas->show();

	fullScreenAction->setText( tr("Stop Full Screen") );
    }

    fullscreen = !fullscreen;
}

void KVNC::closeConnection()
{
    if ( fullscreen )
	toggleFullScreen();
    canvas->closeConnection();
}

void KVNC::showMenu()
{
    QPoint pt = mapToGlobal(cornerButton->pos());
    QSize s = cornerMenu->sizeHint();
    pt.ry() -= s.height();
    pt.rx() -= s.width();
    cornerMenu->popup( pt );
}

void KVNC::connected()
{
    static QString msg = tr( "Connected to remote host" );
    statusMessage( msg );
    connectAction->setEnabled( false );
    disconnectAction->setEnabled( true );
    fullScreenAction->setEnabled( true );
}

void KVNC::loggedIn()
{
    static QString msg = tr( "Logged in to remote host" );
    statusMessage( msg );
}

void KVNC::disconnected()
{
    static QString msg = tr( "Connection closed" );
    statusMessage( msg );
    connectAction->setEnabled( true );
    disconnectAction->setEnabled( false );
    fullScreenAction->setEnabled( false );
}

void KVNC::statusMessage( const QString &m )
{
    Global::statusMessage( m );
}

void KVNC::error( const QString &msg )
{
    statusMessage( msg );
    QMessageBox::warning( this, tr("VNC Viewer"), msg );
}

void KVNC::showOptions()
{
    KVNCOptionsDlg *wdg = new KVNCOptionsDlg( canvas->connection()->options(), this );
    wdg->showMaximized();
    wdg->exec();
    delete wdg;
}

