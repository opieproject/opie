/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <Eilers.Stefan@epost.de>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
// CONTAINS GPLed code of TT

#ifndef OEVENT_H
#define OEVENT_H

/* OPIE */
#include <opie2/otimezone.h>
#include <opie2/opimrecord.h>
#include <qpe/recordfields.h>
#include <qpe/palmtopuidgen.h>

/* QT */
#include <qstring.h>
#include <qdatetime.h>
#include <qvaluelist.h>

namespace Opie
{
struct OCalendarHelper
{
    /** calculate the week number of the date */
    static int week( const QDate& );
    /** calculate the occurence of week days since the start of the month */
    static int ocurrence( const QDate& );

    // returns the dayOfWeek for the *first* day it finds (ignores
    // any further days!). Returns 1 (Monday) if there isn't any day found
    static int dayOfWeek( char day );

    /** returns the diff of month */
    static int monthDiff( const QDate& first, const QDate& second );

};

class OPimNotifyManager;
class ORecur;

/**
 * This is the container for all Events. It encapsules all
 * available information for a single Event
 * @short container for events.
 */
class OEvent : public OPimRecord
{
  public:
    typedef QValueList<OEvent> ValueList;
    /**
     * RecordFields contain possible attributes
     * used in the Results of toMap()..
     */
    enum RecordFields {
        FUid = Qtopia::UID_ID,
        FCategories = Qtopia::CATEGORY_ID,
        FDescription = 0,
        FLocation,
        FType,
        FAlarm,
        FSound,
        FRType,
        FRWeekdays,
        FRPosition,
        FRFreq,
        FRHasEndDate,
        FREndDate,
        FRCreated,
        FRExceptions,
        FStart,
        FEnd,
        FNote,
        FTimeZone,
        FRecParent,
        FRecChildren,
    };

    /**
     * Start with an Empty OEvent. UID == 0 means that it is empty
     */
    OEvent( int uid = 0 );

    /**
     * copy c'tor
     */
    OEvent( const OEvent& );

    /**
     * Create OEvent, initialized by map
     * @see enum RecordFields
     */
    OEvent( const QMap<int, QString> map );
    ~OEvent();
    OEvent &operator=( const OEvent& );

    QString description() const;
    void setDescription( const QString& description );

    QString location() const;
    void setLocation( const QString& loc );

    bool hasNotifiers() const;
    OPimNotifyManager &notifiers() const;

    ORecur recurrence() const;
    void setRecurrence( const ORecur& );
    bool hasRecurrence() const;

    QString note() const;
    void setNote( const QString& note );


    QDateTime createdDateTime() const;
    void setCreatedDateTime( const QDateTime& dt );

    /** set the date to dt. dt is the QDateTime in localtime */
    void setStartDateTime( const QDateTime& );
    /** returns the datetime in the local timeZone */
    QDateTime startDateTime() const;

    /** returns the start datetime in the current zone */
    QDateTime startDateTimeInZone() const;

    /** in current timezone */
    void setEndDateTime( const QDateTime& );
    /** in current timezone */
    QDateTime endDateTime() const;
    QDateTime endDateTimeInZone() const;

    bool isMultipleDay() const;
    bool isAllDay() const;
    void setAllDay( bool isAllDay );

    /* pin this event to a timezone! FIXME */
    void setTimeZone( const QString& timeZone );
    QString timeZone() const;


    virtual bool match( const QRegExp& ) const;

    /** For exception to recurrence here is a list of children...  */
    QArray<int> children() const;
    void setChildren( const QArray<int>& );
    void addChild( int uid );
    void removeChild( int uid );

    /** return the parent OEvent */
    int parent() const;
    void setParent( int uid );


    /* needed reimp */
    QString toRichText() const;
    QString toShortText() const;
    QString type() const;

    QMap<int, QString> toMap() const;
    void fromMap( const QMap<int, QString>& map );
    QString recordField( int ) const;

    static int rtti();

    bool loadFromStream( QDataStream& );
    bool saveToStream( QDataStream& ) const;

    /*    bool operator==( const OEvent& );
        bool operator!=( const OEvent& );
        bool operator<( const OEvent& );
        bool operator<=( const OEvent& );
        bool operator>( const OEvent& );
        bool operator>=(const OEvent& );
    */

  private:
    inline void changeOrModify();
    void deref();
    struct Data;
    Data* data;
    class Private;
    Private* priv;

};

/**
 * AN Event can span through multiple days. We split up a multiday eve
 */
class OEffectiveEvent
{

  public:
    typedef QValueList<OEffectiveEvent> ValueList;
    enum Position { MidWay, Start, End, StartEnd };
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
    OEffectiveEvent();
    OEffectiveEvent( const OEvent& event, const QDate& startDate, Position pos = StartEnd );
    OEffectiveEvent( const OEffectiveEvent& );
    OEffectiveEvent &operator=( const OEffectiveEvent& );
    ~OEffectiveEvent();

    void setStartTime( const QTime& );
    void setEndTime( const QTime& );
    void setEvent( const OEvent& );
    void setDate( const QDate& );

    void setEffectiveDates( const QDate& from, const QDate& to );

    QString description() const;
    QString location() const;
    QString note() const;
    OEvent event() const;
    QTime startTime() const;
    QTime endTime() const;
    QDate date() const;

    /* return the length in hours */
    int length() const;
    int size() const;

    QDate startDate() const;
    QDate endDate() const;

    bool operator<( const OEffectiveEvent &e ) const;
    bool operator<=( const OEffectiveEvent &e ) const;
    bool operator==( const OEffectiveEvent &e ) const;
    bool operator!=( const OEffectiveEvent &e ) const;
    bool operator>( const OEffectiveEvent &e ) const;
    bool operator>= ( const OEffectiveEvent &e ) const;

  private:
    void deref();
    inline void changeOrModify();
    class Private;
    Private* priv;
    struct Data;
    Data* data;

};

}

#endif
