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
#include "setAlarm.h"

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>
#include <qpe/timestring.h>
#include <qpe/alarmserver.h>
#include <qpe/sound.h>
#include <qpe/resource.h>
#include <qsound.h>
#include <qtimer.h>

#include <qlcdnumber.h>
#include <qslider.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qdatetime.h>

#include <math.h>

const double deg2rad = 0.017453292519943295769; // pi/180
const int sw_prec = 2;

static void toggleScreenSaver( bool on )
{
    QCopEnvelope e("QPE/System", "setScreenSaverMode(int)" );
    e << (on ? QPEApplication::Enable: QPEApplication::DisableSuspend );
}

Clock::Clock( QWidget * parent, const char * name, WFlags f )
    : QVBox( parent, name , f )
{
    setSpacing( 4 );
    setMargin( 1 );

    Config config( "qpe" );
    config.setGroup("Time");
    ampm = config.readBoolEntry( "AMPM", TRUE );

    snoozeBtn = new QPushButton ( this);
    snoozeBtn->setText( tr( "Snooze" ) );

    aclock = new AnalogClock( this );
    aclock->display( QTime::currentTime() );
    aclock->setLineWidth( 2 );

    QHBox *hb = new QHBox( this );
    hb->setMargin( 0 );
    QWidget *space = new QWidget( hb );
    lcd = new QLCDNumber( hb );
    lcd->setSegmentStyle( QLCDNumber::Flat );
    lcd->setFrameStyle( QFrame::NoFrame );
    lcd->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
    lcd->setFixedHeight( 23 );

    ampmLabel = new QLabel( tr("PM"), hb );
    ampmLabel->setFont( QFont( "Helvetica", 14, QFont::Bold ) );
    ampmLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred ) );
    ampmLabel->setAlignment( AlignLeft | AlignBottom );
    space = new QWidget( hb );

    date = new QLabel( this );
    date->setAlignment( AlignHCenter | AlignVCenter );
    date->setFont( QFont( "Helvetica", 14, QFont::Bold ) );
    date->setText( TimeString::longDateString( QDate::currentDate() ) );

    QWidget *controls = new QWidget( this );
    QGridLayout *gl = new QGridLayout( controls, 2, 2, 6, 4 );

    QButtonGroup *grp = new QButtonGroup( controls );
    grp->setRadioButtonExclusive( true );
    grp->hide();

    clockRB = new QRadioButton ( tr( "Clock" ), controls  );
    gl->addWidget( clockRB, 0, 0 );
    grp->insert( clockRB );

    swatchRB = new QRadioButton ( tr( "Stopwatch" ), controls );
    gl->addWidget( swatchRB, 1, 0 );
    grp->insert( swatchRB );

    connect( grp, SIGNAL(clicked(int)), this, SLOT(modeSelect(int)) );
    grp->setButton( 0 );

    set = new QPushButton ( controls );
    set->setMaximumSize(50,30);
    gl->addWidget( set, 0, 1 );
    set->setText( tr( "Start" ) );
    set->setEnabled( FALSE );
    grp->insert( set );

    reset = new QPushButton ( controls );
    gl->addWidget( reset, 1, 1 );
    reset->setText( tr( "Reset" ) );
    reset->setEnabled( FALSE );
    grp->insert( reset );

    alarmOffBtn = new QPushButton ( controls );
//    alarmOffBtn->setMaximumSize(60,30);
    gl->addWidget( alarmOffBtn, 0, 2 );

    alarmBtn = new QPushButton ( controls );
      //  alarmBtn->setMaximumSize(60,30);
    gl->addWidget( alarmBtn, 1, 2 );
    alarmBtn->setText( tr( "Set Alarm" ) );

    connect( set, SIGNAL( pressed() ), SLOT( slotSet() ) );
    connect( reset, SIGNAL( clicked() ), SLOT( slotReset() ) );

    connect( alarmBtn, SIGNAL( clicked() ), SLOT( slotSetAlarm() ) );
    connect( snoozeBtn, SIGNAL( clicked() ), SLOT( slotSnooze() ) );
    connect( alarmOffBtn, SIGNAL( clicked() ), SLOT( slotToggleAlarm() ) );

    connect( qApp, SIGNAL(appMessage(const QCString&, const QByteArray&)),
       this, SLOT(appMessage(const QCString&, const QByteArray&)) );

    t = new QTimer( this );
    connect( t, SIGNAL( timeout() ), SLOT( updateClock() ) );
    t->start( 1000 );

    connect( qApp, SIGNAL( timeChanged() ), SLOT( updateClock() ) );

    swatch_running = FALSE;
    swatch_totalms = 0;

    connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(changeClock(bool)) );

    QString tmp = config.readEntry("clockAlarmHour", "");
    bool ok;
    hour = tmp.toInt(&ok,10);
    tmp = config.readEntry("clockAlarmMinute","");
    minute = tmp.toInt(&ok,10);

    if( config.readEntry("clockAlarmSet","FALSE") == "TRUE") {
        alarmOffBtn->setText( tr( "Alarm Is On" ) );
        alarmBool=TRUE;
        snoozeBtn->show();
    } else {
        alarmOffBtn->setText( tr( "Alarm Is Off" ) );
        alarmBool=FALSE;
        snoozeBtn->hide();
    }
    
    QTimer::singleShot( 0, this, SLOT(updateClock()) );
    modeSelect(0);
}

Clock::~Clock()
{
    toggleScreenSaver( true );
}

void Clock::updateClock()
{
    if ( clockRB->isChecked() ) {
  QTime tm = QDateTime::currentDateTime().time();
  QString s;
  if ( ampm ) {
      int hour = tm.hour();
      if (hour == 0) 
    hour = 12;
      if (hour > 12)
    hour -= 12;
      s.sprintf( "%2d%c%02d", hour, ':', tm.minute() );
      ampmLabel->setText( (tm.hour() >= 12) ? "PM" : "AM" );
      ampmLabel->show();
  } else {
      s.sprintf( "%2d%c%02d", tm.hour(), ':', tm.minute() );
      ampmLabel->hide();
  }
  lcd->display( s );
  lcd->repaint( FALSE );
  aclock->display( QTime::currentTime() );
  date->setText( TimeString::longDateString( QDate::currentDate() ) );
    } else {
  QTime swatch_time;
  QString lcdtext;
  int totalms = swatch_totalms;
  if ( swatch_running )
      totalms += swatch_start.elapsed();
  swatch_time = QTime(0,0,0).addMSecs(totalms);
  QString d = swatch_running ? QString("    ")
        : QString::number(totalms%1000+1000);
  lcdtext = swatch_time.toString() + "." + d.right(3).left(sw_prec);
  lcd->display( lcdtext );
  lcd->repaint( FALSE );
  aclock->display( swatch_time );
  date->setText( TimeString::longDateString( QDate::currentDate() ) );
    }
}

void Clock::changeClock( bool a )
{
    ampm = a;
    updateClock();
}

void Clock::clearClock( void )
{
    lcd->display( QTime( 0,0,0 ).toString() );
    aclock->display( QTime( 0,0,0 ) );
}

void Clock::slotSet()
{
    if  ( t->isActive() ) {
  swatch_totalms += swatch_start.elapsed();
  set->setText( tr( "Start" ) );
  t->stop();
  swatch_running = FALSE;
  toggleScreenSaver( TRUE );
  updateClock();
    } else {
  swatch_start.start();
  set->setText( tr( "Stop" ) );
        t->start( 1000 );
  swatch_running = TRUE;
  // disable screensaver while stop watch is running
  toggleScreenSaver( FALSE );
    }
}

void Clock::slotReset()
{
    t->stop();
    swatch_start.start();
    swatch_totalms = 0;

    if (swatch_running )
  t->start(1000);

    updateClock();
}

void Clock::modeSelect( int m )
{
    if ( m ) {
  lcd->setNumDigits( 8+1+sw_prec );
  lcd->setMinimumWidth( lcd->sizeHint().width() );
  set->setEnabled( TRUE );
  reset->setEnabled( TRUE );
  ampmLabel->hide();

  if ( !swatch_running )
      t->stop();
    } else {
  lcd->setNumDigits( 5 );
  lcd->setMinimumWidth( lcd->sizeHint().width() );
  set->setEnabled( FALSE );
  reset->setEnabled( FALSE );
  t->start(1000);
    }
    updateClock();
}

//this sets the alarm time
void Clock::slotSetAlarm() 
{
    if( !snoozeBtn->isHidden())
        slotToggleAlarm();
    Set_Alarm *setAlarmDlg;
    setAlarmDlg = new Set_Alarm(this,"SetAlarm", TRUE);
    int result = setAlarmDlg->exec();
    if(result == 1) {
        Config config( "qpe" );
        config.setGroup("Time");
        QString tmp;
        hour =  setAlarmDlg->Hour_Slider->value();
        minute = setAlarmDlg->Minute_Slider->value();
        snoozeTime=setAlarmDlg->SnoozeSlider->value();
        if(ampm) {
            if(setAlarmDlg->Pm_RadioButton->isChecked() && hour < 12 )
                hour+=12;
        }
        config.writeEntry("clockAlarmHour", tmp.setNum( hour ),10);
        config.writeEntry("clockAlarmMinute",tmp.setNum( minute ),10);
        config.writeEntry("clockAlarmSnooze",tmp.setNum( snoozeTime ),10);
        config.write();
    } 
}

void Clock::slotSnooze()
{
    bSound=FALSE;
    int warn;
    QTime t = QTime::currentTime();
    QDateTime whenl( when.date(), t.addSecs( snoozeTime*60));
    when=whenl;
    AlarmServer::addAlarm( when,
                             "QPE/Application/clock",
                             "alarm(QDateTime,int)", warn );

}

//toggles alarm on/off
void Clock::slotToggleAlarm() 
{
    Config config( "qpe" );
    config.setGroup("Time");
    if(alarmBool) {
        config.writeEntry("clockAlarmSet","FALSE");
        alarmOffBtn->setText( tr( "Alarm Is Off" ) );
        snoozeBtn->hide();
        alarmBool=FALSE;
        alarmOff();
    } else {
        config.writeEntry("clockAlarmSet","TRUE");
        alarmOffBtn->setText( tr( "Alarm Is On" ) );
        snoozeBtn->show();
        alarmBool=TRUE;
        alarmOn();
    }
    config.write();
}

void Clock::alarmOn()
{
    QDate d = QDate::currentDate();
    QTime tm((int)hour,(int)minute,0);
    qDebug("Time set "+tm.toString());
    QTime t = QTime::currentTime();
    if( t > tm)
        d = d.addDays(1);
    int warn;
    QDateTime whenl(d,tm);
    when=whenl;
      AlarmServer::addAlarm( when,
                             "QPE/Application/clock",
                             "alarm(QDateTime,int)", warn );
      QMessageBox::message("Note","Alarm is set for:\n"+ whenl.toString());
}

void Clock::alarmOff()
{
    int warn;
    bSound=FALSE;
        AlarmServer::deleteAlarm( when,
                               "QPE/Application/clock",
                               "alarm(QDateTime,int)", warn );
      qDebug("Alarm Off "+ when.toString());

}

void Clock::appMessage(const QCString& msg, const QByteArray& data)
{
    int stopTimer = 0;
    int timerStay = 5000;
    bSound=TRUE;
   if ( msg == "alarm(QDateTime,int)" ) {
       Sound::soundAlarm();
       stopTimer = startTimer( timerStay);
   }
}

void Clock::timerEvent( QTimerEvent *e )
{
    static int stop = 0;
    if ( stop < 10 && bSound) {
        Sound::soundAlarm();
        stop++;
    } else {
        stop = 0;
        killTimer( e->timerId() );
    }
}


QSizePolicy AnalogClock::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void AnalogClock::drawContents( QPainter *p )
{
    QRect r = contentsRect();
    QPoint center( r.x() + r.width() / 2, r.y() + r.height() / 2 );

    QPoint l1( r.x() + r.width() / 2, r.y() + 2 );
    QPoint l2( r.x() + r.width() / 2, r.y() + 8 );

    QPoint h1( r.x() + r.width() / 2, r.y() + r.height() / 4 );
    QPoint h2( r.x() + r.width() / 2, r.y() + r.height() / 2 );

    QPoint m1( r.x() + r.width() / 2, r.y() + r.height() / 8 );
    QPoint m2( r.x() + r.width() / 2, r.y() + r.height() / 2 );

    QPoint s1( r.x() + r.width() / 2, r.y() + 8 );
    QPoint s2( r.x() + r.width() / 2, r.y() + r.height() / 2 );

    QColor color( clear ? backgroundColor() : black );
    QTime time = clear ? prevTime : currTime;

    if ( clear && prevTime.secsTo(currTime) > 1 ) {
  p->eraseRect( rect() );
  return;
    }

    if ( !clear ) {
  // draw ticks
  p->setPen( QPen( color, 1 ) );
  for ( int i = 0; i < 12; i++ )
      p->drawLine( rotate( center, l1, i * 30 ), rotate( center, l2, i * 30 ) );
    }

    if ( !clear || prevTime.minute() != currTime.minute() ||
      prevTime.hour() != currTime.hour() ) {
  // draw hour pointer
  h1 = rotate( center, h1, 30 * ( time.hour() % 12 ) + time.minute() / 2 );
  h2 = rotate( center, h2, 30 * ( time.hour() % 12 ) + time.minute() / 2 );
  p->setPen( QPen( color, 3 ) );
  p->drawLine( h1, h2 );
    }

    if ( !clear || prevTime.minute() != currTime.minute() ) {
  // draw minute pointer
  m1 = rotate( center, m1, time.minute() * 6 );
  m2 = rotate( center, m2, time.minute() * 6 );
  p->setPen( QPen( color, 2 ) );
  p->drawLine( m1, m2 );
    }

    // draw second pointer
    s1 = rotate( center, s1, time.second() * 6 );
    s2 = rotate( center, s2, time.second() * 6 );
    p->setPen( QPen( color, 1 ) );
    p->drawLine( s1, s2 );

    if ( !clear )
  prevTime = currTime;
}

void AnalogClock::display( const QTime& t )
{
    currTime = t;
    clear = true;
    repaint( false );
    clear = false;
    repaint( false );
}

QPoint AnalogClock::rotate( QPoint c, QPoint p, int a )
{
    double angle = deg2rad * ( - a + 180 );
    double nx = c.x() - ( p.x() - c.x() ) * cos( angle ) -
    ( p.y() - c.y() ) * sin( angle );
    double ny = c.y() - ( p.y() - c.y() ) * cos( angle ) +
    ( p.x() - c.x() ) * sin( angle );
    return QPoint( nx, ny );
}
