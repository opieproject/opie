/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
** Copyright (C) 2002 Trolltech AS.  All rights reserved.
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
// merged in the StyleFix. Applets should raise Popup!
// by zecke@handhelds.org 6th of may 2004

#include "clock.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/otaskbarapplet.h>

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>
#include <qpe/resource.h>

#include <qpopupmenu.h>

using namespace Opie::Core;
using namespace Opie::Ui;

LauncherClock::LauncherClock( QWidget *parent ) : QLabel( parent )
{
    // If you want a sunken border around the clock do this:
    // setFrameStyle( QFrame::Panel | QFrame::Sunken );
    //setFont( QFont( "Helvetica", , QFont::Normal ) );
    connect( qApp, SIGNAL( timeChanged() ), this, SLOT( updateTime() ) );
    connect( qApp, SIGNAL( clockChanged(bool) ),
       this, SLOT( slotClockChanged(bool) ) );
    readConfig();
    timerId = 0;
    timerEvent( 0 );
    show();
}

int LauncherClock::position()
{
    return 10;
}

void LauncherClock::readConfig() {
    Config config( "qpe" );
    config.setGroup( "Time" );
    ampmFormat = config.readBoolEntry( "AMPM", TRUE );
    config.setGroup( "Date" );
    format = config.readNumEntry("ClockApplet",0);
}

void LauncherClock::mousePressEvent( QMouseEvent * )
{
    QPopupMenu *menu = new QPopupMenu(this);
    menu->insertItem(tr("Set time..."), 0);
    menu->insertSeparator();
    menu->insertItem(tr("Clock.."), 1);


    Config config( "Clock" );
    config.setGroup( "Daily Alarm" );
    bool alarmOn = config.readBoolEntry("Enabled", FALSE);
    menu->insertItem(Resource::loadIconSet(alarmOn?"clockapplet/smallalarm":"clockapplet/smallalarm_off" ),
                     tr("Alarm..."), 2);


    /* FIXME use OTaskBarApplet ### */
    QPoint curPos = mapToGlobal( QPoint(0,0) );
    QSize sh = menu->sizeHint();
    switch (menu->exec( curPos-QPoint((sh.width()-width())/2,sh.height()) )) {
	case 0:
	    Global::execute( "systemtime" );
	    break;
	case 1: {
	    QCopEnvelope e("QPE/Application/clock", "showClock()" );
	    }
	    break;
	case 2: {
		QCopEnvelope e("QPE/Application/clock", "editDailyAlarm()" );
	    }
	    break;
	default:
	    break;
    }
    delete menu;
}


void LauncherClock::timerEvent( QTimerEvent *e )
{
    if ( !e || e->timerId() == timerId ) {
  killTimer( timerId );
  changeTime();
  QTime t = QTime::currentTime();
  int ms = (60 - t.second())*1000 - t.msec();
  timerId = startTimer( ms );
    } else {
  QLabel::timerEvent( e );
    }
}

void LauncherClock::updateTime( void )
{
    changeTime();
}

void LauncherClock::changeTime( void )
{
    QTime tm = QDateTime::currentDateTime().time();
    QString s;
    if( ampmFormat ) {
  int hour = tm.hour();
  if (hour == 0)
      hour = 12;
  if (hour > 12)
      hour -= 12;
  s.sprintf( "%2d:%02d %s", hour, tm.minute(),
       (tm.hour() >= 12) ? "PM" : "AM" );
    } else
  s.sprintf( "%2d:%02d", tm.hour(), tm.minute() );

    if (format==1) {
  QDate dm = QDate::currentDate();
  QString d;
  d.sprintf("%d/%d ", dm.day(), dm.month());
  setText( d+s );
    } else if (format==2) {
  QDate dm = QDate::currentDate();
  QString d;
  d.sprintf("%d/%d ", dm.month(), dm.day());
  setText( d+s );
    } else {
  setText( s );
    }
}

void LauncherClock::slotClockChanged( bool  )
{
    readConfig();
    updateTime();
}


EXPORT_OPIE_APPLET_v1( LauncherClock )

