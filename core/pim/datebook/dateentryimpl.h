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
#ifndef DATEENTRY_H
#define DATEENTRY_H

#include "dateentry.h"

#include <qpe/event.h>

#include <qdatetime.h>

class DateBookMonth;

class DateEntry : public DateEntryBase
{
    Q_OBJECT

public:
    DateEntry( bool startOnMonday, const QDateTime &start,
	       const QDateTime &end, bool whichClock = FALSE,
	       QWidget* parent = 0, const char* name = 0 );
    DateEntry( bool startOnMonday, const Event &event, bool whichCLock = FALSE,
	       QWidget* parent = 0, const char* name = 0 );
    ~DateEntry();

    Event event();
    void setAlarmEnabled( bool alarmPreset, int presetTime, Event::SoundTypeChoice );

public slots:
    void endDateChanged( int, int, int );
    void endTimeChanged( const QString & );
    void startDateChanged(int, int, int);
    void startTimeChanged( int index );
    void typeChanged( const QString & );
    void changeEndCombo( int change );
    void slotRepeat();
    void slotChangeClock( bool );
    void slotChangeStartOfWeek( bool );

private:
    void init();
    void initCombos();
    void setDates( const QDateTime& s, const QDateTime& e );
    void setRepeatLabel();

    DateBookMonth *startPicker, *endPicker;
    QDate startDate, endDate;
    QTime startTime, endTime;
    Event::RepeatPattern rp;
    bool ampm;
    bool startWeekOnMonday;
};

#endif // DATEENTRY_H
