#include <qshared.h>

#include <qpe/palmtopuidgen.h>
#include <qpe/categories.h>
#include <qpe/stringutil.h>

#include "orecur.h"
#include "opimresolver.h"
#include "opimnotifymanager.h"

#include "oevent.h"

int OCalendarHelper::week( const QDate& date) {
    // Calculates the week this date is in within that
    // month. Equals the "row" is is in in the month view
    int week = 1;
    QDate tmp( date.year(), date.month(), 1 );
    if ( date.dayOfWeek() < tmp.dayOfWeek() )
        ++week;

    week += ( date.day() - 1 ) / 7;

    return week;
}
int OCalendarHelper::ocurrence( const QDate& date) {
    // calculates the number of occurrances of this day of the
    // week till the given date (e.g 3rd Wednesday of the month)
    return ( date.day() - 1 ) / 7 + 1;
}
int OCalendarHelper::dayOfWeek( char day ) {
    int dayOfWeek = 1;
    char i = ORecur::MON;
    while ( !( i & day ) && i <= ORecur::SUN ) {
        i <<= 1;
        ++dayOfWeek;
    }
    return dayOfWeek;
}
int OCalendarHelper::monthDiff( const QDate& first, const QDate& second ) {
    return ( second.year() - first.year() ) * 12 +
        second.month() - first.month();
}

struct OEvent::Data : public QShared {
    Data() : QShared() {
        child = 0;
        recur = 0;
        manager = 0;
        isAllDay = false;
        parent = 0;
    }
    ~Data() {
        delete manager;
        delete recur;
    }
    QString description;
    QString location;
    OPimNotifyManager* manager;
    ORecur* recur;
    QString note;
    QDateTime created;
    QDateTime start;
    QDateTime end;
    bool isAllDay : 1;
    QString timezone;
    QArray<int>* child;
    int parent;
};

OEvent::OEvent( int uid )
    : OPimRecord( uid ) {
    data = new Data;
}
OEvent::OEvent( const OEvent& ev)
    : OPimRecord( ev ), data( ev.data )
{
    data->ref();
}
OEvent::~OEvent() {
    if ( data->deref() ) {
        delete data;
        data = 0;
    }
}
OEvent& OEvent::operator=( const OEvent& ev) {
    if ( this == &ev ) return  *this;

    OPimRecord::operator=( ev );
    ev.data->ref();
    deref();
    data = ev.data;


    return *this;
}
QString OEvent::description()const {
    return data->description;
}
void OEvent::setDescription( const QString& description ) {
    changeOrModify();
    data->description = description;
}
void OEvent::setLocation( const QString& loc ) {
    changeOrModify();
    data->location = loc;
}
QString OEvent::location()const {
    return data->location;
}
OPimNotifyManager &OEvent::notifiers()const {
    // I hope we can skip the changeOrModify here
    // the notifier should take care of it
    // and OPimNotify is shared too
    if (!data->manager )
        data->manager = new OPimNotifyManager;

    return *data->manager;
}
bool OEvent::hasNotifiers()const {
    if (!data->manager )
        return false;
    if (data->manager->reminders().isEmpty() &&
        data->manager->alarms().isEmpty() )
        return false;

    return true;
}
ORecur OEvent::recurrence()const {
    if (!data->recur)
        data->recur = new ORecur;

    return *data->recur;
}
void OEvent::setRecurrence( const ORecur& rec) {
    changeOrModify();
    if (data->recur )
        (*data->recur) = rec;
    else
        data->recur = new ORecur( rec );
}
bool OEvent::hasRecurrence()const {
    if (!data->recur ) return false;
    return data->recur->doesRecur();
}
QString OEvent::note()const {
    return data->note;
}
void OEvent::setNote( const QString& note ) {
    changeOrModify();
    data->note = note;
}
QDateTime OEvent::createdDateTime()const {
    return data->created;
}
void OEvent::setCreatedDateTime( const QDateTime& time ) {
    changeOrModify();
    data->created = time;
}
QDateTime OEvent::startDateTime()const {
    if ( data->isAllDay )
        return QDateTime( data->start.date(), QTime(0, 0, 0 ) );
    return data->start;
}
QDateTime OEvent::startDateTimeInZone()const {
    /* if no timezone, or all day event or if the current and this timeZone match... */
    if (data->timezone.isEmpty() || data->isAllDay || data->timezone == OTimeZone::current().timeZone() ) return startDateTime();

    OTimeZone zone(data->timezone  );
    return zone.toDateTime( data->start, OTimeZone::current() );
}
void OEvent::setStartDateTime( const QDateTime& dt ) {
    changeOrModify();
    data->start = dt;
}
QDateTime OEvent::endDateTime()const {
    /*
     * if all Day event the end time needs
     * to be on the same day as the start
     */
    if ( data->isAllDay )
        return QDateTime( data->start.date(), QTime(23, 59, 59 ) );
    return data->end;
}
QDateTime OEvent::endDateTimeInZone()const {
    /* if no timezone, or all day event or if the current and this timeZone match... */
    if (data->timezone.isEmpty() || data->isAllDay || data->timezone == OTimeZone::current().timeZone() ) return endDateTime();

    OTimeZone zone(data->timezone  );
    return zone.toDateTime( data->end, OTimeZone::current() );
}
void OEvent::setEndDateTime( const QDateTime& dt ) {
    changeOrModify();
    data->end =    dt;
}
bool OEvent::isMultipleDay()const {
    return data->end.date().day() - data->start.date().day();
}
bool OEvent::isAllDay()const {
    return data->isAllDay;
}
void OEvent::setAllDay( bool allDay ) {
    changeOrModify();
    data->isAllDay = allDay;
    if (allDay ) data->timezone = "UTC";
}
void OEvent::setTimeZone( const QString& tz ) {
    changeOrModify();
    data->timezone = tz;
}
QString OEvent::timeZone()const {
    if (data->isAllDay ) return QString::fromLatin1("UTC");
    return data->timezone;
}
bool OEvent::match( const QRegExp& re )const {
    if ( re.match( data->description ) != -1 ){
        setLastHitField( Qtopia::DatebookDescription );
        return true;
    }
    if ( re.match( data->note ) != -1 ){
        setLastHitField( Qtopia::Note );
        return true;
    }
    if ( re.match( data->location ) != -1 ){
        setLastHitField( Qtopia::Location );
        return true;
    }
    if ( re.match( data->start.toString() ) != -1 ){
        setLastHitField( Qtopia::StartDateTime );
        return true;
    }
    if ( re.match( data->end.toString() ) != -1 ){
        setLastHitField( Qtopia::EndDateTime );
        return true;
    }
    return false;
}
QString OEvent::toRichText()const {
    QString text, value;

    // description
    text += "<b><h3><img src=\"datebook/DateBook\">";
    if ( !description().isEmpty() ) {
        text += Qtopia::escapeString(description() ).replace(QRegExp( "[\n]"),  "" );
    }
    text += "</h3></b><br><hr><br>";

    // location
    if ( !(value = location()).isEmpty() ) {
        text += "<b>" + QObject::tr( "Location:" ) + "</b> ";
        text += Qtopia::escapeString(value) + "<br>";
    }

    // all day event
    if ( isAllDay() ) {
        text += "<b><i>" + QObject::tr( "This is an all day event" ) + "</i></b><br>";
    }
    // multiple day event
    else if ( isMultipleDay () ) {
        text += "<b><i>" + QObject::tr( "This is a multiple day event" ) + "</i></b><br>";
    }
    // start & end times
    else {
        // start time
        if ( startDateTime().isValid() ) {
            text += "<b>" + QObject::tr( "Start:") + "</b> ";
            text += Qtopia::escapeString(startDateTime().toString() ).
                    replace(QRegExp( "[\n]"),  "<br>" ) + "<br>";
        }

        // end time
        if ( endDateTime().isValid() ) {
            text += "<b>" + QObject::tr( "End:") + "</b> ";
            text += Qtopia::escapeString(endDateTime().toString() ).
                    replace(QRegExp( "[\n]"),  "<br>" ) + "<br>";
        }
    }

    // categories
    if ( categoryNames("Calendar").count() ){
	    text += "<b>" + QObject::tr( "Category:") + "</b> ";
	    text += categoryNames("Calendar").join(", ");
	    text += "<br>";
    }

    //notes
    if ( !note().isEmpty() ) {
        text += "<b>" + QObject::tr( "Note:") + "</b><br>";
        text += note();
//         text += Qtopia::escapeString(note() ).
//                 replace(QRegExp( "[\n]"),  "<br>" ) + "<br>";
    }
    return text;
}
QString OEvent::toShortText()const {
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
QString OEvent::type()const {
    return QString::fromLatin1("OEvent");
}
QString OEvent::recordField( int /*id */ )const {
    return QString::null;
}
int OEvent::rtti() {
    return OPimResolver::DateBook;
}
bool OEvent::loadFromStream( QDataStream& ) {
    return true;
}
bool OEvent::saveToStream( QDataStream& )const {
    return true;
}
void OEvent::changeOrModify() {
    if ( data->count != 1 ) {
        data->deref();
        Data* d2 = new Data;
        d2->description = data->description;
        d2->location = data->location;

        if (data->manager )
            d2->manager = new OPimNotifyManager( *data->manager );

        if ( data->recur )
            d2->recur = new ORecur( *data->recur );

        d2->note = data->note;
        d2->created = data->created;
        d2->start = data->start;
        d2->end = data->end;
        d2->isAllDay = data->isAllDay;
        d2->timezone = data->timezone;
        d2->parent = data->parent;

        if ( data->child ) {
            d2->child = new QArray<int>( *data->child );
            d2->child->detach();
        }

        data = d2;
    }
}
void OEvent::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0;
    }
}
// FIXME
QMap<int, QString> OEvent::toMap()const {
    return QMap<int, QString>();
}
int OEvent::parent()const {
    return data->parent;
}
void OEvent::setParent( int uid ) {
    changeOrModify();
    data->parent = uid;
}
QArray<int> OEvent::children() const{
    if (!data->child) return QArray<int>();
    else
        return data->child->copy();
}
void OEvent::setChildren( const QArray<int>& arr ) {
    changeOrModify();
    if (data->child) delete data->child;

    data->child = new QArray<int>( arr );
    data->child->detach();
}
void OEvent::addChild( int uid ) {
    changeOrModify();
    if (!data->child ) {
        data->child = new QArray<int>(1);
        (*data->child)[0] = uid;
    }else{
        int count = data->child->count();
        data->child->resize( count + 1 );
        (*data->child)[count] = uid;
    }
}
void OEvent::removeChild( int uid ) {
    if (!data->child || !data->child->contains( uid ) ) return;
    changeOrModify();
    QArray<int> newAr( data->child->count() - 1 );
    int j = 0;
    uint count = data->child->count();
    for ( uint i = 0; i < count; i++ ) {
        if ( (*data->child)[i] != uid ) {
            newAr[j] = (*data->child)[i];
            j++;
        }
    }
    (*data->child) = newAr;
}
struct OEffectiveEvent::Data : public QShared {
    Data() : QShared() {
    }
    OEvent event;
    QDate date;
    QTime start, end;
    QDate startDate, endDate;
    bool dates : 1;
};

OEffectiveEvent::OEffectiveEvent() {
    data = new Data;
    data->date = QDate::currentDate();
    data->start = data->end = QTime::currentTime();
    data->dates = false;
}
OEffectiveEvent::OEffectiveEvent( const OEvent& ev, const QDate& startDate,
                                  Position pos ) {
    data = new Data;
    data->event = ev;
    data->date = startDate;
    if ( pos & Start )
        data->start = ev.startDateTime().time();
    else
        data->start = QTime( 0, 0, 0 );

    if ( pos & End )
        data->end = ev.endDateTime().time();
    else
        data->end = QTime( 23, 59, 59 );

    data->dates = false;
}
OEffectiveEvent::OEffectiveEvent( const OEffectiveEvent& ev) {
    data = ev.data;
    data->ref();
}
OEffectiveEvent::~OEffectiveEvent() {
    if ( data->deref() ) {
        delete data;
        data = 0;
    }
}
OEffectiveEvent& OEffectiveEvent::operator=( const OEffectiveEvent& ev ) {
    if ( *this == ev ) return  *this;

    ev.data->ref();
    deref();
    data = ev.data;

    return *this;
}

void OEffectiveEvent::setStartTime( const QTime& ti) {
    changeOrModify();
    data->start = ti;
}
void OEffectiveEvent::setEndTime( const QTime& en) {
    changeOrModify();
    data->end = en;
}
void OEffectiveEvent::setEvent( const OEvent& ev) {
    changeOrModify();
    data->event = ev;
}
void OEffectiveEvent::setDate( const QDate& da) {
    changeOrModify();
    data->date = da;
}
void OEffectiveEvent::setEffectiveDates( const QDate& from,
                                         const QDate& to ) {
    if (!from.isValid() ) {
        data->dates = false;
        return;
    }

    data->startDate = from;
    data->endDate = to;
}
QString OEffectiveEvent::description()const {
    return data->event.description();
}
QString OEffectiveEvent::location()const {
    return data->event.location();
}
QString OEffectiveEvent::note()const {
    return data->event.note();
}
OEvent OEffectiveEvent::event()const {
    return data->event;
}
QTime OEffectiveEvent::startTime()const {
    return data->start;
}
QTime OEffectiveEvent::endTime()const {
    return data->end;
}
QDate OEffectiveEvent::date()const {
    return data->date;
}
int OEffectiveEvent::length()const {
    return (data->end.hour() * 60 - data->start.hour() * 60)
        + QABS(data->start.minute() - data->end.minute() );
}
int OEffectiveEvent::size()const {
    return ( data->end.hour() - data->start.hour() ) * 3600
        + (data->end.minute() - data->start.minute() * 60
        + data->end.second() - data->start.second() );
}
QDate OEffectiveEvent::startDate()const {
    if ( data->dates )
        return data->startDate;
    else if ( data->event.hasRecurrence() )  // single day, since multi-day should have a d pointer
        return data->date;
    else
        return data->event.startDateTime().date();
}
QDate OEffectiveEvent::endDate()const {
    if ( data->dates )
        return data->endDate;
    else if ( data->event.hasRecurrence() )
        return data->date;
    else
        return data->event.endDateTime().date();
}
void OEffectiveEvent::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0;
    }
}
void OEffectiveEvent::changeOrModify() {
    if ( data->count != 1 ) {
        data->deref();
        Data* d2 = new Data;
        d2->event = data->event;
        d2->date = data->date;
        d2->start = data->start;
        d2->end = data->end;
        d2->startDate = data->startDate;
        d2->endDate = data->endDate;
        d2->dates = data->dates;
        data = d2;
    }
}
bool OEffectiveEvent::operator<( const OEffectiveEvent &e ) const{
    if ( data->date < e.date() )
	return TRUE;
    if ( data->date == e.date() )
	return ( startTime() < e.startTime() );
    else
	return FALSE;
}
bool OEffectiveEvent::operator<=( const OEffectiveEvent &e ) const{
    return (data->date <= e.date() );
}
bool OEffectiveEvent::operator==( const OEffectiveEvent &e ) const {
    return ( date() == e.date()
	     && startTime() == e.startTime()
	     && endTime()== e.endTime()
	     && event() == e.event() );
}
bool OEffectiveEvent::operator!=( const OEffectiveEvent &e ) const {
    return !(*this == e );
}
bool OEffectiveEvent::operator>( const OEffectiveEvent &e ) const {
    return !(*this <= e );
}
bool OEffectiveEvent::operator>= ( const OEffectiveEvent &e ) const {
    return !(*this < e);
}
