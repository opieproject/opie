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

#include "event.h"
#include "qfiledirect_p.h"
#include <qtopia/timeconversion.h>
#include <qtopia/stringutil.h>
#include <qtopia/private/recordfields.h>
#include <qbuffer.h>
#include <time.h>
#include "vobject_p.h"

#include <stdio.h>

using namespace Qtopia;

static void write( QString& buf, const Event::RepeatPattern &r )
{
    buf += " rtype=\"";
    switch ( r.type ) {
	case Event::Daily:
	    buf += "Daily";
	    break;
	case Event::Weekly:
	    buf += "Weekly";
	    break;
	case Event::MonthlyDay:
	    buf += "MonthlyDay";
	    break;
	case Event::MonthlyDate:
	    buf += "MonthlyDate";
	    break;
	case Event::Yearly:
	    buf += "Yearly";
	    break;
	default:
	    buf += "NoRepeat";
	    break;
    }
    buf += "\"";
    if ( r.days > 0 )
	buf += " rweekdays=\"" + QString::number( static_cast<int>( r.days ) ) + "\"";
    if ( r.position != 0 )
	buf += " rposition=\"" + QString::number( r.position ) + "\"";

    buf += " rfreq=\"" + QString::number( r.frequency ) + "\"";
    buf += " rhasenddate=\"" + QString::number( static_cast<int>( r.hasEndDate ) ) + "\"";
     if ( r.hasEndDate )
	buf += " enddt=\""
	       + QString::number( r.endDateUTC ? r.endDateUTC : time( 0 ) )
	       + "\"";
    buf += " created=\"" + QString::number( r.createTime ) + "\"";
}

Qtopia::UidGen Event::sUidGen( Qtopia::UidGen::Qtopia );

/*!
  \class Event event.h
  \brief The Event class holds the data of a calendar event.

  This data includes descriptive data of the event and schedualing information.

  \ingroup qtopiaemb
  \ingroup qtopiadesktop
*/

/*!
  \class Event::RepeatPattern
  \class The Event::RepeatPattern class is internal.
  \internal
*/

/*!
  \enum Event::Days
  \internal
*/

/*!
  \enum Event::Type
  \internal
*/

/*!
  \enum Event::SoundTypeChoice

  This enum type defines what kind of sound is made when an alarm occurs
  for an event.  The currently defined types are:

  <ul>
  <li>\c Silent - No sound is produced.
  <li>\c Loud - A loud sound is produced.
  </ul>
*/

/*!
  \fn bool Event::operator<( const Event & ) const
  \internal
*/

/*!
  \fn bool Event::operator<=( const Event & ) const
  \internal
*/

/*!
  \fn bool Event::operator!=( const Event & ) const
  \internal
*/

/*!
  \fn bool Event::operator>( const Event & ) const
  \internal
*/

/*!
  \fn bool Event::operator>=( const Event & ) const
  \internal
*/

/*!
  \enum Event::RepeatType

  This enum defines how a event will repeat, if at all.

  <ul>
  <li>\c NoRepeat - Event does not repeat.
  <li>\c Daily - Event occurs every n days.
  <li>\c Weekly - Event occurs every n weeks.
  <li>\c MonthlyDay - Event occurs every n months.  Event will always occur in
  the same week and same day of week as the first event.
  <li>\c MonthlyDate - Event occurs every n months.  Event will always occur
  on the same day of the month as the first event.
  <li>\c Yearly - Event occurs every n years.
  </ul>
*/

/*!
  \fn bool Event::isAllDay() const

  Returns TRUE if the event is an all day event.  Otherwise returns FALSE.
*/

/*!
  \fn void Event::setAllDay(bool allday)

  If \a allday is TRUE, will set the event to be an all day event.
  Otherwise sets the event to not be an all day event.

  \warning This function may affect the start and end times of the event.
*/

/*!
  \fn QDateTime Event::start() const

  Returns the start date and time of the first occurance of the event.
*/

/*!
  \fn QDateTime Event::end() const

  Returns the end date and time of the first occurance of the event.
*/

/*!
  \fn time_t Event::startTime() const
  \internal
*/

/*!
  \fn time_t Event::endTime() const
  \internal
*/

/*!
  \fn void Event::setAlarm(int delay, SoundTypeChoice s)

  Sets the alarm delay of the event to \a delay and the sound type of the
  alarm to \a s.
*/

/*!
  \fn void Event::clearAlarm()

  Clears the alarm for the event.
*/

/*!
  \fn int Event::alarmDelay() const

  Returns the delay in minutes between the alarm for an event and the
  start of the event.
*/

/*!
  \fn Event::RepeatType Event::repeatType() const

  Returns the repeat pattern type for the event.

  \sa frequency()
*/

/*!
  \fn int Event::weekOffset() const

  Returns the number of weeks from the start of the month that this event
  occurs.
*/

/*!
  \fn QDate Event::repeatTill() const

  Returns the date that the event will continue to repeat until.  If the event
  repeats forever the value returned is undefined.

  \sa repeatForever()
*/

/*!
  \fn bool Event::repeatForever() const

  Returns FALSE if there is a date set for the event to continue until.
  Otherwise returns TRUE.
*/

/*!
  \fn bool Event::doRepeat() const
  \internal
*/

/*!
  \fn bool Event::repeatOnWeekDay(int day) const

  Returns TRUE if the event has a RepeatType of Weekly and is set to occur on
  \a day each week.  Otherwise returns FALSE.

  \sa QDate::dayName()
*/

/*!
  \fn void Event::setRepeatOnWeekDay(int day, bool enable)

  If \a enable is TRUE then sets the event to occur on \a day each week.
  Otherwise sets the event not to occur on \a day.

  \warning this function is only relavent for a event with RepeatType of
  Weekly.

  \sa QDate::dayName()
*/

/*!
  \fn int Event::frequency() const

  Returns how often the event repeats.

  \sa repeatType()
*/

/*!
  \fn void Event::setRepeatType(RepeatType t)

  Sets the repeat pattern type of the event to \a t.

  \sa setFrequency()
*/

/*!
  \fn void Event::setFrequency(int n)

  Sets how often the event occurs with in its repeat pattern.

  \sa setRepeatType()
*/

/*!
  \fn void Event::setRepeatTill(const QDate &d)

  Sets the event to repeat until \a d.
*/

/*!
  \fn void Event::setRepeatForever(bool enable)

  If \a enable is TRUE, sets the event to repeat forever.  Otherwise
  sets the event to stop repeating at some date.

  \warning This function may affect the specific date the event will repeat
  till.
*/

/*!
  \fn bool Event::match(const QRegExp &r) const

  Returns TRUE if the event matches the regular expression \a r.
  Otherwise returns FALSE.
*/

/*!
  \fn char Event::day(int)
  \internal
*/

/*!
  Creates a new, empty event.
*/
Event::Event() : Record()
{
    startUTC = endUTC = time( 0 );
    typ = Normal;
    hAlarm = FALSE;
    hRepeat = FALSE;
    aMinutes = 0;
    aSound = Silent;
    pattern.type = NoRepeat;
    pattern.frequency = -1;
}

/*!
  \internal
*/
Event::Event( const QMap<int, QString> &map )
{
    setDescription( map[DatebookDescription] );
    setLocation( map[Location] );
    setCategories( idsFromString( map[DatebookCategory] ) );
    setTimeZone( map[TimeZone] );
    setNotes( map[Note] );
    setStart( TimeConversion::fromUTC( map[StartDateTime].toUInt() ) );
    setEnd( TimeConversion::fromUTC( map[EndDateTime].toUInt() ) );
    setType( (Event::Type) map[DatebookType].toInt() );
    setAlarm( ( map[HasAlarm] == "1" ? TRUE : FALSE ), map[AlarmTime].toInt(), (Event::SoundTypeChoice)map[SoundType].toInt() );
    Event::RepeatPattern p;
    p.type = (Event::RepeatType) map[ RepeatPatternType ].toInt();
    p.frequency = map[ RepeatPatternFrequency ].toInt();
    p.position = map[ RepeatPatternPosition ].toInt();
    p.days = map[ RepeatPatternDays ].toInt();
    p.hasEndDate = map[ RepeatPatternHasEndDate ].toInt();
    p.endDateUTC = map[ RepeatPatternEndDate ].toUInt();
    setRepeat( p );

    setUid( map[ DatebookUid ].toInt() );
}

/*!
  Destroys an event.
*/
Event::~Event()
{
}

/*!
  \internal
*/
int Event::week( const QDate& date )
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

/*!
  \internal
*/
int Event::occurrence( const QDate& date )
{
    // calculates the number of occurrances of this day of the
    // week till the given date (e.g 3rd Wednesday of the month)
    return ( date.day() - 1 ) / 7 + 1;
}

/*!
  \internal
*/
int Event::dayOfWeek( char day )
{
    int dayOfWeek = 1;
    char i = Event::MON;
    while ( !( i & day ) && i <= Event::SUN ) {
	i <<= 1;
	++dayOfWeek;
    }
    return dayOfWeek;
}

/*!
  \internal
*/
int Event::monthDiff( const QDate& first, const QDate& second )
{
    return ( second.year() - first.year() ) * 12 +
	second.month() - first.month();
}

/*!
  \internal
*/
QMap<int, QString> Event::toMap() const
{
    QMap<int, QString> m;

    if ( !description().isEmpty() )
	m.insert( DatebookDescription, description() );
    if ( !location().isEmpty() )
	m.insert ( Location, location() );
    if ( categories().count() )
	m.insert ( DatebookCategory, idsToString( categories() ) );
    if ( !timeZone().isEmpty() )
	m.insert ( TimeZone, timeZone() );
    if ( !notes().isEmpty() )
	m.insert ( Note, notes() );

    m.insert ( StartDateTime, QString::number( TimeConversion::toUTC( start() ) ) );
    m.insert ( EndDateTime, QString::number( TimeConversion::toUTC( end() ) ) );
    m.insert ( DatebookType, QString::number( (int)type() ) );
    m.insert ( HasAlarm, ( hasAlarm() ? "1" : "0" ) );
    m.insert ( SoundType, QString::number( (int)alarmSound() ) );
    m.insert ( AlarmTime, QString::number( alarmTime() ) );
    m.insert ( RepeatPatternType, QString::number( static_cast<int>( repeatPattern().type ) ) );
    m.insert ( RepeatPatternFrequency, QString::number( repeatPattern().frequency ) );
    m.insert ( RepeatPatternPosition, QString::number( repeatPattern().position ) );
    m.insert ( RepeatPatternDays, QString::number( repeatPattern().days ) );
    m.insert ( RepeatPatternHasEndDate, QString::number( static_cast<int>( repeatPattern().hasEndDate ) ) );
    m.insert ( RepeatPatternEndDate, QString::number( repeatPattern().endDateUTC ) );

    m.insert( DatebookUid, QString::number( uid()) );

    return m;
}

/*!
  \internal
*/
void Event::setRepeat( const RepeatPattern &p )
{
    setRepeat( p.type != NoRepeat, p );
}

/*!
  Sets the description of the event to \a s.
*/
void Event::setDescription( const QString &s )
{
    descript = s;
}

/*!
  Sets the location of the event to \a s.
*/
void Event::setLocation( const QString &s )
{
    locat = s;
}

// void Event::setCategory( const QString &s )
// {
//     categ = s;
// }

/*!
  \internal
*/
void Event::setType( Type t )
{
    typ = t;
}

/*!
  Sets the start date and time of the first or only occurance of this event
  to the date and time \a d. \a d should be in local time.
*/
void Event::setStart( const QDateTime &d )
{
    startUTC = TimeConversion::toUTC( d );
}

/*!
  \internal
*/
void Event::setStart( time_t time )
{
    startUTC = time;
}

/*!
  Sets the end date and time of the first or only occurance of this event
  to the date and time \a d.  \a d should be in local time.
*/
void Event::setEnd( const QDateTime &d )
{
    endUTC = TimeConversion::toUTC( d );
}

/*!
  \internal
*/
void Event::setEnd( time_t time )
{
    endUTC = time;
}

/*!
  \internal
*/
void Event::setTimeZone( const QString &z )
{
    tz = z;
}

/*!
  \internal
*/
void Event::setAlarm( bool b, int minutes, SoundTypeChoice s )
{
    hAlarm = b;
    aMinutes = minutes;
    aSound = s;
}

/*!
  \internal
*/
void Event::setRepeat( bool b, const RepeatPattern &p )
{
    hRepeat = b;
    pattern = p;
}

/*!
  Sets the notes for the event to \a n.
*/
void Event::setNotes( const QString &n )
{
    note = n;
}

/*!
  Returns the description of the event.
*/
const QString &Event::description() const
{
    return descript;
}

/*!
  Returns the location of the event.
*/
const QString &Event::location() const
{
    return locat;
}

// QString Event::category() const
// {
//     return categ;
// }

/*!
  \internal
*/
Event::Type Event::type() const
{
    return typ;
}
/*
QDateTime Event::start() const {
    return start( TRUE );
}
*/
/*!
  \internal
*/
QDateTime Event::start( bool actual ) const
{
    QDateTime dt = (startUTC > 0) ? TimeConversion::fromUTC( startUTC ) : QDateTime::currentDateTime();

    if ( actual && typ == AllDay ) {
	QTime t = dt.time();
	t.setHMS( 0, 0, 0 );
	dt.setTime( t );
    }
    return dt;
}
/*
QDateTime Event::end() const {
    return end( TRUE );
}
*/
/*!
  \internal
*/
QDateTime Event::end( bool actual ) const
{
    QDateTime dt = (endUTC > 0) ? TimeConversion::fromUTC( endUTC ) : QDateTime::currentDateTime();

    if ( actual && typ == AllDay ) {
	QTime t = dt.time();
	t.setHMS( 23, 59, 59 );
	dt.setTime( t );
    }
    return dt;
}

/*!
  \internal
*/
const QString &Event::timeZone() const
{
    return tz;
}

/*!
  \internal
*/
bool Event::hasAlarm() const
{
    return hAlarm;
}

/*!
  \internal
*/
int Event::alarmTime() const
{
    return aMinutes;
}

/*!
  Returns the sound type for the alarm of this event.
*/
Event::SoundTypeChoice Event::alarmSound() const
{
    return aSound;
}

/*!
  \internal
*/
bool Event::hasRepeat() const
{
    return doRepeat();
}

/*!
  \internal
*/
const Event::RepeatPattern &Event::repeatPattern() const
{
    return pattern;
}

/*!
  \internal
*/
Event::RepeatPattern &Event::repeatPattern()
{
    return pattern;
}

/*!
  Returns the notes for the event.
*/
const QString &Event::notes() const
{
    return note;
}

/*!
  \internal
*/
bool Event::operator==( const Event &e ) const
{
    if ( uid() && e.uid() == uid() )
	return TRUE;
    return ( e.descript == descript &&
	     e.locat == locat &&
	     e.categ == categ &&
	     e.typ == typ &&
	     e.startUTC == startUTC &&
	     e.endUTC == endUTC &&
	     e.tz == tz &&
	     e.hAlarm == hAlarm &&
	     e.aMinutes == aMinutes &&
	     e.aSound == aSound &&
	     e.hRepeat == hRepeat &&
	     e.pattern == pattern &&
	     e.note == note );
}

/*!
  \internal
  Appends the contact information to \a buf.
*/
void Event::save( QString& buf )
{
    buf += " description=\"" + Qtopia::escapeString(descript) + "\"";
    if ( !locat.isEmpty() )
	buf += " location=\"" + Qtopia::escapeString(locat) + "\"";
    // save the categoies differently....
    QString strCats = idsToString( categories() );
    buf += " categories=\"" + Qtopia::escapeString(strCats) + "\"";
    buf += " uid=\"" + QString::number( uid() ) + "\"";
    if ( (Type)typ != Normal )
	buf += " type=\"AllDay\"";
    if ( hAlarm ) {
	buf += " alarm=\"" + QString::number( aMinutes ) + "\" sound=\"";
	if ( aSound == Event::Loud )
	    buf += "loud";
	else
	    buf += "silent";
	buf += "\"";
    }
    if ( hRepeat )
	write( buf, pattern );

    buf += " start=\""
	   + QString::number( startUTC )
	   + "\"";

    buf += " end=\""
	   + QString::number( endUTC )
	   + "\"";

    if ( !note.isEmpty() )
	buf += " note=\"" + Qtopia::escapeString( note ) + "\"";
    buf += customToXml();
}

/*!
  \internal
*/
bool Event::RepeatPattern::operator==( const Event::RepeatPattern &right ) const
{
    // *sigh*
    return ( type == right.type
	     && frequency == right.frequency
	     && position == right.position
	     && days == right.days
	     && hasEndDate == right.hasEndDate
	     && endDateUTC == right.endDateUTC
	     && createTime == right.createTime );
}

/*!
  \class EffectiveEvent
  \brief The EffectiveEvent class the data for a single occurance of an event.

  This class describes the event for a single occurance of it.  For example if
  an Event occurs every week, the effective event might represent the third
  occurance of this Event.

  \ingroup qtopiaemb
  \ingroup qtopiadesktop
  \warning This class will be phased out in Qtopia 3.x
*/

/*!
  \enum EffectiveEvent::Position
  \internal
*/

/*!
  \fn EffectiveEvent &EffectiveEvent::operator=(const EffectiveEvent &)
  \internal
*/

class EffectiveEventPrivate
{
public:
    //currently the existence of the d pointer means multi-day repeating,
    //msut be changed if we use the d pointer for anything else.
    QDate startDate;
    QDate endDate;
};

/*!
  \internal
*/
EffectiveEvent::EffectiveEvent()
{
    mDate = QDate::currentDate();
    mStart = mEnd = QTime::currentTime();
    d = 0;
}

/*!
  \internal
*/
EffectiveEvent::EffectiveEvent( const Event &e, const QDate &date, Position pos )
{
    mEvent = e;
    mDate = date;
    if ( pos & Start )
	mStart = e.start( TRUE ).time();
    else
	mStart = QTime( 0, 0, 0 );

    if ( pos & End )
	mEnd = e.end( TRUE ).time();
    else
	mEnd = QTime( 23, 59, 59 );
    d = 0;
}

/*!
  \internal
*/
EffectiveEvent::~EffectiveEvent()
{
    delete d;
}

/*!
  \internal
*/
EffectiveEvent::EffectiveEvent( const EffectiveEvent &e )
{
    d = 0;
    *this = e;
}

EffectiveEvent& EffectiveEvent::operator=( const EffectiveEvent & e )
{
    if ( &e == this )
	return *this;
    delete d;
    if ( e.d ) {
	d = new EffectiveEventPrivate;
	d->startDate = e.d->startDate;
	d->endDate = e.d->endDate;
    } else {
	d = 0;
    }
    mEvent = e.mEvent;
    mDate = e.mDate;
    mStart = e.mStart;
    mEnd = e.mEnd;

    return *this;

}

// QString EffectiveEvent::category() const
// {
//     return mEvent.category();
// }

/*!
  Returns the description of the event for this effective event.
*/
const QString &EffectiveEvent::description( ) const
{
    return mEvent.description();
}

/*!
\internal
*/
const QString &EffectiveEvent::location( ) const
{
    return mEvent.location();
}

/*!
\internal
*/
const QString &EffectiveEvent::notes() const
{
    return mEvent.notes();
}

/*!
  Returns the event associated with this effective event.
*/
const Event &EffectiveEvent::event() const
{
    return mEvent;
}

/*!
  \internal
*/
const QTime &EffectiveEvent::end() const
{
    return mEnd;
}

/*!
  \internal
*/
const QTime &EffectiveEvent::start() const
{
    return mStart;
}

/*!
  Returns the date the effective event occurs on.
*/
const QDate &EffectiveEvent::date() const
{
    return mDate;
}

/*!
  \internal
*/
int EffectiveEvent::length() const
{
    return (mEnd.hour() * 60 - mStart.hour() * 60)
	   + QABS(mStart.minute() - mEnd.minute() );
}

/*!
  \internal
*/
void EffectiveEvent::setDate( const QDate &dt )
{
    mDate = dt;
}

/*!
  \internal
*/
void EffectiveEvent::setStart( const QTime &start )
{
    mStart = start;
}

/*!
  \internal
*/
void EffectiveEvent::setEnd( const QTime &end )
{
    mEnd = end;
}

/*!
  \internal
*/
void EffectiveEvent::setEvent( Event e )
{
    mEvent = e;
}

/*!
  \internal
*/
bool EffectiveEvent::operator<( const EffectiveEvent &e ) const
{
    if ( mDate < e.date() )
	return TRUE;
    if ( mDate == e.date() )
	return ( mStart < e.start() );
    else
	return FALSE;
}

/*!
  \internal
*/
bool EffectiveEvent::operator<=( const EffectiveEvent &e ) const
{
    return (mDate <= e.date() );
}

/*!
  \internal
*/
bool EffectiveEvent::operator==( const EffectiveEvent &e ) const
{
    return ( mDate == e.date()
	     && mStart == e.start()
	     && mEnd == e.end()
	     && mEvent == e.event() );
}

/*!
  \internal
*/
bool EffectiveEvent::operator!=( const EffectiveEvent &e ) const
{
    return !(*this == e);
}

/*!
  \internal
*/
bool EffectiveEvent::operator>( const EffectiveEvent &e ) const
{
    return !(*this <= e );
}

/*!
  \internal
*/
bool EffectiveEvent::operator>=(const EffectiveEvent &e) const
{
    return !(*this < e);
}

/*!
  \internal
*/
void EffectiveEvent::setEffectiveDates( const QDate &from, const QDate &to )
{
    if ( !from.isValid() ) {
	delete d;
	d = 0;
	return;
    }
    if ( !d )
	d = new EffectiveEventPrivate;
    d->startDate = from;
    d->endDate = to;
}

/*!
  \internal
*/
QDate EffectiveEvent::startDate() const
{
    if ( d )
	return d->startDate;
    else if ( mEvent.hasRepeat() )
	return mDate; // single day, since multi-day should have a d pointer
    else
	return mEvent.start().date();
}

/*!
  \internal
*/
QDate EffectiveEvent::endDate() const
{
    if ( d )
	return d->endDate;
    else if ( mEvent.hasRepeat() )
	return mDate; // single day, since multi-day should have a d pointer
    else
	return mEvent.end().date();
}

/*!
  \internal
*/
int EffectiveEvent::size() const
{
    return ( mEnd.hour() - mStart.hour() ) * 3600
	     + (mEnd.minute() - mStart.minute() * 60
	     + mEnd.second() - mStart.second() );
}


// vcal conversion code
static inline VObject *safeAddPropValue( VObject *o, const char *prop, const QString &value )
{
    VObject *ret = 0;
    if ( o && !value.isEmpty() )
	ret = addPropValue( o, prop, value.latin1() );
    return ret;
}

static inline VObject *safeAddProp( VObject *o, const char *prop)
{
    VObject *ret = 0;
    if ( o )
	ret = addProp( o, prop );
    return ret;
}

static VObject *createVObject( const Event &e )
{
    VObject *vcal = newVObject( VCCalProp );
    safeAddPropValue( vcal, VCVersionProp, "1.0" );
    VObject *event = safeAddProp( vcal, VCEventProp );

    safeAddPropValue( event, VCDTstartProp, TimeConversion::toISO8601( e.start() ) );
    safeAddPropValue( event, VCDTendProp, TimeConversion::toISO8601( e.end() ) );
    safeAddPropValue( event, "X-Qtopia-NOTES", e.description() );
    safeAddPropValue( event, VCDescriptionProp, e.description() );
    safeAddPropValue( event, VCLocationProp, e.location() );

    if ( e.hasAlarm() ) {
	VObject *alarm = safeAddProp( event, VCAAlarmProp );
	QDateTime dt = e.start();
	dt = dt.addSecs( -e.alarmTime()*60 );
	safeAddPropValue( alarm, VCRunTimeProp, TimeConversion::toISO8601( dt ) );
	safeAddPropValue( alarm, VCAudioContentProp,
			  (e.alarmSound() == Event::Silent ? "silent" : "alarm" ) );
    }

    safeAddPropValue( event, "X-Qtopia-TIMEZONE", e.timeZone() );

    if ( e.type() == Event::AllDay )
	safeAddPropValue( event, "X-Qtopia-AllDay", e.timeZone() );

    // ### repeat missing

    // ### categories missing

    return vcal;
}


static Event parseVObject( VObject *obj )
{
    Event e;

    bool haveAlarm = FALSE;
    bool haveStart = FALSE;
    bool haveEnd = FALSE;
    QDateTime alarmTime;
    Event::SoundTypeChoice soundType = Event::Silent;

    VObjectIterator it;
    initPropIterator( &it, obj );
    while( moreIteration( &it ) ) {
	VObject *o = nextVObject( &it );
	QCString name = vObjectName( o );
	QCString value = vObjectStringZValue( o );
	if ( name == VCDTstartProp ) {
	    e.setStart( TimeConversion::fromISO8601( value ) );
	    haveStart = TRUE;
	}
	else if ( name == VCDTendProp ) {
	    e.setEnd( TimeConversion::fromISO8601( value ) );
	    haveEnd = TRUE;
	}
	else if ( name == "X-Qtopia-NOTES" ) {
	    e.setNotes( value );
	}
	else if ( name == VCDescriptionProp ) {
	    e.setDescription( value );
	}
	else if ( name == VCLocationProp ) {
	    e.setLocation( value );
	}
	else if ( name == VCAudioContentProp ) {
	    haveAlarm = TRUE;
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		QCString value = vObjectStringZValue( o );
		if ( name == VCRunTimeProp )
		    alarmTime = TimeConversion::fromISO8601( value );
		else if ( name == VCAudioContentProp ) {
		    if ( value == "silent" )
			soundType = Event::Silent;
		    else
			soundType = Event::Loud;
		}
	    }
	}
	else if ( name == "X-Qtopia-TIMEZONE") {
	    e.setTimeZone( value );
	}
	else if ( name == "X-Qtopia-AllDay" ) {
	    e.setType( Event::AllDay );
	}
#if 0
	else {
	    printf("Name: %s, value=%s\n", name.data(), vObjectStringZValue( o ) );
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		QString value = vObjectStringZValue( o );
		printf(" subprop: %s = %s\n", name.data(), value.latin1() );
	    }
	}
#endif
    }

    if ( !haveStart && !haveEnd )
	e.setStart( QDateTime::currentDateTime() );

    if ( !haveEnd ) {
	e.setType( Event::AllDay );
	e.setEnd( e.start() );
    }

    if ( haveAlarm ) {
	int minutes = alarmTime.secsTo( e.start() ) / 60;
	e.setAlarm( TRUE, minutes, soundType );
    }
    return e;
}


/*!
    Writes the list of \a events as a set of VCards to the file \a filename.
*/
void Event::writeVCalendar( const QString &filename, const QValueList<Event> &events)
{

	QFileDirect f( filename.utf8().data() );

	if ( !f.open( IO_WriteOnly ) ) {

		qWarning("Unable to open vcard write");

		return;

	}


    QValueList<Event>::ConstIterator it;
    for( it = events.begin(); it != events.end(); ++it ) {
	VObject *obj = createVObject( *it );
	writeVObject( f.directHandle() , obj );
	cleanVObject( obj );
    }


    cleanStrTbl();
}

/*!
    Writes \a event as a VCard to the file \a filename.
*/
void Event::writeVCalendar( const QString &filename, const Event &event)
{

	QFileDirect f( filename.utf8().data() );

	if ( !f.open( IO_WriteOnly ) ) {

		qWarning("Unable to open vcard write");

		return;

	}


    VObject *obj = createVObject( event );
	writeVObject( f.directHandle() , obj );
	cleanVObject( obj );

	cleanStrTbl();
}

/*!
    Returns the set of events read as VCards from the file \a filename.
*/
QValueList<Event> Event::readVCalendar( const QString &filename )
{
    VObject *obj = Parse_MIME_FromFileName( (char *)filename.utf8().data() );

    QValueList<Event> events;

    while ( obj ) {
	QCString name = vObjectName( obj );
	if ( name == VCCalProp ) {
	    VObjectIterator nit;
	    initPropIterator( &nit, obj );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		if ( name == VCEventProp )
		    events.append( parseVObject( o ) );
	    }
	} else if ( name == VCEventProp ) {
	    // shouldn't happen, but just to be sure
	    events.append( parseVObject( obj ) );
	}
	VObject *t = obj;
	obj = nextVObjectInList(obj);
	cleanVObject( t );
    }

    return events;
}

bool Event::match( const QRegExp &r ) const
{
    bool returnMe;
    returnMe = false;

    if ( descript.find( r ) > -1 )
	returnMe = true;
    else if ( locat.find( r ) > -1 )
	returnMe = true;
    else if ( TimeConversion::fromUTC( startUTC ).toString().find( r ) > -1 )
	returnMe = true;
    else if ( TimeConversion::fromUTC( endUTC ).toString().find( r )  > -1 )
	returnMe = true;
    else if ( tz.find( r ) > -1 )
	returnMe = true;
    else if ( note.find( r ) > -1 )
	returnMe = true;
    else if ( doRepeat() ) {
	if ( pattern.hasEndDate )
	    if ( TimeConversion::fromUTC( pattern.endDateUTC ).toString().find(r) > -1 )
		returnMe = true;
    }
    return returnMe;
}
