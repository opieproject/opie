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
#include "battery.h"
#include "batterystatus.h"

#include <qpe/power.h>

#include <qpainter.h>
#include <qtimer.h>


BatteryMeter::BatteryMeter( QWidget *parent )
    : QWidget( parent ), charging(false)
{
    ps = new PowerStatus;
    startTimer( 10000 );
    setFixedHeight(12);
    chargeTimer = new QTimer( this );
    connect( chargeTimer, SIGNAL(timeout()), this, SLOT(chargeTimeout()) );
    timerEvent(0);
}

BatteryMeter::~BatteryMeter()
{
    delete ps;
}

QSize BatteryMeter::sizeHint() const
{
    return QSize(10,12);
}

void BatteryMeter::mouseReleaseEvent( QMouseEvent *)
{
    if ( batteryView && batteryView->isVisible() ) {
	delete (QWidget *) batteryView;
    } else {
	if ( !batteryView )
	    batteryView = new BatteryStatus( ps );
	batteryView->showMaximized();
	batteryView->raise();
	batteryView->show();
    }
}

void BatteryMeter::timerEvent( QTimerEvent * )
{
    PowerStatus prev = *ps;

    *ps = PowerStatusManager::readStatus();

    if ( prev != *ps ) {
	percent = ps->batteryPercentRemaining();
	if ( !charging && ps->batteryStatus() == PowerStatus::Charging && percent < 0 ) {
	    percent = 0;
	    charging = true;
	    chargeTimer->start( 500 );
	} else if ( charging && ps->batteryStatus() != PowerStatus::Charging ) {
	    charging = false;
	    chargeTimer->stop();
	    if ( batteryView )
		batteryView->updatePercent( percent );
	}
	repaint(FALSE);
	if ( batteryView )
	    batteryView->repaint();
    }
}

void BatteryMeter::chargeTimeout()
{
    percent += 20;
    if ( percent > 100 )
	percent = 0;

    repaint(FALSE);
    if ( batteryView )
	batteryView->updatePercent( percent );
}

void BatteryMeter::paintEvent( QPaintEvent* )
{
    QPainter p(this);

    QColor c;
    QColor darkc;
    QColor lightc;
    if ( ps->acStatus() == PowerStatus::Offline ) {
	c = blue.light(120);
	darkc = c.dark(120);
	lightc = c.light(140);
    } else if ( ps->acStatus() == PowerStatus::Online ) {
	c = green.dark(130);
	darkc = c.dark(120);
	lightc = c.light(180);
    } else {
	c = red;
	darkc = c.dark(120);
	lightc = c.light(160);
    }

    int w = 6;
    int h = height()-3;
    int pix = (percent * h) / 100;
    int y2 = height() - 2;
    int y = y2 - pix;
    int x1 = (width() - w) / 2;

    p.setPen(QColor(80,80,80));
    p.drawLine(x1+w/4,0,x1+w/4+w/2,0);
    p.drawRect(x1,1,w,height()-1);
    p.setBrush(c);

    int extra = ((percent * h) % 100)/(100/4);

#define Y(i) ((i<=extra)?y-1:y)
#define DRAWUPPER(i) if ( Y(i) >= 2 ) p.drawLine(i+x1,2,i+x1,Y(i));
    p.setPen(  gray );
    DRAWUPPER(1);
    DRAWUPPER(3);
    p.setPen( gray.light(130) );
    DRAWUPPER(2);
    p.setPen( gray.dark(120) );
    DRAWUPPER(4);

#define DRAW(i) { if ( Y(i) < y2 ) p.drawLine(i+x1,Y(i)+1,i+x1,y2); }
    p.setPen( c );
    DRAW(1);
    DRAW(3);
    p.setPen( lightc );
    DRAW(2);
    p.setPen(darkc);
    DRAW(4);
}

