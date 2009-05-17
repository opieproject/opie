/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
// changes added and Copyright (C) by L. J. Potter <ljp@llornkcor.com> 2002
// changes added and Copyright (C) by Holger Freyther 2004,2005

#include "clock.h"

#include "analogclock.h"

#include <qtabwidget.h>

#include <opie2/ofiledialog.h>
#include <opie2/oresource.h>
#include <opie2/odebug.h>

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>
#include <qpe/timestring.h>
#include <qpe/alarmserver.h>
#include <qpe/sound.h>
#include <qsound.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qfile.h>


#include <qlcdnumber.h>
#include <qslider.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlineedit.h>

static const int sw_prec = 2;
static const int magic_daily = 2292922;
static const int magic_countdown = 2292923;
static const int magic_snooze = 2292924;
static const int ALARMTYPE_DEFALARM = 0;
static const int ALARMTYPE_EXTERNAL = 1;
static const int ALARMTYPE_OPIEPLAYER = 2;
static const int ALARMTYPE_OPIEPLAYER2 = 3;
static const int DEFAULT_ALARMTYPE = ALARMTYPE_DEFALARM;
static const int DEFAULT_SNOOZE_TIME = 5; // minutes
static const char ALARM_CLOCK_CHANNEL [] = "QPE/Application/clock";
static const char ALARM_CLOCK_MESSAGE [] = "alarm(QDateTime,int)";

#include <math.h>
#include <unistd.h>
#include <sys/types.h>

#include <pthread.h>


static void toggleScreenSaver( bool on )
{
    QCopEnvelope e( "QPE/System", "setScreenSaverMode(int)" );
    e << ( on ? QPEApplication::Enable : QPEApplication::DisableSuspend );
}

static void playFile( QString file, bool opiePlayer2 )
{
    if(opiePlayer2) {
        QCopEnvelope e( "QPE/Application/opieplayer2", "play(QString)" );
        e << file;
    }
    else {
        QCopEnvelope e( "QPE/Application/opieplayer", "play(QString)" );
        e << file;
    }
}

static void startPlayer()
{
    Config config( "qpe" );
    config.setGroup( "Time" );
    sleep(3);
    QString file = config.readEntry( "mp3File", "" );
    if(file != "" && QFile::exists(file)) {
        if( config.readNumEntry( "mp3Player", DEFAULT_ALARMTYPE ) == ALARMTYPE_OPIEPLAYER )
            playFile( file, false );
        else
            playFile( file, true );
    }
}

class MySpinBox : public QSpinBox
{
public:
    QLineEdit *lineEdit() const {
        return editor();
    }
};


AlarmDlg::AlarmDlg(QWidget *parent, const char *name, bool modal,
    const QString &txt) :
    AlarmDlgBase(parent, name, modal)
{
    // Increase font size to make it a little more readable
    QFont f(font());
    f.setPointSize((int)(f.pointSize() * 1.7));
    setFont(f);

    setCaption( tr("Clock") );
    pixmap->setPixmap( Opie::Core::OResource::loadPixmap("clock/alarmbell") );
    alarmDlgLabel->setText(txt);

    connect(cmdSnooze, SIGNAL(clicked()), this, SLOT(checkSnooze()));
    connect(cmdOk, SIGNAL(clicked()), this, SLOT(accept()));
}

void AlarmDlg::setText(const QString &txt)
{
    alarmDlgLabel->setText(txt);
}

void AlarmDlg::setSnoozeTime( int mins )
{
    snoozeTime->setValue( mins );
}

void AlarmDlg::checkSnooze(void)
{
    // Ensure we have only one snooze alarm.
    AlarmServer::deleteAlarm(QDateTime(), ALARM_CLOCK_CHANNEL,
        ALARM_CLOCK_MESSAGE, magic_snooze);

    if (snoozeTime->value() > 0) {
        QDateTime wake = QDateTime::currentDateTime();
        wake = wake.addSecs(snoozeTime->value() * 60); // snoozeTime in minutes

        AlarmServer::addAlarm(wake, ALARM_CLOCK_CHANNEL,
            ALARM_CLOCK_MESSAGE, magic_snooze);
    }
    accept();
}


Clock::Clock( QWidget * parent, const char *, WFlags f )
    : ClockBase( parent, "clock", f ), swatch_splitms(99), init(FALSE) // No tr
{
    alarmDlg = 0;
    swLayout = 0;
    alarmProc = 0;
    alarmCmd = 0;
    dayBtn = new QToolButton * [7];

    Config config( "qpe" );
    config.setGroup("Time");
    ampm = config.readBoolEntry( "AMPM", TRUE );
    onMonday = config.readBoolEntry( "MONDAY" );

    connect( tabs, SIGNAL(currentChanged(QWidget*)),
             this, SLOT(tabChanged(QWidget*)) );

    analogStopwatch = new AnalogClock( swFrame );
    stopwatchLcd = new QLCDNumber( swFrame );
    stopwatchLcd->setFrameStyle( QFrame::NoFrame );
    stopwatchLcd->setSegmentStyle( QLCDNumber::Flat );
    stopwatchLcd->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred) );

    analogClock->display( QTime::currentTime() );
    clockLcd->setNumDigits( 5 );
    clockLcd->setFixedWidth( clockLcd->sizeHint().width() );
    date->setText( TimeString::dateString( QDate::currentDate(), TimeString::currentDateFormat() ) );
    if ( qApp->desktop()->width() < 200 )
        date->setFont( QFont(date->font().family(), 14, QFont::Bold) );
    if ( qApp->desktop()->height() > 240 ) {
        clockLcd->setFixedHeight( 30 );
        stopwatchLcd->setFixedHeight( 30 );
    }

    connect( stopStart, SIGNAL(pressed()), SLOT(stopStartStopWatch()) );
    connect( reset, SIGNAL(pressed()), SLOT(resetStopWatch()) );

    t = new QTimer( this );
    connect( t, SIGNAL(timeout()), SLOT(updateClock()) );
    t->start( 1000 );

    applyAlarmTimer = new QTimer( this );
    connect( applyAlarmTimer, SIGNAL(timeout()),
        this, SLOT(applyDailyAlarm()) );

    alarmt = new QTimer( this );
    connect( alarmt, SIGNAL(timeout()), SLOT(alarmTimeout()) );

    connect( qApp, SIGNAL(timeChanged()), SLOT(updateClock()) );
    connect( qApp, SIGNAL(timeChanged()), SLOT(applyDailyAlarm()) );

    swatch_running = FALSE;
    swatch_totalms = 0;
    swatch_currLap = 0;
    swatch_dispLap = 0;
    stopwatchLcd->setNumDigits( 8+1+sw_prec );
    stopwatchLcd->display( "00:00:00.00" );

    QVBoxLayout *lvb = new QVBoxLayout( lapFrame );
    nextLapBtn = new QToolButton( UpArrow, lapFrame );
    connect( nextLapBtn, SIGNAL(clicked()), this, SLOT(nextLap()) );
    nextLapBtn->setAccel( Key_Up );
    lvb->addWidget( nextLapBtn );
    prevLapBtn = new QToolButton( DownArrow, lapFrame );
    connect( prevLapBtn, SIGNAL(clicked()), this, SLOT(prevLap()) );
    prevLapBtn->setAccel( Key_Down );
    prevLapBtn->setMinimumWidth( 15 );
    lvb->addWidget( prevLapBtn );
    prevLapBtn->setEnabled( FALSE );
    nextLapBtn->setEnabled( FALSE );

    reset->setEnabled( FALSE );

    lapLcd->setNumDigits( 8+1+sw_prec );
    lapLcd->display( "00:00:00.00" );

    splitLcd->setNumDigits( 8+1+sw_prec );
    splitLcd->display( "00:00:00.00" );

    lapNumLcd->display( 1 );

    lapTimer = new QTimer( this );
    connect( lapTimer, SIGNAL(timeout()), this, SLOT(lapTimeout()) );

    for (uint s = 0; s < swatch_splitms.count(); s++ )
        swatch_splitms[(int)s] = 0;

    connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(changeClock(bool)) );

    cdGroup->hide(); // XXX implement countdown timer.

    connect( dailyHour, SIGNAL(valueChanged(int)), this, SLOT(scheduleApplyDailyAlarm()) );
    connect( dailyMinute, SIGNAL(valueChanged(int)), this, SLOT(setDailyMinute(int)) );
    connect( dailyAmPm, SIGNAL(activated(int)), this, SLOT(setDailyAmPm(int)) );
    connect( dailyEnabled, SIGNAL(toggled(bool)), this, SLOT(enableDaily(bool)) );
    cdLcd->display( "00:00" );

    dailyMinute->setValidator(0);

    Config cConfig( "Clock" ); // No tr
    cConfig.setGroup( "Daily Alarm" );

    QStringList days;
    days.append( tr("Mon", "Monday") );
    days.append( tr("Tue", "Tuesday") );
    days.append( tr("Wed", "Wednesday") );
    days.append( tr("Thu", "Thursday") );
    days.append( tr("Fri", "Friday") );
    days.append( tr("Sat", "Saturday") );
    days.append( tr("Sun", "Sunday") );

    int i;
    QHBoxLayout *hb = new QHBoxLayout( daysFrame );
    for ( i = 0; i < 7; i++ ) {
        dayBtn[i] = new QToolButton( daysFrame );
        hb->addWidget( dayBtn[i] );
        dayBtn[i]->setToggleButton( TRUE );
        dayBtn[i]->setOn( TRUE );
        dayBtn[i]->setFocusPolicy( StrongFocus );
        connect( dayBtn[i], SIGNAL(toggled(bool)), this, SLOT(scheduleApplyDailyAlarm()) );
    }

    for ( i = 0; i < 7; i++ )
        dayBtn[dayBtnIdx(i+1)]->setText( days[i] );

    QStringList exclDays = cConfig.readListEntry( "ExcludeDays", ',' );
    QStringList::Iterator it;
    for ( it = exclDays.begin(); it != exclDays.end(); ++it ) {
        int d = (*it).toInt();
        if ( d >= 1 && d <= 7 )
            dayBtn[dayBtnIdx(d)]->setOn( FALSE );
    }

    sbSnoozeTime->setValue( cConfig.readNumEntry( "SnoozeTime", DEFAULT_SNOOZE_TIME ) );

    connect( sndOption, SIGNAL( activated(int) ), SLOT( slotSoundChange(int) ) );
    sndOption->insertItem(tr("Default alarm"));
    sndOption->insertItem(tr("Play file in background"));
    sndOption->insertItem(tr("Play file with OpiePlayer"));
    sndOption->insertItem(tr("Play file with OpiePlayer 2"));

    bool alarm = cConfig.readBoolEntry("Enabled", FALSE);
    dailyEnabled->setChecked( alarm );
    sndGroup->setEnabled( alarm );

    // FIXME ODP migrate to own config class.. merge config options
    Config cfg_qpe( "qpe" );
    cfg_qpe.setGroup( "Time" );
    sndFileName->setText( cfg_qpe.readEntry( "mp3File" ) );
    sndOption->setCurrentItem( cfg_qpe.readNumEntry( "mp3Player", DEFAULT_ALARMTYPE ) );
    slotSoundChange(sndOption->currentItem());

    int m = cConfig.readNumEntry( "Minute", 0 );
    dailyMinute->setValue( m );
//    dailyMinute->setPrefix( m <= 9 ? "0" : "" );
    int h = cConfig.readNumEntry( "Hour", 7 );
    if ( ampm ) {
        if (h > 12) {
            h -= 12;
            dailyAmPm->setCurrentItem( 1 );
        }
        if (h == 0) h = 12;
        dailyHour->setMinValue( 1 );
        dailyHour->setMaxValue( 12 );
    } else {
        dailyAmPm->hide();
    }
    dailyHour->setValue( h );

    connect( ((MySpinBox*)dailyHour)->lineEdit(), SIGNAL(textChanged(const QString&)),
            this, SLOT(dailyEdited()) );
    connect( ((MySpinBox*)dailyMinute)->lineEdit(), SIGNAL(textChanged(const QString&)),
            this, SLOT(dailyEdited()) );

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    connect( qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
            this, SLOT(appMessage(const QCString&,const QByteArray&)) );
#endif

    QTimer::singleShot( 0, this, SLOT(updateClock()) );
    swFrame->installEventFilter( this );

    init = TRUE;
}

Clock::~Clock()
{
    toggleScreenSaver( true );
    if(alarmProc)
        delete alarmProc;
    delete [] dayBtn;
}

void Clock::updateClock()
{
    if ( tabs->currentPageIndex() == 0 ) {
        QTime tm = QDateTime::currentDateTime().time();
        QString s;
        if ( ampm ) {
            int hour = tm.hour();
            if (hour == 0)
                hour = 12;
            if (hour > 12)
                hour -= 12;
            s.sprintf( "%2d%c%02d", hour, ':', tm.minute() );
            clockAmPm->setText( (tm.hour() >= 12) ? "PM" : "AM" );
            clockAmPm->show();
        } else {
            s.sprintf( "%2d%c%02d", tm.hour(), ':', tm.minute() );
            clockAmPm->hide();
        }
        clockLcd->display( s );
        clockLcd->repaint( FALSE );
        analogClock->display( QTime::currentTime() );
        date->setText( TimeString::dateString( QDate::currentDate(), TimeString::currentDateFormat() ) );
    }
    else if ( tabs->currentPageIndex() == 1 ) {
        int totalms = swatch_totalms;
        if ( swatch_running )
            totalms += swatch_start.elapsed();
        setSwatchLcd( stopwatchLcd, totalms, !swatch_running );
        QTime swatch_time = QTime(0,0,0).addMSecs(totalms);
        analogStopwatch->display( swatch_time );
        if ( swatch_dispLap == swatch_currLap ) {
            swatch_splitms[swatch_currLap] = swatch_totalms;
            if ( swatch_running )
                swatch_splitms[swatch_currLap] += swatch_start.elapsed();
            updateLap();
        }
    }
    else if ( tabs->currentPageIndex() == 2 ) {
        // nothing.
    }
}

void Clock::changeClock( bool a )
{
    if ( ampm != a ) {
        int minute = dailyMinute->value();
        int hour = dailyHour->value();
        if ( ampm ) {
            if (hour == 12)
                hour = 0;
            if (dailyAmPm->currentItem() == 1 )
                hour += 12;
            dailyHour->setMinValue( 0 );
            dailyHour->setMaxValue( 23 );
            dailyAmPm->hide();
        } else {
            if (hour > 12) {
                hour -= 12;
                dailyAmPm->setCurrentItem( 1 );
            }
            if (hour == 0) hour = 12;
            dailyHour->setMinValue( 1 );
            dailyHour->setMaxValue( 12 );
            dailyAmPm->show();
        }
        dailyMinute->setValue( minute );
        dailyHour->setValue( hour );
    }
    ampm = a;
    updateClock();
}

void Clock::stopStartStopWatch()
{
    if ( swatch_running ) {
        swatch_totalms += swatch_start.elapsed();
        swatch_splitms[swatch_currLap] = swatch_totalms;
        stopStart->setText( tr("Start") );
        reset->setText( tr("Reset") );
        reset->setEnabled( TRUE );
        t->stop();
        swatch_running = FALSE;
        toggleScreenSaver( TRUE );
        updateClock();
    }
    else {
        swatch_start.start();
        stopStart->setText( tr("Stop") );
        reset->setText( tr("Lap/Split") );
        reset->setEnabled( swatch_currLap < 98 );
        t->start( 1000 );
        swatch_running = TRUE;
        // disable screensaver while stop watch is running
        toggleScreenSaver( FALSE );
    }
    swatch_dispLap = swatch_currLap;
    updateLap();
    prevLapBtn->setEnabled( swatch_dispLap );
    nextLapBtn->setEnabled( swatch_dispLap < swatch_currLap );
    stopStart->setAccel( Key_Return );
}

void Clock::resetStopWatch()
{
    if ( swatch_running ) {
        swatch_splitms[swatch_currLap] = swatch_totalms+swatch_start.elapsed();
        swatch_dispLap = swatch_currLap;
        if ( swatch_currLap < 98 )  // allow up to 99 laps
            swatch_currLap++;
        reset->setEnabled( swatch_currLap < 98 );
        updateLap();
        lapTimer->start( 2000, TRUE );
    }
    else {
        swatch_start.start();
        swatch_totalms = 0;
        swatch_currLap = 0;
        swatch_dispLap = 0;
        for ( uint i = 0; i < swatch_splitms.count(); i++ )
            swatch_splitms[(int)i] = 0;
        updateLap();
        updateClock();
        reset->setText( tr("Lap/Split") );
        reset->setEnabled( FALSE );
    }
    prevLapBtn->setEnabled( swatch_dispLap );
    nextLapBtn->setEnabled( swatch_dispLap < swatch_currLap );
}

void Clock::prevLap()
{
    if ( swatch_dispLap > 0 ) {
        swatch_dispLap--;
        updateLap();
        prevLapBtn->setEnabled( swatch_dispLap );
        nextLapBtn->setEnabled( swatch_dispLap < swatch_currLap );
    }
}

void Clock::nextLap()
{
    if ( swatch_dispLap < swatch_currLap ) {
        swatch_dispLap++;
        updateLap();
        prevLapBtn->setEnabled( swatch_dispLap );
        nextLapBtn->setEnabled( swatch_dispLap < swatch_currLap );
    }
}

void Clock::lapTimeout()
{
    swatch_dispLap = swatch_currLap;
    updateLap();
    prevLapBtn->setEnabled( swatch_dispLap );
    nextLapBtn->setEnabled( swatch_dispLap < swatch_currLap );
}

void Clock::updateLap()
{
    if ( swatch_running && swatch_currLap == swatch_dispLap ) {
        swatch_splitms[swatch_currLap] = swatch_totalms;
        swatch_splitms[swatch_currLap] += swatch_start.elapsed();
    }
    int split = swatch_splitms[swatch_dispLap];
    int lap;
    if ( swatch_dispLap > 0 )
        lap = swatch_splitms[swatch_dispLap] - swatch_splitms[swatch_dispLap-1];
    else
        lap = swatch_splitms[swatch_dispLap];

    lapNumLcd->display( swatch_dispLap+1 );
    bool showMs = !swatch_running || swatch_dispLap!=swatch_currLap;
    setSwatchLcd( lapLcd, lap, showMs );
    setSwatchLcd( splitLcd, split, showMs );
}

void Clock::setSwatchLcd( QLCDNumber *lcd, int ms, bool showMs )
{
    QTime swatch_time = QTime(0,0,0).addMSecs(ms);
    QString d = showMs ? QString::number(ms%1000+1000) : QString("    ");
    QString lcdtext = swatch_time.toString() + "." + d.right(3).left(sw_prec);
    lcd->display( lcdtext );
    lcd->repaint( FALSE );
}

bool Clock::eventFilter( QObject *o, QEvent *e )
{
    if ( o == swFrame && e->type() == QEvent::Resize ) {
        QResizeEvent *re = (QResizeEvent *)e;
        delete swLayout;
        if ( re->size().height() < 80 || re->size().height()*3 < re->size().width() )
            swLayout = new QHBoxLayout( swFrame );
        else
            swLayout = new QVBoxLayout( swFrame );
        swLayout->addWidget( analogStopwatch );
        swLayout->addWidget( stopwatchLcd );
        swLayout->activate();
    }

    return FALSE;
}

void Clock::tabChanged( QWidget * )
{
    if ( tabs->currentPageIndex() == 0 ) {
        t->start(1000);
    }
    else if ( tabs->currentPageIndex() == 1 ) {
        if ( !swatch_running )
            t->stop();
        stopStart->setAccel( Key_Return );
    }
    else if ( tabs->currentPageIndex() == 2 ) {
        t->start(1000);
    }
    updateClock();
}

void Clock::setDailyAmPm(int)
{
    scheduleApplyDailyAlarm();
}

void Clock::setDailyMinute( int m )
{
    dailyMinute->setPrefix( m <= 9 ? "0" : "" );
}

void Clock::dailyEdited()
{
    if ( spinBoxValid(dailyMinute) && spinBoxValid(dailyHour) )
        scheduleApplyDailyAlarm();
    else
        applyAlarmTimer->stop();
}

void Clock::enableDaily( bool )
{
    scheduleApplyDailyAlarm();
}

void Clock::appMessage( const QCString &msg, const QByteArray &data )
{
    if ( msg == ALARM_CLOCK_MESSAGE ) {
        QDataStream ds(data,IO_ReadOnly);
        QDateTime when;
        int t;
        ds >> when >> t;
        QTime theTime( when.time() );
        if ( t == magic_daily || t == magic_snooze ) {
            QString msg = tr("<b>Daily Alarm:</b><p>");
            QString ts;
            if ( ampm ) {
            bool pm = FALSE;
                int h = theTime.hour();
                if (h > 12) {
                    h -= 12;
                    pm = TRUE;
                }
                if (h == 0) h = 12;
                ts.sprintf( "%02d:%02d %s", h, theTime.minute(), pm?"PM":"AM" );
            }
            else {
                ts.sprintf( "%02d:%02d", theTime.hour(), theTime.minute() );
            }
            msg += ts;

            Config config( "qpe" );
            config.setGroup("Time");
            int player = config.readNumEntry("mp3Player", DEFAULT_ALARMTYPE);
            if(player == ALARMTYPE_DEFALARM) {
                Sound::soundAlarm();
                alarmCount = 0;
                alarmt->start( 5000 );
            }
            else if(player == ALARMTYPE_OPIEPLAYER || player == ALARMTYPE_OPIEPLAYER2 ) {
                pthread_t thread;
                pthread_create(&thread,NULL, (void * (*) (void *))startPlayer, NULL/* &*/ );
            }
            else
                startAlarmProcess();

            if ( !alarmDlg ) {
                alarmDlg = new AlarmDlg(this);
            }
            alarmDlg->setText(msg);

            Config clockConfig( "Clock" );
            clockConfig.setGroup( "Daily Alarm" );
            alarmDlg->setSnoozeTime( clockConfig.readNumEntry( "SnoozeTime", DEFAULT_SNOOZE_TIME ) );

            // Set for tomorrow, so user wakes up every day, even if they
            // don't confirm the dialog.  Don't set it again when snoozing.
            if (t != magic_snooze) {
                applyDailyAlarm();
            }

            if ( !alarmDlg->isVisible() ) {
                QPEApplication::execDialog(alarmDlg);
                alarmt->stop();
                if(alarmProc)
                    alarmProc->kill();
            }
        }
        else if ( t == magic_countdown ) {
            // countdown
            Sound::soundAlarm();
        }
    }
    else if ( msg == "setDailyEnabled(int)" ) {
        QDataStream ds(data,IO_ReadOnly);
        int enableDaily;
        ds >> enableDaily;
        dailyEnabled->setChecked( enableDaily );
        applyDailyAlarm();
    }
    else if ( msg == "editDailyAlarm()" ) {
        tabs->setCurrentPage(2);
        QPEApplication::setKeepRunning();
    }
    else if (msg == "showClock()") {
        tabs->setCurrentPage(0);
        QPEApplication::setKeepRunning();
    }
    else if (msg == "timerStart()" ) {
        if ( !swatch_running )
            stopStartStopWatch();
        tabs->setCurrentPage(1);
        QPEApplication::setKeepRunning();
    }
    else if (msg == "timerStop()" ) {
        if ( swatch_running )
            stopStartStopWatch();
        tabs->setCurrentPage(1);
        QPEApplication::setKeepRunning();
    }
    else if (msg == "timerReset()" ) {
        resetStopWatch();
        tabs->setCurrentPage(1);
        QPEApplication::setKeepRunning();
    }
}

void Clock::alarmTimeout()
{
    if ( alarmCount < 10 ) {
        Sound::soundAlarm();
        alarmCount++;
    }
    else {
        alarmCount = 0;
        alarmt->stop();
    }
}

QDateTime Clock::nextAlarm( int h, int m )
{
    QDateTime now = QDateTime::currentDateTime();
    QTime at( h, m );
    QDateTime when( now.date(), at );
    int count = 0;
    int dow = when.date().dayOfWeek();
    while ( when < now || !dayBtn[dayBtnIdx(dow)]->isOn() ) {
        when = when.addDays( 1 );
        dow = when.date().dayOfWeek();
        if ( ++count > 7 )
            return QDateTime();
    }

    return when;
}

int Clock::dayBtnIdx( int d ) const
{
    if ( onMonday )
        return d-1;
    else if ( d == 7 )
        return 0;
    else
        return d;
}

void Clock::scheduleApplyDailyAlarm()
{
    applyAlarmTimer->start( 5000, TRUE );
}

void Clock::applyDailyAlarm()
{
    if ( !init )
        return;

    applyAlarmTimer->stop();
    int minute = dailyMinute->value();
    int hour = dailyHour->value();
    if ( ampm ) {
        if (hour == 12)
            hour = 0;
        if (dailyAmPm->currentItem() == 1 )
            hour += 12;
    }

    Config config( "Clock" );
    config.setGroup( "Daily Alarm" );
    config.writeEntry( "Hour", hour );
    config.writeEntry( "Minute", minute );

    bool enableDaily = dailyEnabled->isChecked();
    config.writeEntry( "Enabled", enableDaily );

    QString exclDays;
    int exclCount = 0;
    for ( int i = 1; i <= 7; i++ ) {
        if ( !dayBtn[dayBtnIdx(i)]->isOn() ) {
            if ( !exclDays.isEmpty() )
                exclDays += ",";
            exclDays += QString::number( i );
            exclCount++;
        }
    }
    config.writeEntry( "ExcludeDays", exclDays );

    /* try to delete all  */
    AlarmServer::deleteAlarm(QDateTime(), ALARM_CLOCK_CHANNEL,
                             ALARM_CLOCK_MESSAGE, magic_daily);
    AlarmServer::deleteAlarm(QDateTime(), ALARM_CLOCK_CHANNEL,
                             ALARM_CLOCK_MESSAGE, magic_snooze);

    if ( enableDaily && exclCount < 7 ) {
        QDateTime when = nextAlarm( hour, minute );
        AlarmServer::addAlarm(when, ALARM_CLOCK_CHANNEL,
                            ALARM_CLOCK_MESSAGE, magic_daily);
    }

    config.writeEntry( "SnoozeTime", sbSnoozeTime->value() );
}

bool Clock::validDaysSelected(void)
{
    for ( int i = 1; i <= 7; i++ ) {
        if ( dayBtn[dayBtnIdx(i)]->isOn() ) {
            return TRUE;
        }
    }
    return FALSE;
}

void Clock::closeEvent( QCloseEvent *e )
{
    if (dailyEnabled->isChecked()) {
        if (!validDaysSelected()) {
            QMessageBox::warning(this, tr("Select Day"),
                tr("Daily alarm requires at least\none day to be selected."));
            return;
        }
    }

    applyDailyAlarm();
    ClockBase::closeEvent(e);
}

bool Clock::spinBoxValid( QSpinBox *sb )
{
    bool valid = TRUE;
    QString tv = sb->text();
    for ( uint i = 0; i < tv.length(); i++ ) {
        if ( !tv[0].isDigit() )
            valid = FALSE;
    }
    bool ok = FALSE;
    int v = tv.toInt( &ok );
    if ( !ok )
        valid = FALSE;
    if ( v < sb->minValue() || v > sb->maxValue() )
        valid = FALSE;

    return valid;
}

void Clock::slotBrowseMp3File()
{
    Config config( "qpe" );
    config.setGroup("Time");

    QMap<QString, QStringList> map;
    map.insert(tr("All"), QStringList() );
    QStringList text;
    text << "audio/*";
    map.insert(tr("Audio"), text );
    QString path;
    if(sndFileName->text().isEmpty())
        path = QPEApplication::qpeDir() + "sounds";
    else {
        QFileInfo fi(sndFileName->text());
        path = fi.dirPath(true);
    }
    QString str = Opie::Ui::OFileDialog::getOpenFileName( 2, path, QString::null, map);
    if(!str.isEmpty() ) {
        config.writeEntry("mp3Alarm",1);
        config.writeEntry("mp3File",str);
        sndFileName->setText( str );
        scheduleApplyDailyAlarm();
    }
}

void Clock::slotTestAlarmSound()
{
    int player = sndOption->currentItem();
    if( player == ALARMTYPE_DEFALARM ) 
        Sound::soundAlarm();
    else if( player == ALARMTYPE_OPIEPLAYER || player == ALARMTYPE_OPIEPLAYER2 )
        playFile( sndFileName->text(), player == ALARMTYPE_OPIEPLAYER2 );
    else
        alarmPlayTest( sndFileName->text() );
}

void Clock::slotSoundChange(int idx)
{
    bool playFile = (idx != 0);
    sndChoose->setEnabled(playFile);
    sndFileName->setEnabled(playFile);

    Config config( "qpe" );
    config.setGroup("Time");
    config.writeEntry("mp3Player",idx);
}

void Clock::startAlarmProcess()
{
    Config config( "qpe" );
    config.setGroup( "Time" );
    QString file = config.readEntry( "mp3File", "" );
    sleep(3);

    if(!file.isEmpty() && QFile::exists(file)) {
        alarmPlay(file);
    }
}

void Clock::alarmPlayTest( const QString file ) {
    alarmPlay(file);
    if(alarmProc && alarmProc->isRunning()) {
        QMessageBox::information(this, tr("Test Alarm"),
            tr("Click OK to stop player"));
        if(alarmProc->isRunning())
            alarmProc->kill();
    }
}

#define NUMCMDS 4
const char* playerCmds[NUMCMDS] = { "mpg321", "madplay", "mpg123", "play" };

void Clock::alarmPlay( const QString file, const QString cmd ) {
    QStringList command;
    if(alarmProc)
        delete alarmProc;

    alarmProc = new Opie::Core::OProcess();

    if(cmd.isEmpty()) {
        // Try to find a player that is installed out of the ones we know
        if(alarmCmd < NUMCMDS) {
            command << "which";
            command << playerCmds[alarmCmd];
            alarmFile = file;
        }
        else
            owarn << "Alarm command number out of range!" << oendl;
    }
    else {
        command << cmd;
        command << file;
    }

    *alarmProc << command;
    QApplication::connect(alarmProc, SIGNAL(processExited(Opie::Core::OProcess *)),
                        this, SLOT(slotAlarmProcessExit(Opie::Core::OProcess *)));
    if(!alarmProc->start(Opie::Core::OProcess::NotifyOnExit, Opie::Core::OProcess::All) )
        owarn << "could not start process" << oendl;
}

void Clock::slotAlarmProcessExit(Opie::Core::OProcess*)
{
    odebug << "process exited" << oendl;
    if(alarmProc->args()[0] == "which") {
        if(alarmProc->normalExit()) {
            if(alarmProc->exitStatus() == 0) {
                // Found an existing command
                odebug << "Playing with " << playerCmds[alarmCmd] << oendl; 
                alarmPlay(alarmFile, playerCmds[alarmCmd]);
                alarmCmd = 0;
            }
            else {
                alarmCmd++;
                if(alarmCmd == NUMCMDS) {
                    alarmCmd = 0;
                    QMessageBox::warning(this, tr("No player found"),
                        tr("<p>No suitable background player was found.<p>Please install mpg321, mpg123, or madplay to enable this functionality."));
                }
                else
                    alarmPlay(alarmFile);
            }
        }
    }
}
