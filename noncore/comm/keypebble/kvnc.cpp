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
#include <qspinbox.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qpe/qpeapplication.h>
#include <qpe/global.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <assert.h>

#include "kvnc.h"
#include "krfbcanvas.h"
#include "krfbconnection.h"
#include "kvncconndlg.h"
#include "krfbserver.h"

static int u_id = 1;
static int get_unique_id() 
{   
		return u_id++;
}


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

KVNC::KVNC( const char *name ) : QMainWindow( 0, name ,WStyle_ContextHelp)
{
    setCaption( tr("VNC Viewer") );
    fullscreen = false;

		stack = new QWidgetStack( this );
		setCentralWidget( stack );

		bookmarkSelector=new KVNCBookmarkDlg();
		stack->addWidget(bookmarkSelector,get_unique_id());
		stack->raiseWidget( bookmarkSelector );                                        

		canvas = new KRFBCanvas( stack, "canvas" );
		stack->addWidget(canvas,get_unique_id());
    setCentralWidget( stack );


    connect( bookmarkSelector->bookmarkList, SIGNAL(clicked(QListBoxItem *)),
	    this, SLOT(openConnection(QListBoxItem *)) );
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

		stack->raiseWidget( bookmarkSelector );


		bar= new QToolBar( this );
		setToolBarsMovable( false );
		setRightJustification(false);


		 QAction *n = new QAction( tr( "New Connection" ), Resource::loadPixmap( "new" ),
				QString::null, 0, this, 0 );
		 connect( n, SIGNAL( activated() ),
						this, SLOT( newConnection() ) );
		n->addTo( bar );
								     
		 QAction *o = new QAction( tr( "Open Bookmark" ), Resource::loadPixmap( "fileopen" ),
				QString::null, 0, this, 0 );
		 connect( o, SIGNAL( activated() ),
						this, SLOT( openConnection() ) );
		o->addTo( bar );

		 QAction *d = new QAction( tr( "Delete Bookmark" ), Resource::loadPixmap( "trash" ),
				QString::null, 0, this, 0 );
		 connect( d, SIGNAL( activated() ),
						this, SLOT( deleteBookmark() ) );
		d->addTo( bar );
}

KVNC::~KVNC()
{

}

void KVNC::newConnection()
{
    curServer=new KRFBServer;

		KVNCConnDlg dlg( curServer,this);
		dlg.showMaximized();
		if (	dlg.exec()) {
				if (!curServer->name.isEmpty())
						bookmarkSelector->addBookmark(curServer);
				canvas->openConnection(*curServer);
		} else
				curServer=0;
}

void KVNC::openConnection( QString name)
{
    curServer=bookmarkSelector->getServer(name);

		if (curServer) {
				KVNCConnDlg dlg( curServer,this);
			dlg.showMaximized();

			if ( dlg.exec() ) {
					canvas->openConnection(*curServer);
					bookmarkSelector->writeBookmarks();
			} else
					curServer=0;
		}
}

void KVNC::openConnection( void )
{
		openConnection( bookmarkSelector->selectedBookmark());
}

void KVNC::openConnection( QListBoxItem * item)
{
    if (item)
  		openConnection(item->text());
}

void KVNC::setupActions()
{
    cornerMenu = new QPopupMenu( this );

    fullScreenAction = new QAction( tr("Full Screen"), QString::null, 0, 0 );
    connect( fullScreenAction, SIGNAL(activated()),
	    this, SLOT( toggleFullScreen() ) );
    fullScreenAction->addTo( cornerMenu );
    fullScreenAction->setEnabled( false );

    ctlAltDelAction = new QAction( tr("Send Contrl-Alt-Delete"), QString::null, 0, 0 );
    connect( ctlAltDelAction, SIGNAL(activated()),
	    canvas, SLOT( sendCtlAltDel() ) );
    ctlAltDelAction->addTo( cornerMenu );
    ctlAltDelAction->setEnabled( false );

    disconnectAction = new QAction( tr("Disconnect"), QString::null, 0, 0 );
    connect( disconnectAction, SIGNAL(activated()),
	    this, SLOT( closeConnection() ) );
    disconnectAction->addTo( cornerMenu );
    disconnectAction->setEnabled( false );

    doubleClickAction = new QAction( tr("Next Click is Double Click"), QString::null, 0, 0 );
    connect( doubleClickAction, SIGNAL(activated()),
	    canvas, SLOT( markDoubleClick() ) );
    doubleClickAction->addTo( cornerMenu );
    doubleClickAction->setEnabled( false );

    rightClickAction = new QAction( tr("Next Click is Right Click"), QString::null, 0, 0 );
    connect( rightClickAction, SIGNAL(activated()),
	    canvas, SLOT( markRightClick() ) );
    rightClickAction->addTo( cornerMenu );
    rightClickAction->setEnabled( false );
}

void KVNC::toggleFullScreen()
{
		if ( fullscreen ) {
			canvas->releaseKeyboard();
			canvas->reparent( stack, 0, QPoint(0,0), false );
			canvas->setFrameStyle( QFrame::Panel | QFrame::Sunken );
			setCentralWidget( stack );
			stack->addWidget(canvas,get_unique_id());
			stack->raiseWidget(canvas);
			canvas->show();
			stack->show();
			fullScreenAction->setText( tr("Full Screen") );
		} else {
			canvas->setFrameStyle( QFrame::NoFrame );
			stack->removeWidget(canvas);
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
    ctlAltDelAction->setEnabled(true);
    disconnectAction->setEnabled( true );
    fullScreenAction->setEnabled( true );
    doubleClickAction->setEnabled( false );
    rightClickAction->setEnabled( true );
		stack->raiseWidget(canvas);
	 	bar->hide();
}

void KVNC::loggedIn()
{
    static QString msg = tr( "Logged in to remote host" );
    statusMessage( msg );
}

void KVNC::disconnected()
{

		if ( fullscreen ) 
				toggleFullScreen();
    static QString msg = tr( "Connection closed" );
    statusMessage( msg );
    ctlAltDelAction->setEnabled(false);
    disconnectAction->setEnabled( false );
    fullScreenAction->setEnabled( false );
    doubleClickAction->setEnabled( false );
    rightClickAction->setEnabled( false );
		stack->raiseWidget(bookmarkSelector);
	 	bar->show();
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
void KVNC::deleteBookmark(void)
{
		bookmarkSelector->deleteBookmark(bookmarkSelector->selectedBookmark());
}
