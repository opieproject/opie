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

#include "clock.h"

#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

#include <qmainwindow.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qfile.h>
#include <stdlib.h>


LauncherClock::LauncherClock( QWidget *parent ) : QLabel( parent )
{
    // If you want a sunken border around the clock do this:
    // setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setFont( QFont( "Helvetica", 10, QFont::Normal ) );
    connect( qApp, SIGNAL( timeChanged() ), this, SLOT( updateTime( ) ) );
    connect( qApp, SIGNAL( clockChanged( bool ) ),
       this, SLOT( slotClockChanged( bool ) ) );
    readConfig();
    timerId = 0;
    timerEvent( 0 );
    show();
}

void LauncherClock::readConfig() {
    Config config( "qpe" );
    config.setGroup( "Time" );
    ampmFormat = config.readBoolEntry( "AMPM", TRUE );
    config.setGroup( "Date" );
    format = config.readNumEntry("ClockApplet",0);
}

void LauncherClock::mouseReleaseEvent( QMouseEvent * )
{
	QCString setTimeApp;
	if (QFile::exists(QPEApplication::qpeDir()+"bin/netsystemtime"))
 		setTimeApp="netsystemtime";
 	else setTimeApp="systemtime";
	QCopEnvelope e("QPE/Application/"+setTimeApp, "setDocument(QString)");
  QString lf = QString::null;

	e << lf;

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

void LauncherClock::slotClockChanged( bool pm )
{
    readConfig();
    updateTime();
}
