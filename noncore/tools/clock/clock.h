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
#ifndef CLOCK_H
#define CLOCK_H

#include "clockbase.h"
#include "alarmdlgbase.h"
#include <qdatetime.h>

class QTimer;
class QLabel;
class QDialog;
class AnalogClock;
class QBoxLayout;
class QToolButton;

class AlarmDlg: public AlarmDlgBase
{
    Q_OBJECT

public:
    AlarmDlg(QWidget *parent = 0, const char *name = 0, bool modal = TRUE,
	const QString &txt = "Alarm");

    void    setText(const QString &txt);


private slots:
    void    checkSnooze(void);
    void    changePrompt(int minutes);
};

class Clock : public ClockBase
{
    Q_OBJECT

public:
    Clock( QWidget *parent=0, const char *name=0, WFlags fl=0 );
    ~Clock();
    QDateTime when;
    bool bSound;
    int hour, minute, snoozeTime;
    static QString appName() { return QString::fromLatin1("clock"); }

private slots:
    void stopStartStopWatch();
    void resetStopWatch();
    void prevLap();
    void nextLap();
    void lapTimeout();
    void tabChanged(QWidget*);
    void updateClock();
    void changeClock( bool );
    void setDailyAmPm( int );
    void setDailyMinute( int );
    void dailyEdited();
    void enableDaily( bool );
    void appMessage(const QCString& msg, const QByteArray& data);
    void alarmTimeout();
    void applyDailyAlarm();
    void scheduleApplyDailyAlarm();
    void slotBrowseMp3File();

protected:
    QDateTime nextAlarm( int h, int m );
    int dayBtnIdx( int ) const;
    void closeEvent( QCloseEvent *e );
    void updateLap();
    void setSwatchLcd( QLCDNumber *lcd, int ms, bool showMs );
    bool eventFilter( QObject *, QEvent * );
    bool spinBoxValid( QSpinBox *sb );
    bool validDaysSelected(void);

private:
    QTimer *t;
    QTimer *alarmt;
    QTime swatch_start;
    int swatch_totalms;
    QArray<int> swatch_splitms;
    bool swatch_running;
    int swatch_currLap;
    int swatch_dispLap;
    QToolButton *prevLapBtn;
    QToolButton *nextLapBtn;
    QTimer *lapTimer;
    AnalogClock* analogStopwatch;
    QLCDNumber* stopwatchLcd;
    QBoxLayout *swLayout;
    bool ampm;
    bool onMonday;
    int alarmCount;
    AlarmDlg* alarmDlg;
    QToolButton **dayBtn;
    bool init;
    QTimer *applyAlarmTimer;
};

#endif

