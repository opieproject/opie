/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

//
// Full-screen and rotation options contributed by Robert Wittams <robert@wittams.com>
//

#include "showimg.h"

#include <qpe/resource.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>

#include <qpe/qpemenubar.h>
#include <qwidgetstack.h>
#include <qpe/qpetoolbar.h>
#include <qaction.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qkeycode.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qtimer.h>


ImagePane::ImagePane( QWidget *parent ) : QWidget( parent )
{
    vb = new QVBoxLayout( this );

    image = new ImageWidget( this );
    connect(image, SIGNAL( clicked() ), this, SLOT( imageClicked() ));

    vb->addWidget( image );

    status = new QLabel( this );
    status->setFixedHeight( fontMetrics().height() + 4 );
    vb->addWidget( status );
}

void ImagePane::setPixmap( const QPixmap &pm )
{
    image->setPixmap( pm );
    image->repaint( false );
}

void ImagePane::imageClicked()
{
    emit clicked();
}

void ImagePane::showStatus()
{
    delete vb;
    vb = new QVBoxLayout( this );
    vb->addWidget( image );
    status->show();
    vb->addWidget( status );
}


void ImagePane::hideStatus()
{
    delete vb;
    vb = new QVBoxLayout( this );
    vb->addWidget( image );
    status->hide();
}

//===========================================================================
/*
  Draws the portion of the scaled pixmap that needs to be updated
*/

void ImageWidget::paintEvent( QPaintEvent *e )
{
    QPainter painter(this);

    painter.setClipRect(e->rect());
    painter.setBrush( black );
    painter.drawRect( 0, 0, width(), height() );

    if ( pixmap.size() != QSize( 0, 0 ) ) { // is an image loaded?
	painter.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *)
{
    emit clicked();
}


//===========================================================================

ImageViewer::ImageViewer( QWidget *parent, const char *name, int wFlags )
    : QMainWindow( parent, name, wFlags ), filename( 0 ), 
      pickx( -1 ), picky( -1 ), clickx( -1 ), clicky( -1 ), bFromDocView( FALSE )
{
    setCaption( tr("Image Viewer") );
    setIcon( Resource::loadPixmap( "ImageViewer" ) );

    isFullScreen = FALSE;

    setToolBarsMovable( FALSE );

    toolBar = new QPEToolBar( this );
    toolBar->setHorizontalStretchable( TRUE );

    menubar = new QPEMenuBar( toolBar );

    QStrList fmt = QImage::outputFormats();

    QPopupMenu *edit = new QPopupMenu( menubar );
    QPopupMenu *view = new QPopupMenu( menubar );

    menubar->insertItem(tr("Edit"), edit );
    menubar->insertItem(tr("View"), view );

    edit->insertItem(tr("Horizontal flip"), this, SLOT(hFlip()), 0);
    edit->insertItem(tr("Vertical flip"), this, SLOT(vFlip()), 0);

    stack = new QWidgetStack( this );
    stack->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    setCentralWidget( stack );

    imagePanel = new ImagePane( stack );
    connect(imagePanel, SIGNAL(clicked()), this, SLOT(normalView()));

    fileSelector = new FileSelector("image/*", stack, "fs");
    fileSelector->setNewVisible(FALSE);
    fileSelector->setCloseVisible(FALSE);
    connect( fileSelector, SIGNAL( closeMe() ), this, SLOT( closeFileSelector() ) );
    connect( fileSelector, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( openFile( const DocLnk & ) ) );

    toolBar = new QPEToolBar( this );

    QAction *a;

    a = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( open() ) );
    a->addTo( toolBar );

    a = new QAction( tr( "Rotate 180" ), Resource::loadPixmap( "repeat" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( rot180() ) );
    a->addTo( toolBar );
    a->addTo( edit );

    a = new QAction( tr( "Rotate 90"), Resource::loadPixmap( "rotate90" ), QString::null, 0, this, 0);
    connect( a, SIGNAL( activated() ), this, SLOT( rot90() ) );
    a->addTo( toolBar );
    a->addTo( edit );

    a = new QAction( tr( "Fullscreen" ), Resource::loadPixmap( "fullscreen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fullScreen() ) );
    a->addTo( toolBar );
    a->addTo( view);

    stack->raiseWidget( fileSelector );

    setMouseTracking( TRUE );
}

ImageViewer::~ImageViewer()
{
    delete imagePanel; // in case it is fullscreen
}

void ImageViewer::setDocument(const QString& fileref)
{
    delayLoad = fileref;
    stack->raiseWidget(imagePanel);
    QTimer::singleShot( 0, this, SLOT(doDelayedLoad()) );
}

void ImageViewer::doDelayedLoad()
{
    show(delayLoad);
}

void ImageViewer::show()
{
    normalView();
    QMainWindow::show();
}

void ImageViewer::show(const QString& fileref)
{
    bFromDocView = TRUE;
    closeFileSelector();
    DocLnk link(fileref);
    if ( link.isValid() ) {
	openFile(link);
    } else {
	filename = fileref;
	updateCaption( fileref );
	loadImage( fileref );
    }
}

void ImageViewer::openFile( const DocLnk &file )
{
    closeFileSelector();
    DocLnk link(file);
    updateCaption( link.name() );
    loadImage( link.file() );
}

void ImageViewer::open()
{
    stack->raiseWidget(fileSelector);
}

void ImageViewer::closeFileSelector()
{
    stack->raiseWidget(imagePanel);
}

void ImageViewer::updateCaption( QString name )
{
    int sep = name.findRev( '/' );
    if ( sep >= 0 )
	name = name.mid( sep+1 );
    setCaption( name + tr(" - Image Viewer") );
}

/*
  This function loads an image from a file.
*/

void ImageViewer::loadImage( const char *fileName )
{
    filename = fileName;
    if ( filename ) {
	QApplication::setOverrideCursor( waitCursor ); // this might take time
	imagePanel->statusLabel()->setText( tr("Loading image...") );
	qApp->processEvents();
	bool ok = image.load(filename, 0);
	pickx = -1;
	clickx = -1;
	if ( ok )
	    ok = reconvertImage();
	if ( !ok ) {
	    pm.resize(0,0);				// couldn't load image
	    update();
	}
	QApplication::restoreOverrideCursor();	// restore original cursor
    }
    updateStatus();
    imagePanel->setPixmap( pmScaled );
    stack->raiseWidget(imagePanel);
}

bool ImageViewer::loadSelected()
{
    bool ok = false;
    if ( stack->visibleWidget() == fileSelector ) {
	const DocLnk *link = fileSelector->selected();
	if ( link ) {
	    if ( link->file() != filename ) {
		updateCaption( link->name() );
		filename = link->file();
		imagePanel->statusLabel()->setText( tr("Loading image...") );
		qApp->processEvents();
		ok = image.load(filename, 0);
		if ( ok )
		    ok = reconvertImage();
		if ( !ok )
		    pm.resize(0,0);
	    }
	}
    }
    if ( !image.isNull() ) {
	ok = true;
	closeFileSelector();
    }

    return ok;
}

bool ImageViewer::reconvertImage()
{
    bool success = FALSE;

    if ( image.isNull() ) return FALSE;

    QApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( pm.convertFromImage(image /*, conversion_flags */ ) )
    {
	pmScaled = QPixmap();
	scale();
	success = TRUE;				// load successful
    } else {
	pm.resize(0,0);				// couldn't load image
    }
    QApplication::restoreOverrideCursor();	// restore original cursor

    return success;				// TRUE if loaded OK
}


int ImageViewer::calcHeight()
{
    if ( !isFullScreen)
	 return  height() - menubar->heightForWidth( width() )
		    - imagePanel->statusLabel()->height();
    else
         return qApp->desktop()->height();
}
/*
  This functions scales the pixmap in the member variable "pm" to fit the
  widget size and  puts the resulting pixmap in the member variable "pmScaled".
*/

void ImageViewer::scale()
{
    int h = calcHeight();
    if ( image.isNull() ) return;

    QApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( width() == pm.width() && h == pm.height() ) { // no need to scale if widget
	pmScaled = pm;				// size equals pixmap size
    } else {
	double hs = (double)h / (double)image.height();
	double ws = (double)width() / (double)image.width();
	double scaleFactor = (hs > ws) ? ws : hs;
	int smoothW = (int)(scaleFactor * image.width());
	int smoothH = (int)(scaleFactor * image.height());

	pmScaled.convertFromImage( image.smoothScale( smoothW, smoothH ) /*, conversion_flags */ );
    }
    QApplication::restoreOverrideCursor();	// restore original cursor
}

/*
  The resize event handler, if a valid pixmap was loaded it will call
  scale() to fit the pixmap to the new widget size.
*/

void ImageViewer::resizeEvent( QResizeEvent * )
{
    imagePanel->statusLabel()->setGeometry(0, height() - imagePanel->statusLabel()->height(),
			width(), imagePanel->statusLabel()->height());

    if ( pm.size() == QSize( 0, 0 ) )		// we couldn't load the image
	return;

    int h = calcHeight();

    if ( width() != pmScaled.width() || h != pmScaled.height())
    {						// if new size,
	scale();				// scale pmScaled to window
	updateStatus();
    }
    if ( image.hasAlphaBuffer() )
	erase();
}

void ImageViewer::convertEvent( QMouseEvent* e, int& x, int& y)
{
    if ( pm.size() != QSize( 0, 0 ) ) {
	int h = height() - menubar->heightForWidth( width() ) - imagePanel->statusLabel()->height();
	int nx = e->x() * image.width() / width();
	int ny = (e->y()-menubar->heightForWidth( width() )) * image.height() / h;
	if (nx != x || ny != y ) {
	    x = nx;
	    y = ny;
	    updateStatus();
	}
    }
}

void ImageViewer::mousePressEvent( QMouseEvent *e )
{
    convertEvent(e, clickx, clicky);
}

void ImageViewer::mouseMoveEvent( QMouseEvent *e )
{
    convertEvent( e, pickx, picky );
}

void ImageViewer::hFlip()
{
    if ( loadSelected() )
	setImage(image.mirror(TRUE,FALSE));
}

void ImageViewer::vFlip()
{
    if ( loadSelected() )
	setImage(image.mirror(FALSE,TRUE));
}

void ImageViewer::rot180()
{
    if ( loadSelected() )
	setImage(image.mirror(TRUE,TRUE));
}

void ImageViewer::rot90()
{
    if ( loadSelected() ) {
	QImage oldimage, newimage;
	uchar *oldbits, *newbits;
	int i, j, p;
	int w, h;

	oldimage = image.convertDepth(32);
	w = oldimage.height();
	h = oldimage.width();
	newimage = QImage( w, h, 32);

	oldbits = oldimage.bits();
	newbits = newimage.bits();

	for (i=0; i < w ; i++)
	    for (j=0; j < h; j++)
		for (p = 0 ; p < 4 ; p++)
		    newbits[(j * w + i) * 4 + p] = oldbits[ ((i + 1) * h  - j ) * 4 + p];

	setImage(newimage);
    }
}



void ImageViewer::normalView()
{
    if ( !imagePanel->parentWidget() ) {
	isFullScreen = FALSE;
	stack->addWidget( imagePanel, 1 );
//	imagePanel->reparent(stack,0,QPoint(0,0),FALSE);
//	imagePanel->resize(width(), calcHeight());
	scale();
	updateStatus();
	imagePanel->setPixmap( pmScaled );
	imagePanel->showStatus();
	//	imagePanel->show();
	stack->raiseWidget( imagePanel );
    }
}

void ImageViewer::fullScreen()
{
    // Full-screen and rotation options
    // contributed by Robert Wittams <robert@wittams.com>

    if ( imagePanel->parentWidget() && loadSelected() ) {
	isFullScreen = TRUE;
	imagePanel->reparent(0,QPoint(0,0));
	imagePanel->resize(qApp->desktop()->width(), qApp->desktop()->height());

	scale();
	updateStatus();
	imagePanel->hideStatus();
	imagePanel->setPixmap( pmScaled );
	imagePanel->showFullScreen();
    }
}

void ImageViewer::setImage(const QImage& newimage)
{
    image = newimage;
    pickx = -1;
    clickx = -1;
    reconvertImage();
    imagePanel->setPixmap( pmScaled );
    updateStatus();
}

void ImageViewer::updateStatus()
{
    if ( pm.size() == QSize( 0, 0 ) ) {
	if ( filename )
	    imagePanel->statusLabel()->setText( tr("Could not load image") );
	else
	    imagePanel->statusLabel()->setText( tr("No image - select Open from File menu.") );
    } else {
	QString message("%1x%2");
	message = message.arg(image.width()).arg(image.height());
	if ( pm.size() != pmScaled.size() )
	    message += QString(" [%1x%2]").arg(pmScaled.width()).arg(pmScaled.height());
	if (image.valid(pickx,picky)) {
	    QString moremsg;
	    moremsg.sprintf("(%d,%d)=#%0*x ",
			  pickx, picky,
			  image.hasAlphaBuffer() ? 8 : 6,
			  image.pixel(pickx,picky));
	    message += moremsg;
	}
	if ( image.numColors() > 0 ) {
	    if (image.valid(pickx,picky)) {
		message += tr(", %1/%2 colors")
		    .arg(image.pixelIndex(pickx,picky))
		    .arg(image.numColors());
	    } else {
		message += tr(", %1 colors").arg(image.numColors());
	    }
	} else if ( image.depth() >= 16 ) {
	    message += tr(" True color");
	}
	if ( image.hasAlphaBuffer() ) {
	    if ( image.depth() == 8 ) {
		int i;
		bool alpha[256];
		int nalpha=0;

		for (i=0; i<256; i++)
		    alpha[i] = FALSE;

		for (i=0; i<image.numColors(); i++) {
		    int alevel = image.color(i) >> 24;
		    if (!alpha[alevel]) {
			alpha[alevel] = TRUE;
			nalpha++;
		    }
		}
		message += tr(", %1 alpha levels").arg(nalpha);
	    } else {
		// Too many pixels to bother counting.
		message += tr(", 8-bit alpha channel");
	    }
	}
	imagePanel->statusLabel()->setText(message);
    }
}

void ImageViewer::closeEvent( QCloseEvent *e )
{
    if ( stack->visibleWidget() == imagePanel && !bFromDocView ) {
	e->ignore();
	open();
    } else {
	bFromDocView = FALSE;
	e->accept();
    }
}
