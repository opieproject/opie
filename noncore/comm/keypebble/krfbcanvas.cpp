#include "kvncconnectdlg.h"
#include "krfbconnection.h"
#include "krfbcanvas.h"
#include "krfboptions.h"
#include "krfbbuffer.h"

#include <qpe/config.h>

#include <qapplication.h>
#include <qclipboard.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qiconset.h>

KRFBCanvas::KRFBCanvas( QWidget *parent, const char *name )
  : QScrollView( parent, name )
{
    connection_ = new KRFBConnection();
    connect( connection_, SIGNAL( passwordRequired( KRFBConnection * ) ),
	    this, SLOT( passwordRequired( KRFBConnection * ) ) );
    connect( connection_, SIGNAL( loggedIn() ),
	    this, SLOT( loggedIn() ) );

    loggedIn_ = false;

    viewport()->setFocusPolicy( QWidget::StrongFocus );
    viewport()->setFocus();
}

KRFBCanvas::~KRFBCanvas()
{
}

void KRFBCanvas::openConnection()
{
  KVNCConnectDlg dlg( connection_, this, "connect dialog" );
  if ( dlg.exec() ) {
    QCString host = dlg.hostname().latin1();
    password = dlg.password();
    connection_->connectTo( host, dlg.display() );
  }
}

void KRFBCanvas::openURL( const QUrl &url )
{
  if ( loggedIn_ ) {
      qWarning( "openURL invoked when logged in\n" );
      return;
  }

  QCString host = url.host().latin1();
  int display = url.port();
  if ( url.hasPassword() )
    connection_->setPassword( url.password().latin1() );

  connection_->connectTo( host, display );
}

void KRFBCanvas::closeConnection()
{
  loggedIn_ = false;
  connection_->disconnect();

  viewport()->setMouseTracking( false );
  viewport()->setBackgroundMode( PaletteDark );
  setBackgroundMode( PaletteDark );
  update();
}

void KRFBCanvas::passwordRequired( KRFBConnection *con )
{
  con->setPassword( password.latin1() );
}

void KRFBCanvas::bell()
{
  if ( connection_->options()->deIconify ) {
    topLevelWidget()->raise();
    topLevelWidget()->show();
  }
}

void KRFBCanvas::loggedIn()
{
  qWarning( "Ok, we're logged in" );

  //
  // Get ready for action
  //
  loggedIn_ = true;
  viewport()->setMouseTracking( true );
  viewport()->setBackgroundMode( NoBackground );
  setBackgroundMode( NoBackground );

  // Start using the buffer
  connect( connection_->buffer(), SIGNAL( sizeChanged( int, int ) ),
           this, SLOT( resizeContents(int,int) ) );
  connect( connection_->buffer(), SIGNAL( updated( int, int, int, int ) ),
           this, SLOT( viewportUpdate(int,int,int,int) ) );
  connect( connection_->buffer(), SIGNAL( bell() ),
           this, SLOT( bell() ) );
  connect( qApp->clipboard(), SIGNAL( dataChanged() ),
           this, SLOT( clipboardChanged() ) );
}

void KRFBCanvas::viewportPaintEvent( QPaintEvent *e )
{
  QRect r = e->rect();

  if ( loggedIn_ ) {
    bitBlt( viewport(), r.x(), r.y(),
	    connection_->buffer()->pixmap(),
	    r.x() + contentsX(), r.y() + contentsY(),
	    r.width(), r.height() );
  }
  else {
    QScrollView::viewportPaintEvent( e );
  }
}

void KRFBCanvas::viewportUpdate( int x, int y, int w, int h )
{
  updateContents( x, y, w, h );
}

void KRFBCanvas::contentsMousePressEvent( QMouseEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->mouseEvent( e );
}

void KRFBCanvas::contentsMouseReleaseEvent( QMouseEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->mouseEvent( e );
}

void KRFBCanvas::contentsMouseMoveEvent( QMouseEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->mouseEvent( e );
}

void KRFBCanvas::keyPressEvent( QKeyEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->keyPressEvent( e );
}

void KRFBCanvas::keyReleaseEvent( QKeyEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->keyReleaseEvent( e );
}

void KRFBCanvas::refresh()
{
  if ( loggedIn_ )
    connection_->refresh();
}

void KRFBCanvas::clipboardChanged()
{
  if ( loggedIn_ ) {
      connection_->sendCutText( qApp->clipboard()->text() );
  }
}
