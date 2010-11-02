/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <Eilers.Stefan@epost.de>
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

#include "opimevent.h"

/* OPIE */
#include <opie2/opimrecurrence.h>
#include <opie2/opimresolver.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/odebug.h>

#include <qpe/categories.h>
#include <qpe/stringutil.h>

/* QT */

namespace Opie
{

int OCalendarHelper::week( const QDate& date )
{
    // Calculates the week this date is in within that
    // month. Equals the "row" is is in in the month view
    int week = 1;
    QDate tmp( date.year(), date.month(), 1 );
    if ( date.dayOfWeek() < tmp.dayOfWeek() )
        ++week;

    week += ( date.day() - 1 ) / 7;

    return week;
}


int OCalendarHelper::ocurrence( const QDate& date )
{
    // calculates the number of occurrances of this day of the
    // week till the given date (e.g 3rd Wednesday of the month)
    return ( date.day() - 1 ) / 7 + 1;
}


int OCalendarHelper::dayOfWeek( char day )
{
    int dayOfWeek = 1;
    char i = OPimRecurrence::MON;
    while ( !( i & day ) && i <= static_cast<char>(OPimRecurrence::SUN) )
    {
        i <<= 1;
        ++dayOfWeek;
    }
    return dayOfWeek;
}


int OCalendarHelper::monthDiff( const QDate& first, const QDate& second )
{
    return ( second.year() - first.year() ) * 12 +
           second.month() - first.month();
}


struct OPimEvent::Data : public QShared
{
    Data() : QShared()
    {
        child = 0;
        recur = 0;
        manager = 0;
        isAllDay = false;
        parent = 0;
        action = ACTION_ADD;
    }
    ~Data()
    {
        delete manager;
        delete recur;
        delete child;
    }
    QString description;
    QString location;
    OPimNotifyManager* manager;
    OPimRecurrence* recur;
    QString note;
    QDateTime created;
    QDateTime start;
    QDateTime end;
    bool isAllDay : 1;
    QString timezone;
    QArray<int>* child;
    int parent;
    ChangeAction action;
};


OPimEvent::OPimEvent( int uid )
        : OPimRecord( uid )
{
    data = new Data;
}


OPimEvent::OPimEvent( const OPimEvent& ev )
        : OPimRecord( ev ), data( ev.data )
{
    data->ref();
}


OPimEvent::OPimEvent( const QMap<int, QString> map )
        : OPimRecord( 0 )
{
    data = new Data;

    fromMap( map );
}


OPimEvent::~OPimEvent()
{
    if ( data->deref() )
    {
        delete data;
        data = 0;
    }
}


OPimEvent& OPimEvent::operator=( const OPimEvent& ev )
{
    if ( this == &ev ) return * this;

    OPimRecord::operator=( ev );
    ev.data->ref();
    deref();
    data = ev.data;


    return *this;
}


QString OPimEvent::description() const
{
    return data->description;
}


void OPimEvent::setDescription( const QString& description )
{
    changeOrModify();
    data->description = description;
}


void OPimEvent::setLocation( const QString& loc )
{
    changeOrModify();
    data->location = loc;
}


QString OPimEvent::location() const
{
    return data->location;
}


OPimNotifyManager &OPimEvent::notifiers() const
{
    // I hope we can skip the changeOrModify here
    // the notifier should take care of it
    // and OPimNotify is shared too
    if ( !data->manager )
        data->manager = new OPimNotifyManager;

    return *data->manager;
}


bool OPimEvent::hasNotifiers() const
{
    if ( !data->manager )
        return false;
    if ( data->manager->reminders().isEmpty() &&
            data->manager->alarms().isEmpty() )
        return false;

    return true;
}


OPimRecurrence OPimEvent::recurrence() const
{
    if ( !data->recur ) {
        data->recur = new OPimRecurrence;
        data->recur->setStart( data->start.date() );
    }

    return *data->recur;
}


void OPimEvent::setRecurrence( const OPimRecurrence& rec )
{
    changeOrModify();
    if ( data->recur )
        ( *data->recur ) = rec;
    else
        data->recur = new OPimRecurrence( rec );

    data->recur->setStart( data->start.date() );
}


bool OPimEvent::hasRecurrence() const
{
    if ( !data->recur ) return false;
    return data->recur->doesRecur();
}


QString OPimEvent::note() const
{
    return data->note;
}


void OPimEvent::setNote( const QString& note )
{
    changeOrModify();
    data->note = note;
}


QDateTime OPimEvent::createdDateTime() const
{
    return data->created;
}


void OPimEvent::setCreatedDateTime( const QDateTime& time )
{
    changeOrModify();
    data->created = time;
}


QDateTime OPimEvent::startDateTime() const
{
    if ( data->isAllDay )
        return QDateTime( data->start.date(), QTime( 0, 0, 0 ) );
    return data->start;
}


QDateTime OPimEvent::startDateTimeInZone() const
{
    /* if no timezone, or all day event or if the current and this timeZone match... */
    if ( data->timezone.isEmpty() || data->isAllDay || data->timezone == OPimTimeZone::current().timeZone() ) return startDateTime();

    OPimTimeZone zone( data->timezone );
    return zone.toDateTime( data->start, OPimTimeZone::current() );
}


void OPimEvent::setStartDateTime( const QDateTime& dt )
{
    changeOrModify();
    data->start = dt;
    if ( data->recur )
        data->recur->setStart( data->start.date() );
}


QDateTime OPimEvent::endDateTime() const
{
    /*
     * if all Day event the end time needs
     * to be on the same day as the start
     */
    if ( data->isAllDay ) {
        QDate end = data->end.isValid() ? data->end.date() : data->start.date() ;
        return QDateTime( end, QTime( 23, 59, 59 ) );
    }
    return data->end;
}


QDateTime OPimEvent::endDateTimeInZone() const
{
    /* if no timezone, or all day event or if the current and this timeZone match... */
    if ( data->timezone.isEmpty() || data->isAllDay || data->timezone == OPimTimeZone::current().timeZone() ) return endDateTime();

    OPimTimeZone zone( data->timezone );
    return zone.toDateTime( data->end, OPimTimeZone::current() );
}


void OPimEvent::setEndDateTime( const QDateTime& dt )
{
    changeOrModify();
    data->end = dt;
}


bool OPimEvent::isMultipleDay() const
{
    return data->end.date().day() - data->start.date().day();
}


bool OPimEvent::isAllDay() const
{
    return data->isAllDay;
}


void OPimEvent::setAllDay( bool allDay )
{
    changeOrModify();
    data->isAllDay = allDay;
}


void OPimEvent::setTimeZone( const QString& tz )
{
    changeOrModify();
    data->timezone = tz;
}


QString OPimEvent::timeZone() const
{
    if ( data->isAllDay ) return QString::null;
    return data->timezone;
}


bool OPimEvent::match( const QRegExp& re ) const
{
    if ( re.match( data->description ) != -1 )
    {
        setLastHitField( Qtopia::DatebookDescription );
        return true;
    }
    if ( re.match( data->note ) != -1 )
    {
        setLastHitField( Qtopia::Note );
        return true;
    }
    if ( re.match( data->location ) != -1 )
    {
        setLastHitField( Qtopia::Location );
        return true;
    }
    if ( re.match( data->start.toString() ) != -1 )
    {
        setLastHitField( Qtopia::StartDateTime );
        return true;
    }
    if ( re.match( data->end.toString() ) != -1 )
    {
        setLastHitField( Qtopia::EndDateTime );
        return true;
    }
    return false;
}


QString OPimEvent::toRichText() const
{
    QString text, value;

    // description
    text += "<b><h3><img src=\"datebook/DateBook\">";
    if ( !description().isEmpty() )
    {
        text += Qtopia::escapeString( description() ).replace( QRegExp( "[\n]" ), "" );
    }
    text += "</h3></b><br><hr><br>";

    // location
    if ( !( value = location() ).isEmpty() )
    {
        text += "<b>" + QObject::tr( "Location:" ) + "</b> ";
        text += Qtopia::escapeString( value ) + "<br>";
    }

    // all day event
    if ( isAllDay() )
    {
        text += "<b><i>" + QObject::tr( "This is an all day event" ) + "</i></b><br>";
    }
    // multiple day event
    else if ( isMultipleDay () )
    {
        text += "<b><i>" + QObject::tr( "This is a multiple day event" ) + "</i></b><br>";
    }
    // start & end times
    else
    {
        // start time
        if ( startDateTime().isValid() )
        {
            text += "<b>" + QObject::tr( "Start:" ) + "</b> ";
            text += Qtopia::escapeString( startDateTime().toString() ).
                    replace( QRegExp( "[\n]" ), "<br>" ) + "<br>";
        }

        // end time
        if ( endDateTime().isValid() )
        {
            text += "<b>" + QObject::tr( "End:" ) + "</b> ";
            text += Qtopia::escapeString( endDateTime().toString() ).
                    replace( QRegExp( "[\n]" ), "<br>" ) + "<br>";
        }
    }

    // categories
    if ( categoryNames( "Calendar" ).count() )
    {
        text += "<b>" + QObject::tr( "Category:" ) + "</b> ";
        text += categoryNames( "Calendar" ).join( ", " );
        text += "<br>";
    }

    //notes
    if ( !note().isEmpty() )
    {
        text += "<b>" + QObject::tr( "Note:" ) + "</b><br>";
        text += note();
        //         text += Qtopia::escapeString(note() ).
        //                 replace(QRegExp( "[\n]"),  "<br>" ) + "<br>";
    }
    return text;
}


QString OPimEvent::toShortText() const
{
    QString text;
    text += QString::number( startDateTime().date().day() );
    text += ".";
    text += QString::number( startDateTime().date().month() );
    text += ".";
    text += QString::number( startDateTime().date().year() );
    text += " ";
    text += QString::number( startDateTime().time().hour() );
    text += ":";
    text += QString::number( startDateTime().time().minute() );
    text += " - ";
    text += description();
    return text;
}


QString OPimEvent::type() const
{
    return QString::fromLatin1( "OPimEvent" );
}


QString OPimEvent::recordField( int /*id */ ) const
{
    return QString::null;
}


int OPimEvent::rtti() const
{
    return OPimResolver::DateBook;
}

/**
 * \brief Cast safely to OPimEvent from OPimRecord
 *
 * Safely cast from OPimRecord to OPimEvent. If the
 * OPimRecord is not of type OPimEvent Null will be
 * returned.
 *
 * @param rec The OPimRecord to be casted to OPimEvent
 *
 * @see OPimTodo::safeCast
 * @return OPimEvent or Null Pointer
 */
OPimEvent* OPimEvent::safeCast( const OPimRecord* rec) {
    return ( rec && rec->rtti() == OPimResolver::DateBook ) ?
        static_cast<OPimEvent*>( const_cast<OPimRecord*>(rec) ) :
        0l;
}



bool OPimEvent::loadFromStream( QDataStream& )
{
    return true;
}


bool OPimEvent::saveToStream( QDataStream& ) const
{
    return true;
}


void OPimEvent::changeOrModify()
{
    if ( data->count != 1 )
    {
        data->deref();
        Data* d2 = new Data;
        d2->description = data->description;
        d2->location = data->location;

        if ( data->manager )
            d2->manager = new OPimNotifyManager( *data->manager );

        if ( data->recur )
            d2->recur = new OPimRecurrence( *data->recur );

        d2->note = data->note;
        d2->created = data->created;
        d2->start = data->start;
        d2->end = data->end;
        d2->isAllDay = data->isAllDay;
        d2->timezone = data->timezone;
        d2->parent = data->parent;

        if ( data->child )
        {
            d2->child = new QArray<int>( *data->child );
            d2->child->detach();
        }

        d2->action = data->action;

        data = d2;
    }
}


void OPimEvent::deref()
{
    if ( data->deref() )
    {
        delete data;
        data = 0;
    }
}
// Exporting Event data to map. Using the same
// encoding as ODateBookAccessBackend_xml does..
// Thus, we could remove the stuff there and use this
// for it and for all other places..
// Encoding should happen at one place, only ! (eilers)
QMap<int, QString> OPimEvent::toMap() const
{
    QMap<int, QString> retMap;

    retMap.insert( OPimEvent::FUid, QString::number( uid() ) );
    retMap.insert( OPimEvent::FCategories, Qtopia::escapeString( Qtopia::Record::idsToString( categories() ) ) );
    retMap.insert( OPimEvent::FDescription, Qtopia::escapeString( description() ) );
    retMap.insert( OPimEvent::FLocation, Qtopia::escapeString( location() ) );
    retMap.insert( OPimEvent::FType, isAllDay() ? "AllDay" : "" );
    if ( notifiers().alarms().count() ){
        // Currently we just support one alarm.. (eilers)
        OPimAlarm alarm = notifiers().alarms() [ 0 ];
        retMap.insert( OPimEvent::FAlarm, QString::number( alarm.dateTime().secsTo( startDateTime() ) / 60 ) );
        retMap.insert( OPimEvent::FSound, ( alarm.sound() == OPimAlarm::Loud ) ? "loud" : "silent" );
    }

    OPimTimeZone zone( OPimTimeZone::current() );
    retMap.insert( OPimEvent::FStart, QString::number( zone.fromDateTime( startDateTime())));
    retMap.insert( OPimEvent::FEnd, QString::number( zone.fromDateTime(   endDateTime() )));
    retMap.insert( OPimEvent::FNote, Qtopia::escapeString( note() ) );
    if( !isAllDay() )
        retMap.insert( OPimEvent::FTimeZone, timeZone().isEmpty() ? QString( "None" ) : timeZone() );
    if ( parent() )
        retMap.insert( OPimEvent::FRecParent, QString::number( parent() ) );
    if ( children().count() )
    {
        QArray<int> childr = children();
        QString buf;
        for ( uint i = 0; i < childr.count(); i++ )
        {
            if ( i != 0 ) buf += " ";
            buf += QString::number( childr[ i ] );
        }
        retMap.insert( OPimEvent::FRecChildren, buf );
    }

    // Add recurrence stuff
    if ( hasRecurrence() )
    {
        OPimRecurrence recur = recurrence();
        QMap<int, QString> recFields = recur.toMap();
        retMap.insert( OPimEvent::FRType, recFields[ OPimRecurrence::RType ] );
        retMap.insert( OPimEvent::FRWeekdays, recFields[ OPimRecurrence::RWeekdays ] );
        retMap.insert( OPimEvent::FRPosition, recFields[ OPimRecurrence::RPosition ] );
        retMap.insert( OPimEvent::FRFreq, recFields[ OPimRecurrence::RFreq ] );
        retMap.insert( OPimEvent::FRHasEndDate, recFields[ OPimRecurrence::RHasEndDate ] );
        retMap.insert( OPimEvent::FREndDate, recFields[ OPimRecurrence::EndDate ] );
        retMap.insert( OPimEvent::FRCreated, recFields[ OPimRecurrence::Created ] );
        retMap.insert( OPimEvent::FRExceptions, recFields[ OPimRecurrence::Exceptions ] );
    }

    ChangeAction action = this->action();
    if( action != ACTION_ADD )
        retMap.insert( FIELDID_ACTION, actionToStr( action ) );

    return retMap;
}


void OPimEvent::fromMap( const QMap<int, QString>& map )
{

    // We just want to set the UID if it is really stored.
    if ( !map[ OPimEvent::FUid ].isEmpty() )
        setUid( map[ OPimEvent::FUid ].toInt() );

    setCategories( idsFromString( map[ OPimEvent::FCategories ] ) );
    setDescription( map[ OPimEvent::FDescription ] );
    setLocation( map[ OPimEvent::FLocation ] );

    if ( map[ OPimEvent::FType ] == "AllDay" )
        setAllDay( true );
    else
        setAllDay( false );

    if ( !map[ OPimEvent::FTimeZone ].isEmpty() && ( map[ OPimEvent::FTimeZone ] != "None" ) )
    {
        setTimeZone( map[ OPimEvent::FTimeZone ] );
    }

    time_t start = ( time_t ) map[ OPimEvent::FStart ].toLong();
    time_t end = ( time_t )   map[ OPimEvent::FEnd ].toLong();

    /* Someone thought AllDay events should be in UTC here, but they weren't in Datebook 1, so... */
    /* Convert to current date time */
    OPimTimeZone to_zone( OPimTimeZone::current() );
    setStartDateTime(to_zone.toDateTime( start));
    setEndDateTime  (to_zone.toDateTime( end));

    int alarmTime = -1;
    if ( !map[ OPimEvent::FAlarm ].isEmpty() )
        alarmTime = map[ OPimEvent::FAlarm ].toInt();

    int sound = ( ( map[ OPimEvent::FSound ] == "loud" ) ? OPimAlarm::Loud : OPimAlarm::Silent );
    if ( ( alarmTime != -1 ) )
    {
        QDateTime dt = startDateTime().addSecs( -1 * alarmTime * 60 );
        OPimAlarm al( sound , dt );
        notifiers().add( al );
    }


    if ( !map[ OPimEvent::FNote ].isEmpty() )
        setNote( map[ OPimEvent::FNote ] );

    if ( !map[ OPimEvent::FRecParent ].isEmpty() )
        setParent( map[ OPimEvent::FRecParent ].toInt() );

    if ( !map[ OPimEvent::FRecChildren ].isEmpty() )
    {
        QStringList list = QStringList::split( ' ', map[ OPimEvent::FRecChildren ] );
        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
        {
            addChild( ( *it ).toInt() );
        }
    }

    // Fill recurrence stuff and put it directly into the OPimRecurrence-Object using fromMap..
    if ( !map[ OPimEvent::FRType ].isEmpty() )
    {
        QMap<int, QString> recFields;
        recFields.insert( OPimRecurrence::RType, map[ OPimEvent::FRType ] );
        recFields.insert( OPimRecurrence::RWeekdays, map[ OPimEvent::FRWeekdays ] );
        recFields.insert( OPimRecurrence::RPosition, map[ OPimEvent::FRPosition ] );
        recFields.insert( OPimRecurrence::RFreq, map[ OPimEvent::FRFreq ] );
        recFields.insert( OPimRecurrence::RHasEndDate, map[ OPimEvent::FRHasEndDate ] );
        recFields.insert( OPimRecurrence::EndDate, map[ OPimEvent::FREndDate ] );
        recFields.insert( OPimRecurrence::Created, map[ OPimEvent::FRCreated ] );
        recFields.insert( OPimRecurrence::Exceptions, map[ OPimEvent::FRExceptions ] );
        OPimRecurrence recur( recFields );
        setRecurrence( recur );
    }

    data->action = strToAction( map[ FIELDID_ACTION ] );
}


int OPimEvent::parent() const
{
    return data->parent;
}


void OPimEvent::setParent( int uid )
{
    changeOrModify();
    data->parent = uid;
}


QArray<int> OPimEvent::children() const
{
    if ( !data->child ) return QArray<int>();
    else
        return data->child->copy();
}


void OPimEvent::setChildren( const QArray<int>& arr )
{
    changeOrModify();
    if ( data->child ) delete data->child;

    data->child = new QArray<int>( arr );
    data->child->detach();
}


void OPimEvent::addChild( int uid )
{
    changeOrModify();
    if ( !data->child )
    {
        data->child = new QArray<int>( 1 );
        ( *data->child ) [ 0 ] = uid;
    }
    else
    {
        int count = data->child->count();
        data->child->resize( count + 1 );
        ( *data->child ) [ count ] = uid;
    }
}


void OPimEvent::removeChild( int uid )
{
    if ( !data->child || !data->child->contains( uid ) ) return ;
    changeOrModify();
    QArray<int> newAr( data->child->count() - 1 );
    int j = 0;
    uint count = data->child->count();
    for ( uint i = 0; i < count; i++ )
    {
        if ( ( *data->child ) [ i ] != uid )
        {
            newAr[ j ] = ( *data->child ) [ i ];
            j++;
        }
    }
    ( *data->child ) = newAr;
}

OPimRecord::ChangeAction OPimEvent::action() const
{
    return data->action;
}

OPimEvent::CompareResult OPimEvent::compareTo( const OPimEvent &ev ) const
{
    bool minorDiff = false;

    if( data == ev.data )  
        return Equal;    // it's an unmodified copy of the same object
    
    if( data->description != ev.data->description )
        return Different;
    if( data->location != ev.data->location )
        return Different;
    if( data->manager != ev.data->manager )
        return Different;
    if( data->recur != ev.data->recur ) {
        if( ! data->recur->equals( *ev.data->recur ) )
            minorDiff = true;
    }
    if( data->note != ev.data->note )
        return Different;
    if( data->start != ev.data->start )
        return Different;
    if( data->end != ev.data->end )
        return Different;
    if( data->created != ev.data->created )
        minorDiff = true;
    if( data->isAllDay != ev.data->isAllDay )
        return Different;
    if( data->timezone != ev.data->timezone )
        minorDiff = true;
    if( data->child != ev.data->child )
        minorDiff = true;
    if( data->parent != ev.data->parent )
        minorDiff = true;

    if( minorDiff )
        return Similar;
    else
        return Equal;
}

}
