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

#include <stdio.h>

#include <qfile.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include "load.h"

LoadInfo::LoadInfo( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    QVBoxLayout *vb = new QVBoxLayout( this, 6 );

    QString cpuInfo = getCpuInfo();
    if ( !cpuInfo.isNull() )
	vb->addWidget( new QLabel( cpuInfo, this ) );
    vb->addWidget( new Load( this ), 100 );
    QLabel *l = new QLabel( this );
    l->setPixmap( makeLabel( red, tr("Application CPU usage (%)") ) );
    vb->addWidget( l, 1 );
    l = new QLabel( this );
    l->setPixmap( makeLabel( green, tr("System CPU usage (%)") ) );
    vb->addWidget( l, 1 );
    vb->addStretch(50);

    QWhatsThis::add( this, tr( "This page shows how much this device's processor is being used." ) );
}

QPixmap LoadInfo::makeLabel( const QColor &col, const QString &text )
{
    int h = fontMetrics().height();
    QPixmap pm( 20 + fontMetrics().width( text ), h );
    QPainter p( &pm );
    p.fillRect( pm.rect(), colorGroup().background() );
    p.fillRect( 0, h/2-4, 18, h/2+3, black );
    p.setPen( col );
    p.drawLine( 2, h/2, 15, h/2 );
    p.setPen( colorGroup().text() );
    p.drawText( 20, fontMetrics().ascent(), text );

    return pm;
}

QString LoadInfo::getCpuInfo()
{
    bool haveInfo = FALSE;
    QString info = tr("Type: ");
    QFile f( "/proc/cpuinfo" );
    if ( f.open( IO_ReadOnly ) ) {
	QTextStream ts( &f );

	while ( !ts.atEnd() ) {
	    QString s = ts.readLine();
	    if ( s.find( "model name" ) == 0 ) {
		info += s.mid( s.find( ':' ) + 2 );
		haveInfo = TRUE;
	    } else if ( s.find( "cpu MHz" ) == 0 ) {
		double mhz = s.mid( s.find( ':' ) + 2 ).toDouble();
		info += " " + QString::number( mhz, 'f', 0 );
		info += "MHz";
		break;
	    } else if ( s.find( "Processor" ) == 0 ) {
		info += s.mid( s.find( ':' ) + 2 );
		haveInfo = TRUE;
		break;
#ifdef __MIPSEL__
	    } else if ( s.find( "cpu model" ) == 0 ) {
		info += " " + s.mid( s.find( ':' ) + 2 );
		break;
	    } else if ( s.find( "cpu" ) == 0 ) {
		info += s.mid( s.find( ':' ) + 2 );
		haveInfo = TRUE;
#endif
	    }
	}
    }

    if ( !haveInfo )
	info = QString();

    return info;
}

Load::Load( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    setMinimumHeight( 30 );
    setBackgroundColor( black );
    points = 100;
    setMinimumWidth( points );
    userLoad = new double [points];
    systemLoad = new double [points];
    for ( int i = 0; i < points; i++ ) {
	userLoad[i] = 0.0;
	systemLoad[i] = 0.0;
    }
    maxLoad = 1.3;
    QTimer *timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), SLOT(timeout()) );
    timer->start( 2000 );
    gettimeofday( &last, 0 );
    first = TRUE;
    timeout();
}

void Load::paintEvent( QPaintEvent * )
{
    QPainter p( this );

    int h = height() - 5;

    int mult = (int)(h / maxLoad);

    p.setPen( gray );
    p.drawLine( 0, h - mult, width(), h - mult );
    p.drawText( 0, h - mult, "100" );
    p.drawText( 0, h, "0" );

    p.setPen( green );
    for ( int i = 1; i < points; i++ ) {
	int x1 = (i - 1) * width() / points;
	int x2 = i * width() / points;
	p.drawLine( x1, h - systemLoad[i-1] * mult,
		    x2, h - systemLoad[i] * mult );
    }

    p.setPen( red );
    for ( int i = 1; i < points; i++ ) {
	int x1 = (i - 1) * width() / points;
	int x2 = i * width() / points;
	p.drawLine( x1, h - userLoad[i-1] * mult,
		    x2, h - userLoad[i] * mult );
    }
}

void Load::timeout()
{
    int user;
    int usernice;
    int sys;
    int idle;
    FILE *fp;
    fp = fopen( "/proc/stat", "r" );
    fscanf( fp, "cpu %d %d %d %d", &user, &usernice, &sys, &idle );
    fclose( fp );
    struct timeval now;
    gettimeofday( &now, 0 );
    int tdiff = now.tv_usec - last.tv_usec;
    tdiff += (now.tv_sec - last.tv_sec) * 1000000;
    tdiff /= 10000;

    int udiff = user - lastUser;
    int sdiff = sys - lastSys;
    if ( tdiff > 0 ) {
	double uload = (double)udiff / (double)tdiff;
	double sload = (double)sdiff / (double)tdiff;
	if ( !first ) {
	    for ( int i = 1; i < points; i++ ) {
		userLoad[i-1] = userLoad[i];
		systemLoad[i-1] = systemLoad[i];
	    }
	    userLoad[points-1] = uload;
	    systemLoad[points-1] = sload;
//	    scroll( -width()/points, 0, QRect( 0, 0, width() - width()/points + 1, height() ) );
	    repaint( TRUE );
	    double ml = 1.3;
	    /*
	    for ( int i = 0; i < points; i++ ) {
		if ( userLoad[i] > ml )
		    ml = userLoad[i];
	    }
	    */
	    if ( maxLoad != ml ) {
		maxLoad = ml;
		update();
	    }
	}

	last = now;
	lastUser = user;
	lastSys = sys;
	first = FALSE;
    } else if ( tdiff < 0 ) {
	last = now;
    }
}

