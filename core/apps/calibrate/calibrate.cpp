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

#include <math.h>

#include "calibrate.h"

#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <qapplication.h>

//#if defined(Q_WS_QWS) || defined(_WS_QWS_)

#include <qpainter.h>
#include <qtimer.h>
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>


Calibrate::Calibrate( QWidget* parent, const char * name, WFlags wf ) :
		QDialog( parent, name, TRUE, wf | WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop )
{
#ifdef QWS
	showCross = TRUE;
	const int offset = 30;
	QRect desk = qApp->desktop() ->geometry();
	setGeometry( 0, 0, desk.width(), desk.height() );
	if ( desk.height() < 250 ) {
		int w = desk.height() / 3;
		logo.convertFromImage( Resource::loadImage( "logo/opielogo" ).smoothScale( w, w ) );
	}
	else {
		logo = Resource::loadPixmap( "logo/opielogo" );
	}
	cd.screenPoints[ QWSPointerCalibrationData::TopLeft ] = QPoint( offset, offset );
	cd.screenPoints[ QWSPointerCalibrationData::BottomLeft ] = QPoint( offset, qt_screen->deviceHeight() - offset );
	cd.screenPoints[ QWSPointerCalibrationData::BottomRight ] = QPoint( qt_screen->deviceWidth() - offset, qt_screen->deviceHeight() - offset );
	cd.screenPoints[ QWSPointerCalibrationData::TopRight ] = QPoint( qt_screen->deviceWidth() - offset, offset );
	cd.screenPoints[ QWSPointerCalibrationData::Center ] = QPoint( qt_screen->deviceWidth() / 2, qt_screen->deviceHeight() / 2 );
	goodcd = cd;
	reset();

	timer = new QTimer( this );
	connect( timer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
#endif
}

Calibrate::~Calibrate()
{
	store();
}

void Calibrate::show()
{
#ifdef QWS
	grabMouse();
	QWSServer::mouseHandler() ->getCalibration( &goodcd );
	QWSServer::mouseHandler() ->clearCalibration();
	QDialog::show();
#endif
}

void Calibrate::store()
{
#ifdef QWS
	QWSServer::mouseHandler() ->calibrate( &goodcd );
#endif
}

void Calibrate::hide()
{
	if ( isVisible ( )) {
		store();

		// hack - calibrate is a launcher dialog, but treated like a standalone app
		{
			QCopEnvelope e( "QPE/System", "closing(QString)" );
			e << QString ( "calibrate" );
		}
	}
	QDialog::hide();
}

void Calibrate::reset()
{
#ifdef QWS
	penPos = QPoint();
	location = QWSPointerCalibrationData::TopLeft;
	crossPos = fromDevice( cd.screenPoints[ location ] );
#endif
}

QPoint Calibrate::fromDevice( const QPoint &p )
{
#ifdef QWS
	return qt_screen->mapFromDevice ( p, QSize( qt_screen->deviceWidth ( ), qt_screen->deviceHeight() ) );
#else
        return QPoint();
#endif

}

bool Calibrate::sanityCheck()
{
#ifdef QWS
	QPoint tl = cd.devPoints[QWSPointerCalibrationData::TopLeft];
	QPoint tr = cd.devPoints[QWSPointerCalibrationData::TopRight];
	QPoint bl = cd.devPoints[QWSPointerCalibrationData::BottomLeft];
	QPoint br = cd.devPoints[QWSPointerCalibrationData::BottomRight];

	// not needed anywhere .. just calculate it, so it's there
	cd. devPoints [QWSPointerCalibrationData::Center] = QRect ( tl, br ). normalize ( ). center ( );

	int dlx = QABS( bl. x ( ) - tl. x ( ));
	int dly = QABS( bl. y ( ) - tl. y ( ));
	int drx = QABS( br. x ( ) - tr. x ( ));
	int dry = QABS( br. y ( ) - tr. y ( ));
	int dtx = QABS( tr. x ( ) - tl. x ( ));
	int dty = QABS( tr. y ( ) - tl. y ( ));
	int dbx = QABS( br. x ( ) - bl. x ( ));
	int dby = QABS( br. y ( ) - bl. y ( ));

	int dl = (int) ::sqrt (( dlx * dlx ) + ( dly * dly )); // calculate vector lengths for all sides
	int dr = (int) ::sqrt (( drx * drx ) + ( dry * dry ));
	int dt = (int) ::sqrt (( dtx * dtx ) + ( dty * dty ));
	int db = (int) ::sqrt (( dbx * dbx ) + ( dby * dby ));

	// Calculate leeway for x/y (we do not care if diff1/diff2 is for x or y here !)
	int diff1 = QABS( dl - dr );
	int avg1  = ( dl + dr ) / 2;
	int diff2 = QABS( dt - db );
	int avg2 = ( dt + db ) / 2;

	// Calculate leeway for "real" vector length against "manhattan" vector length
	// This is a check, if the rect is rotated (other then 0/90/180/270)
	// It needs to be performed only for the triange (bl, tl, tr)
	int diff3 = QABS(( dlx + dly + dtx + dty ) - ( dl + dt ));
	int avg3 = (( dlx + dly + dtx + dty ) + ( dl + dt )) / 2;

	if (( diff1 > ( avg1 / 20 )) || // 5% leeway
	    ( diff2 > ( avg2 / 20 )) ||
	    ( diff3 > ( avg3 / 20 )))
		return false;
	else
		return true;
#else
return true;
#endif
}

void Calibrate::moveCrosshair( QPoint pt )
{
	showCross = FALSE;
	repaint( crossPos.x() - 8, crossPos.y() - 8, 16, 16 );
	showCross = TRUE;
	crossPos = pt;
	repaint( crossPos.x() - 8, crossPos.y() - 8, 16, 16 );
}

void Calibrate::paintEvent( QPaintEvent * )
{
	QPainter p( this );

	int y;

	if ( !logo.isNull() ) {
		y = height() / 2 - logo.height() - 15;
		p.drawPixmap( ( width() - logo.width() ) / 2, y, logo );
	}

	y = height() / 2 + 15;

	p.drawText( 0, y + height() / 8, width(), height() - y, AlignHCenter,
	            tr( "Touch the crosshairs firmly and\n"
	                "accurately to calibrate your screen." ) );

	QFont f = p.font();
	f.setBold( TRUE );
	p.setFont( f );
	p.drawText( 0, y, width(), height() - y, AlignHCenter | WordBreak,
	            tr( "Welcome to Opie" ) );

	if ( showCross ) {
		p.drawRect( crossPos.x() - 1, crossPos.y() - 8, 2, 7 );
		p.drawRect( crossPos.x() - 1, crossPos.y() + 1, 2, 7 );
		p.drawRect( crossPos.x() - 8, crossPos.y() - 1, 7, 2 );
		p.drawRect( crossPos.x() + 1, crossPos.y() - 1, 7, 2 );
	}
}

void Calibrate::mousePressEvent( QMouseEvent *e )
{
#ifdef QWS
	// map to device coordinates
	QPoint devPos = qt_screen->mapToDevice( e->pos(), QSize( qt_screen->width(), qt_screen->height() ) );
	if ( penPos.isNull() )
		penPos = devPos;
	else
		penPos = QPoint( ( penPos.x() + devPos.x() ) / 2,
		                 ( penPos.y() + devPos.y() ) / 2 );
#endif
}

void Calibrate::mouseReleaseEvent( QMouseEvent * )
{
#ifdef QWS
	if ( timer->isActive() )
		return ;

	bool doMove = TRUE;

	cd.devPoints[ location ] = penPos;
	if ( location < QWSPointerCalibrationData::TopRight ) {
		location = (QWSPointerCalibrationData::Location) ( int( location ) + 1 );
	}
	else {
		if ( sanityCheck() ) {
			reset();
			goodcd = cd;
			hide();
			emit accept();
			doMove = FALSE;
		}
		else {
			location = QWSPointerCalibrationData::TopLeft;
		}
	}

	if ( doMove ) {
		QPoint target = fromDevice( cd.screenPoints[ location ] );
		dx = ( target.x() - crossPos.x() ) / 10;
		dy = ( target.y() - crossPos.y() ) / 10;
		timer->start( 30 );
	}
#endif
}

void Calibrate::timeout()
{
#ifdef QWS
	QPoint target = fromDevice( cd.screenPoints[ location ] );

	bool doneX = FALSE;
	bool doneY = FALSE;
	QPoint newPos( crossPos.x() + dx, crossPos.y() + dy );

	if ( QABS( crossPos.x() - target.x() ) <= QABS( dx ) ) {
		newPos.setX( target.x() );
		doneX = TRUE;
	}

	if ( QABS( crossPos.y() - target.y() ) <= QABS( dy ) ) {
		newPos.setY( target.y() );
		doneY = TRUE;
	}

	if ( doneX && doneY ) {
		penPos = QPoint();
		timer->stop();
	}

	moveCrosshair( newPos );
#endif
}

//#endif // _WS_QWS_
