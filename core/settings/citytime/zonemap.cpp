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

#include "sun.h"
#include "zonemap.h"

#include <qpe/resource.h>
#include <qpe/timestring.h>
#include <qpe/qpeapplication.h>

#include <qdatetime.h>
#include <qfile.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlist.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtoolbutton.h>

#include <limits.h>

// the map file...
static const char strZONEINFO[] = "/usr/share/zoneinfo/zone.tab";
static const char strMAP[] = "simple_grid_400";

// the maximum distance we'll allow the pointer to be away from a city
// and still show the city's time
static const int iTHRESHOLD = 50000;

// The label offset (how far away from pointer)
static const int iLABELOFFSET = 8;

// the size of the dot to draw, and where to start it
static const int iCITYSIZE = 3;
const int iCITYOFFSET = 2;

// the darkening function
static inline void darken( QImage *pImage, int start, int stop, int row );
static void dayNight( QImage *pImage );

ZoneField::ZoneField( const QString& strLine )
{
    // make a bunch of RegExp's to match the data from the line
    QRegExp regCoord( "[-+][0-9]+" );	// the latitude
    QRegExp regCountry( "[A-Za-z]+/" ); // the country (not good enough)
    QRegExp regCity( "[A-Za-z_-]*" ); // the city

    int iStart,
	iStop,
        iLen,
        tmp;
    QString strTmp;
    // we should be able to assume that the country code is always the first
    // two chars, so just grap them and let it go...
    strCountryCode = strLine.left( 2 );
    iStart = regCoord.match( strLine, 0, &iLen );
    if ( iStart >= 0 ) {
	strTmp = strLine.mid( iStart, iLen );
	tmp = strTmp.toInt();
	// okay, there are two versions of the format, make a decision based on
	// the size...
	// Oh BTW, we are storing everything in seconds!
	if ( iLen < 7 ) {
	    _y = tmp / 100;
	    _y *= 60;
	    _y += tmp % 100;
	    _y *= 60;
	} else {
	    _y = tmp / 10000;
	    _y *= 60;
	    tmp %= 10000;
	    _y += tmp / 100;
	    _y *= 60;
	    tmp %= 100;
	    _y += tmp;
	}
    }
    iStart = regCoord.match( strLine, iStart + iLen, &iLen );
    if ( iStart >= 0 ) {
	strTmp = strLine.mid( iStart, iLen );
	tmp = strTmp.toInt();
	if ( iLen < 8 ) {
	    _x = tmp / 100;
	    _x *= 60;
	    _x += tmp % 100;
	    _x *= 60;
	} else {
	    _x = tmp / 10000;
	    _x *= 60;
	    tmp %= 10000;
	    _x += tmp / 100;
	    _x *= 60;
	    tmp %= 100;
	    _x += tmp;
	}
    }
    iStart = regCountry.match( strLine, 0, &iLen );
    // help with the shortcoming in 2.x regexp...
    iStop = strLine.findRev( '/' );
    if ( iStart >= 0 ) {
	iLen = (iStop - iStart) + 1;
	strCountry = strLine.mid( iStart, iLen );
    }
    // now match the city...
    iStart = regCity.match( strLine, iStart + iLen, &iLen );
    if ( iStart >= 0 ) {
	strCity = strLine.mid( iStart, iLen );
    }
}

void ZoneField::showStructure( void ) const
{
    qDebug( "Country: %s", strCountry.latin1() );
    qDebug( "City: %s", strCity.latin1() );
    qDebug( "x: %d", _x );
    qDebug( "y: %d\n", _y );
}

ZoneMap::ZoneMap( QWidget *parent, const char* name )
    : QScrollView( parent, name ),
      pLast( 0 ),
      pRepaint( 0 ),
      ox( 0 ),
      oy( 0 ),
      drawableW( -1 ),
      drawableH( -1 ),
      bZoom( FALSE ),
      bIllum( TRUE ),
      cursor( 0 )
{
    viewport()->setFocusPolicy( StrongFocus );

    // set mouse tracking so we can use the mouse move event
    zones.setAutoDelete( true );
    // get the map loaded
    // just set the current image to point
    pixCurr = new QPixmap();

    QPixmap pixZoom = Resource::loadPixmap( "mag" );

    cmdZoom = new QToolButton( this, "Zoom command" );
    cmdZoom->setPixmap( pixZoom );
    cmdZoom->setToggleButton( true );

    cmdZoom->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0,
                            (QSizePolicy::SizeType)0,
                            cmdZoom->sizePolicy().hasHeightForWidth() ) );
    cmdZoom->setMaximumSize( cmdZoom->sizeHint() );
    // probably don't need this, but just in case...
    cmdZoom->move( width() - cmdZoom->width(), height() - cmdZoom->height() );


    lblCity = new QLabel( tr( "CITY" ), this, "City Label" );
    lblCity->setMinimumSize( lblCity->sizeHint() );
    lblCity->setFrameStyle( QFrame::Plain | QFrame::Box );
    lblCity->setBackgroundColor( yellow );
    lblCity->hide();

    // A timer to make sure the label gets hidden
    tHide = new QTimer( this, "Label Timer" );
    QObject::connect( tHide, SIGNAL( timeout() ),
                      lblCity, SLOT( hide() ) );
    QObject::connect( tHide, SIGNAL( timeout() ),
                      this, SLOT( slotRedraw() ) );
    QTimer *tUpdate = new QTimer( this, "Update Timer" );
    QObject::connect( tUpdate, SIGNAL( timeout() ),
                      this, SLOT( slotUpdate() ) );
    QObject::connect( qApp, SIGNAL( timeChanged() ),
                      this, SLOT( slotUpdate() ) );
    QObject::connect( cmdZoom, SIGNAL( toggled( bool ) ),
                      this, SLOT( slotZoom( bool ) ) );
    QObject::connect( &norm, SIGNAL( signalNewPoint( const QPoint& ) ),
                      this, SLOT( slotFindCity( const QPoint& ) ) );
    QObject::connect( qApp, SIGNAL( clockChanged( bool ) ),
                      this, SLOT( changeClock( bool ) ) );
    // update the sun's movement every 5 minutes
    tUpdate->start( 5 * 60 * 1000 );
    // May as well read in the timezone information too...
    readZones();
}

ZoneMap::~ZoneMap()
{
}

void ZoneMap::readZones( void )
{
    QFile fZone( strZONEINFO );
    if ( !fZone.open( IO_ReadOnly ) ) {
	QMessageBox::warning (this,
		tr( "Unable to Find Timezone Info" ),
		tr( "<p>Unable to find any timezone information in %1" )
		.arg( strZONEINFO ));
	exit(-1);
    } else {
	QTextStream tZone( &fZone );
	while ( !tZone.atEnd() ) {
	    QString strLine = tZone.readLine();
	    // only pass on lines that aren't comments
	    if ( strLine[0] != '#' ) {
		zones.append( new ZoneField( strLine ) );
	    }
	}
	fZone.close();
    }
}

void ZoneMap::viewportMousePressEvent( QMouseEvent* event )
{
    // add the mouse event into the normalizer, and get the average,
    // pass it along
    slotRedraw();
    norm.start();
    norm.addEvent( event->pos() );
}

void ZoneMap::viewportMouseMoveEvent( QMouseEvent* event )
{
    norm.addEvent( event->pos() );
}

void ZoneMap::viewportMouseReleaseEvent( QMouseEvent* )
{
    // get the averaged points in case a timeout hasn't occurred,
    // more for "mouse clicks"
    norm.stop();
    if ( pLast != NULL ) {
	emit signalTz( pLast->country(), pLast->city() );
	pLast = NULL;
    }
    tHide->start( 2000, true );
}

void ZoneMap::keyPressEvent( QKeyEvent *ke )
{
    switch ( ke->key() ) {
	case Key_Left:
	case Key_Right:
	case Key_Up:
	case Key_Down: {
		tHide->stop();
		if ( !cursor )
		    slotFindCity( QPoint( contentsWidth(), contentsHeight() ) / 2 );
		ZoneField *city = findCityNear( cursor, ke->key() );
		if ( city ) {
		    cursor = city;
		    int tmpx, tmpy;
		    zoneToWin( cursor->x(), cursor->y(), tmpx, tmpy );
		    ensureVisible( tmpx, tmpy );
		    showCity( cursor );
		    tHide->start( 3000, true );
		}
	    }
	    break;

	case Key_Space:
	case Key_Enter:
	case Key_Return:
	    if ( cursor ) {
		emit signalTz( cursor->country(), cursor->city() );
		tHide->start( 0, true );
	    }
	    break;
    }
}

ZoneField *ZoneMap::findCityNear( ZoneField *city, int key )
{
    ZoneField *pZone;
    ZoneField *pClosest = 0;
    long ddist = LONG_MAX;

    QListIterator<ZoneField> it( zones );
    for (; it.current(); ++it) {
	pZone = it.current();
	long dx = (pZone->x() - city->x())/100;
	long dy = (pZone->y() - city->y())/100;
	switch ( key ) {
	    case Key_Right:
	    case Key_Left:
		if ( key == Key_Left )
		    dx = -dx;
		if ( dx > 0 ) {
		    long dist = QABS(dy)*4 + dx;
		    if ( dist < ddist ) {
			ddist = dist;
			pClosest = pZone;
		    }
		}
		break;
	    case Key_Down:
	    case Key_Up:
		if ( key == Key_Down )
		    dy = -dy;
		if ( dy > 0 ) {
		    long dist = QABS(dx)*4 + dy;
		    if ( dist < ddist ) {
			ddist = dist;
			pClosest = pZone;
		    }
		}
		break;
	}
    }

    return pClosest;
}

void ZoneMap::slotFindCity( const QPoint &pos )
{
    lblCity->hide();
    // given coordinates on the screen find the closest city and display the
    // label close to it
    int tmpx, tmpy, x, y;
    long lDistance,
         lClosest;
    ZoneField *pZone,
              *pClosest;

    if ( tHide->isActive() ) {
        tHide->stop();
    }
    viewportToContents(pos.x(), pos.y(), tmpx, tmpy);
    winToZone( tmpx, tmpy, x, y );
    // Find city alogorithim: start out at an (near) infinite distance away and
    // then find the closest city, (similar to the Z-buffer technique, I guess)
    // the only problem is that this is all done with doubles, but I don't know
    // another way to do it at the moment.  Another problem is a linked list is
    // used obviously something indexed would help
    QListIterator<ZoneField> it( zones );
    pClosest = 0;
    lClosest = LONG_MAX;
    for (; it.current(); ++it) {
	pZone = it.current();
	// use the manhattenLength, a good enough of an appoximation here
	lDistance = QABS( x - pZone->x() ) + QABS( y - pZone->y() );
	// first to zero wins!
	if ( lDistance < lClosest ) {
	    lClosest = lDistance;
	    pClosest = pZone;
	}
    }

    // Okay, we found the closest city, but it might still be too far away.
    if ( lClosest <= iTHRESHOLD ) {
	showCity( pClosest );
	cursor = pClosest;
    }
}

void ZoneMap::showCity( ZoneField *city )
{
    pLast = city;
    // we'll use city and country a couple of times, get them to save some
    // time
    QString strCity = pLast->city();
    QString strCountry = pLast->country();
    // Display the time at this location by setting the environment timezone
    // getting the current time [there] and then swapping back the variable
    // so no one notices...
    QString strSave;
    char *p = getenv( "TZ" );
    if ( p ) {
	strSave = p;
    }
    // set the timezone :)
    setenv( "TZ", strCountry + strCity, true );
    lblCity->setText( strCity.replace( QRegExp("_"), " ") + "\n" +
		      TimeString::shortTime( ampm ) );
    lblCity->setMinimumSize( lblCity->sizeHint() );
    // undue our damage...
    unsetenv( "TZ" );
    if ( p )
	setenv( "TZ", strSave, true );
    // Now decide where to move the label, x & y can be reused
    int tmpx, tmpy, x, y;
    zoneToWin( pLast->x(), pLast->y(), tmpx, tmpy );
    contentsToViewport(tmpx, tmpy, x, y);
    if ( lblCity->width() > drawableW - x ) {
	// oops... try putting it on the right
	x = x - lblCity->width() - iLABELOFFSET;
    } else {
	// the default...
	x += iLABELOFFSET;
    }
    if ( lblCity->height() > drawableH - y ) {
	// move it up...
	y = y - lblCity->height() - iLABELOFFSET;
    } else if ( y < 0 ) {
	// the city is actually off the screen...
	// this only happens on the a zoom when you are near the top,
	// a quick workaround..
	y = iLABELOFFSET;
    } else {
	// the default
	y += iLABELOFFSET;
    }

    // draw in the city and the label
    if ( pRepaint ) {
	int repx,
	    repy;
	zoneToWin( pRepaint->x(), pRepaint->y(), repx, repy );
	updateContents( repx - iCITYOFFSET, repy - iCITYOFFSET,
			iCITYSIZE, iCITYSIZE );
    }
    updateContents( tmpx - iCITYOFFSET, tmpy - iCITYOFFSET, iCITYSIZE,
		    iCITYSIZE );
    pRepaint = pLast;

    lblCity->move( x, y );
    lblCity->show();
}

void ZoneMap::resizeEvent( QResizeEvent *e )
{
    // keep the zoom button down in the corner
    QSize _size = e->size();
    cmdZoom->move( _size.width() - cmdZoom->width(),
                   _size.height() - cmdZoom->height() );
    if ( !bZoom ) {
	drawableW = width() - 2 * frameWidth();
	drawableH = height() - 2 * frameWidth();
	makeMap( drawableW, drawableH );
	resizeContents( drawableW, drawableH );
    }
}

void ZoneMap::showZones( void ) const
{
    // go through the zones in the list and just display the values...
    QListIterator<ZoneField> itZone( zones );
    for ( itZone.toFirst(); itZone.current(); ++itZone ) {
	ZoneField *pZone = itZone.current();
	pZone->showStructure();
    }
}

void ZoneMap::drawCities( QPainter *p )
{
    int x,
        y,
        j;
    // draw in the cities
    // for testing only as when you put it
    // on the small screen it looks awful and not to mention useless
    p->setPen( red );
    QListIterator<ZoneField> itZone( zones );
    for ( itZone.toFirst(), j = 0; itZone.current(); ++itZone, j++ ) {
	ZoneField *pZone = itZone.current();
	zoneToWin( pZone->x(), pZone->y(), x, y );
	if ( x > wImg )
	    x = x - wImg;
	p->drawRect( x - iCITYOFFSET, y - iCITYOFFSET, iCITYSIZE, iCITYSIZE);
    }
}

static void dayNight(QImage *pImage)
{
    // create a mask the functions from sun.h
    double dJulian,
           dSunRad,
           dSunDecl,
           dSunRadius,
           dSunLong;
    int wImage = pImage->width(),
        hImage = pImage->height(),
        iStart,
        iStop,
        iMid,
        relw,
        i;
    short wtab[ wImage ];
    time_t tCurrent;
    struct tm *pTm;

    // get the position of the sun bassed on our current time...
    tCurrent = time( NULL );
    pTm = gmtime( &tCurrent );
    dJulian = jtime( pTm );
    sunpos( dJulian, 0, &dSunRad, &dSunDecl, &dSunRadius, &dSunLong );

    // now get the projected illumination
    projillum( wtab, wImage, hImage, dSunDecl );
    relw = wImage - int( wImage * 0.0275 );

    // draw the map, keeping in mind that we may go too far off the map...
    iMid = ( relw * ( 24*60 - pTm->tm_hour * 60 - pTm->tm_min ) ) / ( 24*60 );

    for ( i = 0; i < hImage; i++ ) {
	if ( wtab[i] > 0 ) {
	    iStart = iMid - wtab[i];
	    iStop = iMid + wtab[i];
	    if ( iStart < 0 ) {
		darken( pImage, iStop, wImage + iStart, i );
	    } else if ( iStop > wImage ) {
		darken( pImage, iStop - wImage, iStart, i );
	    } else {
		darken( pImage, 0, iStart, i );
		darken( pImage, iStop, wImage, i );
	    }
	} else {
	    darken( pImage, 0, wImage, i );
	}
    }
}

static inline void darken( QImage *pImage, int start, int stop, int row )
{
    int colors,
        j;
    uchar *p;

    // assume that the image is similar to the one we have...
    colors = pImage->numColors() / 2;

    p = pImage->scanLine( row );
    for ( j = start; j <= stop; j++ ) {
	if ( p[j] < colors )
	    p[j] += colors;
    }
}

void ZoneMap::makeMap( int w, int h )
{
    QImage imgOrig = Resource::loadImage( strMAP );
    if ( imgOrig.isNull() ) {
	QMessageBox::warning( this,
	                      tr( "Couldn't Find Map" ),
	                      tr( "<p>Couldn't load map: %1, exiting")
                              .arg( strMAP ) );
	exit(-1);
    }

    // set up the color table for darkening...
    imgOrig = imgOrig.convertDepth( 8 );
    int numColors = imgOrig.numColors();
    // double the colors
    imgOrig.setNumColors( 2 * numColors );
    // darken the new ones...
    for ( int i = 0; i < numColors; i++ ) {
	QRgb rgb = imgOrig.color( i );
	imgOrig.setColor ( i + numColors, qRgb(  2 * qRed( rgb ) / 3,
	                   2 * qGreen( rgb ) / 3, 2 * qBlue( rgb ) / 3 ) );
    }

    // else go one with making the map...
    if ( bIllum ) {
	// do a daylight mask
	dayNight(&imgOrig);
    }
    // redo the width and height
    wImg = w;
    hImg = h;
    ox = ( wImg / 2 ) - int( wImg * 0.0275 );
    oy = hImg / 2;
    pixCurr->convertFromImage( imgOrig.smoothScale(w, h),
                               QPixmap::ThresholdDither );
}

void ZoneMap::drawCity( QPainter *p, const ZoneField *pCity )
{
    int x,
        y;

    p->setPen( red );
    zoneToWin( pCity->x(), pCity->y(), x, y );
    p->drawRect( x - iCITYOFFSET, y - iCITYOFFSET, iCITYSIZE, iCITYSIZE );
}

void ZoneMap::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    // if there is a need to resize, then do it...
    // get our drawable area
    drawableW = width() - 2 * frameWidth();
    drawableH = height() - 2 * frameWidth();

    int pixmapW = pixCurr->width(),
        pixmapH = pixCurr->height();
    if ( !bZoom &&  ( ( pixmapW != drawableW ) ||
                      ( pixmapH != drawableH) ) ) {
	makeMap( drawableW, drawableH );
    }

    // taken from the scrollview example...
    int rowheight = pixCurr->height();
    int toprow = cy / rowheight;
    int bottomrow = ( cy + ch + rowheight - 1 ) / rowheight;
    int colwidth = pixCurr->width();
    int leftcol= cx / colwidth;
    int rightcol= ( cx + cw + colwidth - 1 ) / colwidth;
    for ( int r = toprow; r <= bottomrow; r++ ) {
	int py = r * rowheight;
	for ( int c = leftcol; c <= rightcol; c++ ) {
	    int px = c * colwidth;
	    p->drawPixmap( px, py, *pixCurr );
	}
    }

    // Draw that city!
    if ( pLast )
	drawCity( p, pLast );
}

void ZoneMap::slotZoom( bool setZoom )
{
    bZoom = setZoom;
    if ( bZoom ) {
	makeMap( 2 * wImg , 2 * hImg );
	resizeContents( wImg, hImg );
    } else {
	makeMap( drawableW, drawableH );
	resizeContents( drawableW, drawableH );
    }
}

void ZoneMap::slotIllum( bool setIllum )
{
    bIllum = !setIllum;
    // make the map...
    makeMap( pixCurr->width(), pixCurr->height() );
    updateContents( 0, 0, wImg, hImg );
}

void ZoneMap::slotUpdate( void )
{
    // recalculate the light, most people will never see this,
    // but it is good to be complete
    makeMap ( pixCurr->width(), pixCurr->height() );
    updateContents( contentsX(), contentsY(), drawableW, drawableH );
}

void ZoneMap::slotRedraw( void )
{
    // paint over that pesky city...
    int x,
        y;
    if ( pRepaint ) {
	pLast = 0;
	zoneToWin(pRepaint->x(), pRepaint->y(), x, y);
	updateContents( x - iCITYOFFSET, y - iCITYOFFSET, iCITYSIZE, iCITYSIZE);
        pRepaint = 0;
    }
}

void ZoneMap::changeClock( bool whichClock )
{
    ampm = whichClock;
}
