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
#ifndef CLOCK_H
#define CLOCK_H

#include <qdatetime.h>
#include <qvbox.h>

class QLCDNumber;
class QLabel;
class QTimer;
class QRadioButton;
class QPushButton;
class QDateTime;

class AnalogClock : public QFrame
{
    Q_OBJECT

public:
    AnalogClock( QWidget * parent = 0, const char * name = 0 )
  : QFrame( parent, name ), clear(false) {}

    QSizePolicy sizePolicy() const;

    void display( const QTime& time );

protected:
    void drawContents( QPainter *p );

private:
    QPoint rotate( QPoint center, QPoint p, int angle );

    QTime currTime;
    QTime prevTime;
    bool clear;
};

class Clock : public QVBox
{
    Q_OBJECT

public:
    Clock( QWidget * parent = 0, const char * name = 0, WFlags f=0 );
    ~Clock();
    QDateTime when;
    bool bSound;
    int hour, minute, snoozeTime;
private slots:
    void slotSet();
    void slotReset();
    void modeSelect(int);
    void updateClock();
    void changeClock( bool );
    void slotSetAlarm();
    void slotSnooze();
    void slotToggleAlarm();
    void alarmOn();
    void alarmOff();
    void appMessage(const QCString& msg, const QByteArray& data);
    void timerEvent( QTimerEvent *e );
    void slotAdjustTime();
private:
    void clearClock();

    bool alarmBool;
    QTimer *t;
    QLCDNumber *lcd;
    QLabel *date;
    QLabel *ampmLabel;
    QPushButton *set, *reset, *alarmBtn, *snoozeBtn, *alarmOffBtn;
    QRadioButton *clockRB, *swatchRB;
    AnalogClock *aclock;
    QTime swatch_start;
    int swatch_totalms;
    bool swatch_running;
    bool ampm;
};

#endif

