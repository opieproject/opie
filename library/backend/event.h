/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
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

#ifndef __EVENT_H__
#define __EVENT_H__

#include <qdatetime.h>
#include <qvaluelist.h>

#ifdef PALMTOPCENTER
#include <qpc/qsorter.h>
#endif
#include <qpe/palmtoprecord.h>

#include <qpe/timeconversion.h>

class EventPrivate;
class QPC_EXPORT Event : public Qtopia::Record
{
public:
    enum RepeatType { NoRepeat = -1, Daily, Weekly, MonthlyDay,
                      MonthlyDate, Yearly };
    enum Days { MON = 0x01, TUE = 0x02, WED = 0x04, THU = 0x08,
                FRI = 0x10, SAT = 0x20, SUN = 0x40 };
    struct QPC_EXPORT RepeatPattern
    {
	RepeatPattern() {
	    type = NoRepeat; frequency = -1; days = 0;  position = 0; createTime = -1;
	    hasEndDate = FALSE; endDateUTC = 0; }
	bool operator ==( const RepeatPattern &right ) const;

	RepeatType type;
	int frequency;
	int position;	// the posistion in the month (e.g. the first sunday, etc) positive, count from the front negative count from the end...
	char days;  // a mask for days OR in your days!
	bool hasEndDate;
	QDate endDate() const { return TimeConversion::fromUTC( endDateUTC ).date(); }
	void setEndDate( const QDate &dt ) { endDateUTC = TimeConversion::toUTC( dt ); }
	time_t endDateUTC;
	time_t createTime;
    };

    Event();
    Event( const QMap<int, QString > & map );
    virtual ~Event();

    QMap<int, QString> toMap() const;

    static void writeVCalendar( const QString &filename, const QValueList<Event> &events);
    static void writeVCalendar( const QString &filename, const Event &event);
    static QValueList<Event> readVCalendar( const QString &filename );

    enum Type { Normal, AllDay };
    enum SoundTypeChoice { Silent, Loud };

    bool operator<( const Event &e1) const { return start() < e1.start(); };
    bool operator<=( const Event &e1 ) const { return start() <= e1.start(); };
    bool operator!=( const Event &e1 ) const { return !( *this == e1 ); };
    bool operator>( const Event &e1 ) const { return start() > e1.start(); };
    bool operator>=(const Event &e1 ) const { return start() >= e1.start(); };
    bool operator==( const Event &e ) const;

    void setDescription( const QString &s );
    const QString &description() const;

    void setLocation( const QString &s );
    const QString &location() const;

    void setType( Type t );
    Type type() const;
    void setStart( const QDateTime &d );
    void setStart( time_t time );
    QDateTime start( bool actual = FALSE ) const;
    time_t startTime() const { return startUTC; }
    void setEnd( const QDateTime &e );
    void setEnd( time_t time );
    QDateTime end( bool actual = FALSE ) const;
    time_t endTime() const { return endUTC; }
    void setTimeZone( const QString & );
    const QString &timeZone() const;
    void setAlarm( bool b, int minutes, SoundTypeChoice );
    bool hasAlarm() const;
    int alarmTime() const;
    SoundTypeChoice alarmSound() const;
    void setRepeat( bool b, const RepeatPattern &p );
    void setRepeat( const RepeatPattern &p );
    bool hasRepeat() const;
    const RepeatPattern &repeatPattern() const;
    RepeatPattern &repeatPattern();
    void setNotes( const QString &n );
    const QString &notes() const;
    bool doRepeat() const { return pattern.type != NoRepeat; }

    void save( QString& buf );
    //void load( Node *n );

    // helper function to calculate the week of the given date
    static int week( const QDate& date );
    // calculates the number of occurrences of the week day of
    // the given date from the start of the month
    static int occurrence( const QDate& date );
    // returns a proper days-char for a given dayOfWeek()
    static char day( int dayOfWeek ) { return 1 << ( dayOfWeek - 1 ); }
    // returns the dayOfWeek for the *first* day it finds (ignores
    // any further days!). Returns 1 (Monday) if there isn't any day found
    static int dayOfWeek( char day );
    // returns the difference of months from first to second.
    static int monthDiff( const QDate& first, const QDate& second );
    bool match( const QRegExp &r ) const;

private:
    Qtopia::UidGen &uidGen() { return sUidGen; }
    static Qtopia::UidGen sUidGen;

    QString descript, locat, categ;
    Type typ : 4;
    bool startTimeDirty : 1;
    bool endTimeDirty : 1;
    time_t startUTC, endUTC;
    QString tz;
    bool hAlarm, hRepeat;
    int aMinutes;
    SoundTypeChoice aSound;
    RepeatPattern pattern;
    QString note;
    EventPrivate *d;
};

// Since an event spans multiple day, it is better to have this
// class to represent a day instead of creating many
// dummy events...

class EffectiveEventPrivate;
class QPC_EXPORT EffectiveEvent
{
public:
    // If we calculate the effective event of a multi-day event
    // we have to figure out whether we are at the first day,
    // at the end, or anywhere else ("middle"). This is important
    // for the start/end times (00:00/23:59)
    // MidWay: 00:00 -> 23:59, as we are "in the middle" of a multi-
    //         day event
    // Start: start time -> 23:59
    // End: 00:00 -> end time
    // Start | End == StartEnd: for single-day events (default)
    //                          here we draw start time -> end time
    enum Position { MidWay = 0, Start = 1, End = 2, StartEnd = 3 };

    EffectiveEvent();
    EffectiveEvent( const Event &event, const QDate &startDate, Position pos = StartEnd );
    EffectiveEvent( const EffectiveEvent & );
    EffectiveEvent& operator=( const EffectiveEvent & );
    ~EffectiveEvent();


    bool operator<( const EffectiveEvent &e ) const;
    bool operator<=( const EffectiveEvent &e ) const;
    bool operator==( const EffectiveEvent &e ) const;
    bool operator!=( const EffectiveEvent &e ) const;
    bool operator>( const EffectiveEvent &e ) const;
    bool operator>= ( const EffectiveEvent &e ) const;

    void setStart( const QTime &start );
    void setEnd( const QTime &end );
    void setEvent( Event e );
    void setDate( const QDate &date );
    void setEffectiveDates( const QDate &from, const QDate &to );

    //    QString category() const;
    const QString &description() const;
    const QString &location() const;
    const QString &notes() const;
    const Event &event() const;
    const QTime &start() const;
    const QTime &end() const;
    const QDate &date() const;
    int length() const;
    int size() const;

    QDate startDate() const;
    QDate endDate() const;

private:
    class EffectiveEventPrivate *d;
    Event mEvent;
    QDate mDate;
    QTime mStart,
	  mEnd;

};

#ifdef PALMTOPCENTER
class QPC_EXPORT EffectiveEventSizeSorter : public QSorter<EffectiveEvent>
{
public:
    int compare( const EffectiveEvent& a, const EffectiveEvent& b ) const
    {
	return a.size() - b.size();
    }
};

class QPC_EXPORT EffectiveEventTimeSorter : public QSorter<EffectiveEvent>
{
public:
    int compare( const EffectiveEvent& a, const EffectiveEvent& b ) const
    {
	return a.start().secsTo( b.start() );
    }
};
#endif

#endif
