#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <unistd.h>

#include <qasciidict.h>
#include <qfile.h>

#include <qtopia/global.h>
#include <qtopia/stringutil.h>

#include "opimnotifymanager.h"
#include "orecur.h"
#include "otimezone.h"
#include "odatebookaccessbackend_xml.h"

namespace {
    time_t start, end, created, rp_end;
    ORecur* rec;
    ORecur* recur() {
        if (!rec)
            rec = new ORecur;

        return rec;
    }
    int alarmTime;
    int snd;
    enum Attribute{
        	FDescription = 0,
	FLocation,
	FCategories,
	FUid,
	FType,
	FAlarm,
	FSound,
	FRType,
	FRWeekdays,
	FRPosition,
	FRFreq,
	FRHasEndDate,
	FREndDate,
	FRStart,
	FREnd,
	FNote,
	FCreated
    };
}

ODateBookAccessBackend_XML::ODateBookAccessBackend_XML( const QString& ,
                                                        const QString& fileName )
    : ODateBookAccessBackend() {
    m_name = fileName.isEmpty() ? Global::applicationFileName( "datebook", "datebook.xml" ) : fileName;
    m_changed = false;
}
ODateBookAccessBackend_XML::~ODateBookAccessBackend_XML() {
}
bool ODateBookAccessBackend_XML::load() {
    return loadFile();
}
bool ODateBookAccessBackend_XML::reload() {
    clear();
    return load();
}
bool ODateBookAccessBackend_XML::save() {
    if (!m_changed) return true;
    m_changed = false;

    return true;
}
QArray<int> ODateBookAccessBackend_XML::allRecords()const {
    QArray<int> ints( m_raw.count()+ m_rep.count() );
    uint i = 0;
    QMap<int, OEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }
    for ( it = m_rep.begin(); it != m_rep.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}
QArray<int> ODateBookAccessBackend_XML::queryByExample(const OEvent&, int ) {
    return QArray<int>();
}
void ODateBookAccessBackend_XML::clear() {
    m_raw.clear();
    m_rep.clear();
}
OEvent ODateBookAccessBackend_XML::find( int uid ) const{
    if ( m_raw.contains( uid ) )
        return m_raw[uid];
    else
        return m_rep[uid];
}
bool ODateBookAccessBackend_XML::add( const OEvent& ev ) {
    m_changed = true;
    if (ev.hasRecurrence() )
        m_rep.insert( ev.uid(), ev );
    else
        m_raw.insert( ev.uid(), ev );

    return true;
}
bool ODateBookAccessBackend_XML::remove( int uid ) {
    m_changed = true;
    m_rep.remove( uid );
    m_rep.remove( uid );

    return true;
}
bool ODateBookAccessBackend_XML::replace( const OEvent& ev ) {
    replace( ev.uid() );
    return add( ev );
}
QArray<int> ODateBookAccessBackend_XML::rawEvents()const {
    return allRecords();
}
QArray<int> ODateBookAccessBackend_XML::rawRepeats()const {
    QArray<int> ints( m_rep.count() );
    uint i = 0;
    QMap<int, OEvent>::ConstIterator it;

    for ( it = m_rep.begin(); it != m_rep.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}
QArray<int> ODateBookAccessBackend_XML::nonRepeats()const {
    QArray<int> ints( m_raw.count() );
    uint i = 0;
    QMap<int, OEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}
OEvent::ValueList ODateBookAccessBackend_XML::directNonRepeats() {
    OEvent::ValueList list;
    QMap<int, OEvent>::ConstIterator it;
    for (it = m_raw.begin(); it != m_raw.end(); ++it )
        list.append( it.data() );

    return list;
}
OEvent::ValueList ODateBookAccessBackend_XML::directRawRepeats() {
    OEvent::ValueList list;
    QMap<int, OEvent>::ConstIterator it;
    for (it = m_rep.begin(); it != m_rep.end(); ++it )
        list.append( it.data() );

    return list;
}
bool ODateBookAccessBackend_XML::loadFile() {
    m_changed = false;

    int fd = ::open( QFile::encodeName(m_name).data(), O_RDONLY );
    if ( fd < 0 ) return false;

    struct stat attribute;
    if ( ::fstat(fd, &attribute ) == -1 ) {
        ::close( fd );
        return false;
    }
    void* map_addr = ::mmap(NULL, attribute.st_size, PROT_READ, MAP_SHARED, fd, 0 );
    if ( map_addr == ( (caddr_t)-1) ) {
        ::close( fd );
        return false;
    }

    ::madvise( map_addr, attribute.st_size, MADV_SEQUENTIAL );
    ::close( fd );

    QAsciiDict<int> dict(FCreated+1);
    dict.setAutoDelete( true );
    dict.insert( "description", new int(FDescription) );
    dict.insert( "location", new int(FLocation) );
    dict.insert( "categories", new int(FCategories) );
    dict.insert( "uid", new int(FUid) );
    dict.insert( "type", new int(FType) );
    dict.insert( "alarm", new int(FAlarm) );
    dict.insert( "sound", new int(FSound) );
    dict.insert( "rtype", new int(FRType) );
    dict.insert( "rweekdays", new int(FRWeekdays) );
    dict.insert( "rposition", new int(FRPosition) );
    dict.insert( "rfreq", new int(FRFreq) );
    dict.insert( "rhasenddate", new int(FRHasEndDate) );
    dict.insert( "enddt", new int(FREndDate) );
    dict.insert( "start", new int(FRStart) );
    dict.insert( "end", new int(FREnd) );
    dict.insert( "note", new int(FNote) );
    dict.insert( "created", new int(FCreated) );

    char* dt = (char*)map_addr;
    int len = attribute.st_size;
    int i = 0;
    char* point;
    const char* collectionString = "<event ";
    int strLen = ::strlen(collectionString);
    int *find;
    while ( dt + 1 != 0 && (( point = ::strstr( dt+i, collectionString ) ) != 0 ) ) {
        i = point -dt;
        i+= strLen;

        alarmTime = -1;
        snd = 0; // silent

        OEvent ev;
        rec = 0;

        while ( TRUE ) {
            while ( i < len && (dt[i] == ' ' || dt[i] == '\n' || dt[i] == '\r') )
		++i;
	    if ( i >= len-2 || (dt[i] == '/' && dt[i+1] == '>') )
		break;


	    // we have another attribute, read it.
	    int j = i;
	    while ( j < len && dt[j] != '=' )
		++j;
	    QCString attr( dt+i, j-i+1);

	    i = ++j; // skip =

	    // find the start of quotes
	    while ( i < len && dt[i] != '"' )
		++i;
	    j = ++i;

	    bool haveUtf = FALSE;
	    bool haveEnt = FALSE;
	    while ( j < len && dt[j] != '"' ) {
		if ( ((unsigned char)dt[j]) > 0x7f )
		    haveUtf = TRUE;
		if ( dt[j] == '&' )
		    haveEnt = TRUE;
		++j;
	    }
	    if ( i == j ) {
		// empty value
		i = j + 1;
		continue;
	    }

	    QCString value( dt+i, j-i+1 );
	    i = j + 1;

	    QString str = (haveUtf ? QString::fromUtf8( value )
		    : QString::fromLatin1( value ) );
	    if ( haveEnt )
		str = Qtopia::plainString( str );

            /*
             * add key + value
             */
            find = dict[attr.data()];
            if (!find)
                ev.setCustomField( attr, value );
            else {
                setField( ev, *find, value );
            }
        }
        /* time to finalize */
        finalizeRecord( ev );
        add( ev );
        delete rec;
    }
    ::munmap(map_addr, attribute.st_size );
    m_changed = false; // changed during add

    return true;
}
void ODateBookAccessBackend_XML::finalizeRecord( OEvent& ev ) {
    /* AllDay is alway in UTC */
    if ( ev.isAllDay() ) {
        OTimeZone utc = OTimeZone::utc();
        ev.setStartDateTime( utc.fromUTCDateTime( start ) );
        ev.setEndDateTime  ( utc.fromUTCDateTime( end   ) );
    }else {
        OTimeZone zone( ev.timeZone().isEmpty() ? OTimeZone::current() : ev.timeZone() );
        ev.setStartDateTime( zone.toDateTime( start ) );
        ev.setEndDateTime  ( zone.toDateTime( end   ) );
    }
    if ( rec && rec->doesRecur() ) {
        OTimeZone utc = OTimeZone::utc();
        ORecur recu( *rec ); // call copy c'tor;
        recu.setEndDate ( utc.fromUTCDateTime( rp_end ).date() );
        recu.setCreatedDateTime( utc.fromUTCDateTime( created ) );
        recu.setStart( ev.startDateTime().date() );
        ev.setRecurrence( recu );
    }

    if (alarmTime != -1 ) {
        QDateTime dt = ev.startDateTime().addSecs( -1*alarmTime*60 );
        OPimAlarm al( snd ,  dt  );
        ev.notifiers().add( al );
    }
    if ( m_raw.contains( ev.uid() ) || m_rep.contains( ev.uid() ) ) {
        ev.setUid( 1 );
    }
    if ( ev.hasRecurrence() )
        m_rep.insert( ev.uid(), ev );
    else
        m_raw.insert( ev.uid(), ev );

}
void ODateBookAccessBackend_XML::setField( OEvent& e, int id, const QString& value) {
//    qWarning(" setting %s", value.latin1() );
    switch( id ) {
    case FDescription:
        e.setDescription( value );
        break;
    case FLocation:
        e.setLocation( value );
        break;
    case FCategories:
        e.setCategories( e.idsFromString( value ) );
        break;
    case FUid:
        e.setUid( value.toInt() );
        break;
    case FType:
        if ( value == "AllDay" ) {
            e.setAllDay( true );
            e.setTimeZone( "UTC" );
        }
        break;
    case FAlarm:
        alarmTime = value.toInt();
        break;
    case FSound:
        snd = value == "loud" ? OPimAlarm::Loud : OPimAlarm::Silent;
        break;
        // recurrence stuff
    case FRType:
        if ( value == "Daily" )
            recur()->setType( ORecur::Daily );
        else if ( value == "Weekly" )
            recur()->setType( ORecur::Weekly);
        else if ( value == "MonthlyDay" )
            recur()->setType( ORecur::MonthlyDay );
        else if ( value == "MonthlyDate" )
            recur()->setType( ORecur::MonthlyDate );
        else if ( value == "Yearly" )
            recur()->setType( ORecur::Yearly );
        else
            recur()->setType( ORecur::NoRepeat );
        break;
    case FRWeekdays:
        recur()->setDays( value.toInt() );
        break;
    case FRPosition:
        recur()->setPosition( value.toInt() );
        break;
    case FRFreq:
        recur()->setFrequency( value.toInt() );
        break;
    case FRHasEndDate:
        recur()->setHasEndDate( value.toInt() );
        break;
    case FREndDate: {
        rp_end = (time_t) value.toLong();
        break;
    }
    case FRStart: {
        start =  (time_t) value.toLong();
        break;
    }
    case FREnd: {
        end = ( (time_t) value.toLong() );
        break;
    }
    case FNote:
        e.setNote( value );
        break;
    case FCreated:
        created = value.toInt();
        break;
    default:
        break;
    }
}
