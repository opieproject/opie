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

#include "calibrate.h"

#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>

#include <qpainter.h>
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>


Calibrate::Calibrate( QWidget* parent, const char * name, WFlags wf ) :
		QDialog( parent, name, TRUE, wf | WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop )
{
	showCross = true;
	const int offset = 50;
	QRect desk = qApp->desktop() ->geometry();
	setGeometry( 0, 0, desk.width(), desk.height() );
	if ( desk.height() < 250 ) {
		int w = desk.height() / 3;
		logo.convertFromImage( Resource::loadImage( "logo/opielogo" ).smoothScale( w, w ) );
	} else {
		logo = Resource::loadPixmap( "logo/opielogo" );
	}

	cal.xfb[0] = offset;
        cal.yfb[0] = offset;
	cal.xfb[1] = desk.width()-offset;
        cal.yfb[1] = offset;
	cal.xfb[2] = desk.width()-offset;
        cal.yfb[2] = desk.height()-offset;
	cal.xfb[3] = offset;
        cal.yfb[3] = desk.height()-offset;
	cal.xfb[4] = desk.width()/2;
        cal.yfb[4] = desk.height()/2;

	reset();
}

Calibrate::~Calibrate()
{
	store();
}

void Calibrate::reset()
{
	penPos = QPoint();
        location = 0;
	samples = 0;
	crossPos = QPoint(cal.xfb[0], cal.yfb[0]);
}


void Calibrate::show()
{
	grabMouse();
	QWSServer::mouseHandler() ->clearCalibration();
	QDialog::show();
}

void Calibrate::store()
{
#ifndef QT_NO_TEXTSTREAM
    QFile file("/etc/pointercal");
    if ( file.open( IO_WriteOnly ) ) {
	QTextStream t( &file );
	t << cal.a[1] << " " << cal.a[2] << " " << cal.a[0] << " ";
	t << cal.a[4] << " " << cal.a[5] << " " << cal.a[3] << " ";
	t << cal.a[6] << endl;
    } else	
#endif
    {
	qDebug("Could not save calibration");
    }
}

void Calibrate::hide()
{
	if ( isVisible() )
		store();
	// hack - calibrate is a launcher dialog, but treated like a standalone app
	{
		QCopEnvelope e( "QPE/System", "closing(QString)" );
		e << QString ( "calibrate" );
	}
	QDialog::hide();
}

QPoint Calibrate::fromDevice( const QPoint &p )
{

	QPoint np = qt_screen->mapFromDevice ( p, QSize( qt_screen->deviceWidth ( ), qt_screen->deviceHeight() ) );
	qDebug("Calibrate::fromDevice(%d,%d) -> %d,%d", p.x(), p.y(), np.x(), np.y());
	return np;
}

bool Calibrate::performCalculation(void)
{
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

	//qDebug("Top left : X = %4d Y = %4d", cal.x[0], cal.y[0]);
	//qDebug("Top right: X = %4d Y = %4d", cal.x[1], cal.y[1]);
	//qDebug("Bot left : X = %4d Y = %4d", cal.x[2], cal.y[2]);
	//qDebug("Bot right: X = %4d Y = %4d", cal.x[3], cal.y[3]);
	//qDebug("Middle: X = %4d Y = %4d", cal.x[4], cal.y[4]);

	// Get sums for matrix
	n = x = y = x2 = y2 = xy = 0;
	for(j=0;j<5;j++) {
		n += 1.0;
		x += (float)cal.x[j];
		y += (float)cal.y[j];
		x2 += (float)(cal.x[j]*cal.x[j]);
		y2 += (float)(cal.y[j]*cal.y[j]);
		xy += (float)(cal.x[j]*cal.y[j]);
	}

	// Get determinant of matrix -- check if determinant is too small
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < 0.1 && det > -0.1) {
		qDebug("determinant is too small -- %f",det);
		return false;
	}

	// Get elements of inverse matrix
	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

	// Get sums for x calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal.xfb[j];
		zx += (float)(cal.xfb[j]*cal.x[j]);
		zy += (float)(cal.xfb[j]*cal.y[j]);
	}

	// Now multiply out to get the calibration for framebuffer x coord
	cal.a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal.a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal.a[2] = (int)((c*z + f*zx + i*zy)*(scaling));

	qDebug("%f %f %f",(a*z + b*zx + c*zy), (b*z + e*zx + f*zy), (c*z + f*zx + i*zy));

	// Get sums for y calibration
	z = zx = zy = 0;
	for (j=0;j<5;j++) {
		z += (float)cal.yfb[j];
		zx += (float)(cal.yfb[j]*cal.x[j]);
		zy += (float)(cal.yfb[j]*cal.y[j]);
	}

	// Now multiply out to get the calibration for framebuffer y coord
	cal.a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal.a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal.a[5] = (int)((c*z + f*zx + i*zy)*(scaling));

	qDebug("%f %f %f",(a*z + b*zx + c*zy), (b*z + e*zx + f*zy), (c*z + f*zx + i*zy));


	// If we got here, we're OK, so assign scaling to a[6] and return
	cal.a[6] = (int) scaling;

	qDebug("Calibration constants: %d %d %d %d %d %d %d",
		cal.a[0], cal.a[1], cal.a[2],
		cal.a[3], cal.a[4], cal.a[5],
		cal.a[6]);

	return true;
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
		p.drawPixmap( ( width() - logo.width() ) / 2, 0, logo );
	}

	y = height() / 2 + 15;

	p.drawText( 0, y + height() / 8, width(), height() - y, AlignHCenter,
	            tr( "Um den Touchscreen zu kalibrieren bitte\n"
	                "mit dem Stift auf die Fadenkreuze tippen." ) );

	QFont f = p.font();
	f.setBold( TRUE );
	p.setFont( f );
	p.drawText( 0, y, width(), height() - y, AlignHCenter | WordBreak,
	            tr( "Willkommen beim Ramses" ) );

	if ( showCross ) {
		p.drawRect( crossPos.x() - 1, crossPos.y() - 8, 2, 7 );
		p.drawRect( crossPos.x() - 1, crossPos.y() + 1, 2, 7 );
		p.drawRect( crossPos.x() - 8, crossPos.y() - 1, 7, 2 );
		p.drawRect( crossPos.x() + 1, crossPos.y() - 1, 7, 2 );
	}
}

void Calibrate::mouseMoveEvent( QMouseEvent *e)
{
qDebug("Calibrate::mouseMoveEvent(%d,%d)", e->pos().x(), e->pos().y());
	penPos += e->pos();
	samples++;
}

void Calibrate::mouseReleaseEvent( QMouseEvent *e )
{
qDebug("Calibrate::mouseReleaseEvent(%d,%d)", e->pos().x(), e->pos().y());
	penPos += e->pos();
	samples++;
	penPos /= samples;

	if (location < 5) {
qDebug(" entering %d samples as penPos %d,%d", samples, penPos.x(), penPos.y() );
		cal.x[location] = e->pos().x();
		cal.y[location] = e->pos().y();
		location++;
		penPos  = QPoint();
		samples = 0;
	}
	if (location < 5) {
		moveCrosshair(QPoint(cal.xfb[location], cal.yfb[location]));
	} else {
		showCross = FALSE;
		repaint( crossPos.x() - 8, crossPos.y() - 8, 16, 16 );
		if (performCalculation()) {
			hide();
			emit accept();
		} else {
			qDebug("nochmal");
			reset();
		}
	}
}

#if 0

# ts_calibrate
xres = 240, yres = 320
Top left : X =  822 Y =  750
Top right: X =  806 Y =  242
Bot right: X =  209 Y =  255
Bot left : X =  216 Y =  754
Middle: X =  518 Y =  504
261.248383 -0.003901 -0.277929
343.781494 -0.365597 0.008400
Calibration constants: 17121174 -255 -18214 22530064 -23959 550 65536
# cat /etc/pointercal
-255 -18214 17121174 -23959 550 22530064 65536


# calibrate -qws
xres = 240, yres = 320
Top left : X =  814 Y =  759
Top right: X =  817 Y =  247
Bot left : X =  205 Y =  270
Bot right: X =  200 Y =  760
Middle: X =  511 Y =  507
264.857605 -0.005462 -0.279346
344.296326 -0.358942 -0.002853
Calibration constants: 17357708 -357 -18307 22563804 -23523 -186 65536
# cat /etc/pointercal
-357 -18307 17357708 -23523 -186 22563804 65536

#endif
