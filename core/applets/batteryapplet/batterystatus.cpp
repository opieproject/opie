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
#include "batterystatus.h"

#include <qpe/power.h>

#include <qpainter.h>
#include <qpushbutton.h> 
#include <qdrawutil.h> 


BatteryStatus::BatteryStatus( const PowerStatus *p, QWidget *parent )
    : QWidget( parent, 0, WDestructiveClose), ps(p)
{
    setCaption( tr("Battery Status") );
    QPushButton *pb = new QPushButton( tr("Close"), this );
    pb->move( 70, 220 );
    pb->show();
    connect( pb, SIGNAL( clicked() ), this, SLOT( close() ) );
    percent = ps->batteryPercentRemaining();
    show();
}

BatteryStatus::~BatteryStatus()
{
}

void BatteryStatus::updatePercent( int pc )
{
    percent = pc;
    repaint(FALSE);
}

void BatteryStatus::drawSegment( QPainter *p, const QRect &r, const QColor &topgrad, const QColor &botgrad, const QColor &highlight, int hightlight_height )
{
    int h1, h2, s1, s2, v1, v2, ng = r.height(), hy = ng*30/100, hh = hightlight_height;
    topgrad.hsv( &h1, &s1, &v1 );
    botgrad.hsv( &h2, &s2, &v2 );
    for ( int j = 0; j < hy-2; j++ ) {
	p->setPen( QColor( h1 + ((h2-h1)*j)/(ng-1), s1 + ((s2-s1)*j)/(ng-1),
		    v1 + ((v2-v1)*j)/(ng-1),  QColor::Hsv ) );
	p->drawLine( r.x(), r.top()+hy-2-j, r.x()+r.width(), r.top()+hy-2-j );
    }
    for ( int j = 0; j < hh; j++ ) {
	p->setPen( highlight );
	p->drawLine( r.x(), r.top()+hy-2+j, r.x()+r.width(), r.top()+hy-2+j );
    }
    for ( int j = 0; j < ng-hy-hh; j++ ) {
	p->setPen( QColor( h1 + ((h2-h1)*j)/(ng-1), s1 + ((s2-s1)*j)/(ng-1),
		    v1 + ((v2-v1)*j)/(ng-1),  QColor::Hsv ) );
	p->drawLine( r.x(), r.top()+hy+hh-2+j, r.x()+r.width(), r.top()+hy+hh-2+j );
    }
}

void BatteryStatus::paintEvent( QPaintEvent * )
{
    QPainter p(this);
    QString text;
    if ( ps->batteryStatus() == PowerStatus::Charging ) {
	text = tr("Charging");
    } else if ( ps->batteryPercentAccurate() ) {
	text.sprintf( tr("Percentage battery remaining") + ": %i%%", percent );
    } else {
	text = tr("Battery status: ");
	switch ( ps->batteryStatus() ) {
	    case PowerStatus::High:
		text += tr("Good");
		break;
	    case PowerStatus::Low:
		text += tr("Low");
		break;
	    case PowerStatus::VeryLow:
		text += tr("Very Low");
		break;
	    case PowerStatus::Critical:
		text += tr("Critical");
		break;
	    default: // NotPresent, etc.
		text += tr("Unknown");
	}
    }
    p.drawText( 10, 120, text );
    if ( ps->acStatus() == PowerStatus::Backup )
	p.drawText( 10, 150, tr("On backup power") );
    else if ( ps->acStatus() == PowerStatus::Online )
	p.drawText( 10, 150, tr("Power on-line") );
    else if ( ps->acStatus() == PowerStatus::Offline )
	p.drawText( 10, 150, tr("External power disconnected") );

    if ( ps->batteryTimeRemaining() >= 0 ) {
	text.sprintf( tr("Battery time remaining") + ": %im %02is",
	    ps->batteryTimeRemaining() / 60, ps->batteryTimeRemaining() % 60 );
	p.drawText( 10, 180, text );
    }

    QColor c;
    QColor darkc;
    QColor lightc;
    if ( ps->acStatus() == PowerStatus::Offline ) {
	c = blue.light(120);
	darkc = c.dark(280);
	lightc = c.light(145);
    } else if ( ps->acStatus() == PowerStatus::Online ) {
	c = green.dark(130);
	darkc = c.dark(200);
	lightc = c.light(220);
    } else {
	c = red;
	darkc = c.dark(280);
	lightc = c.light(140);
    }
    if ( percent < 0 )
	return;

    int percent2 = percent * 2;
    p.setPen( black );
    qDrawShadePanel( &p,   9, 30, 204, 39, colorGroup(), TRUE, 1, NULL);
    qDrawShadePanel( &p, 212, 37,  12, 24, colorGroup(), TRUE, 1, NULL);
    drawSegment( &p, QRect( 10, 30, percent2, 40 ), lightc, darkc, lightc.light(115), 6 );
    drawSegment( &p, QRect( 11 + percent2, 30, 200 - percent2, 40 ), white.light(80), black, white.light(90), 6 );
    drawSegment( &p, QRect( 212, 37, 10, 25 ), white.light(80), black, white.light(90), 2 );
}

