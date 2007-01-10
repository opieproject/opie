/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
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

/* OPIE */
#include <opie2/opimdateconversion.h>
#include <opie2/opimstate.h>
#include <opie2/opimtimezone.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/opimrecurrence.h>
#include <opie2/otodoaccessxml.h>
#include <opie2/otodoaccess.h>
#include <opie2/odebug.h>

#include <opie2/private/opimtodosortvector.h>

#include <qpe/global.h>
#include <qpe/stringutil.h>
#include <qpe/timeconversion.h>

/* QT */
#include <qfile.h>
#include <qvector.h>

/* STD */
#include <errno.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>


using namespace Opie;

namespace {
    time_t rp_end;
    OPimRecurrence* rec;
    OPimRecurrence *recur() {
        if (!rec ) rec = new OPimRecurrence;
        return rec;
    }
    int snd;
    enum MoreAttributes {
        FRType = OPimTodo::CompletedDate + 2,
        FRWeekdays,
        FRPosition,
        FRFreq,
        FRHasEndDate,
        FREndDate,
        FRStart,
        FREnd
    };
    // FROM TT again
char *strstrlen(const char *haystack, int hLen, const char* needle, int nLen)
{
    char needleChar;
    char haystackChar;
    if (!needle || !haystack || !hLen || !nLen)
    return 0;

    const char* hsearch = haystack;

    if ((needleChar = *needle++) != 0) {
    nLen--; //(to make up for needle++)
    do {
        do {
        if ((haystackChar = *hsearch++) == 0)
            return (0);
        if (hsearch >= haystack + hLen)
            return (0);
        } while (haystackChar != needleChar);
    } while (strncmp(hsearch, needle, QMIN(hLen - (hsearch - haystack), nLen)) != 0);
    hsearch--;
    }
    return ((char *)hsearch);
}
}

namespace Opie {

OPimTodoAccessXML::OPimTodoAccessXML( const QString& appName,
                                const QString& fileName )
    : OPimTodoAccessBackend(), m_app( appName ),  m_opened( false ), m_changed( false )
{
    if (!fileName.isEmpty() )
        m_file = fileName;
    else
        m_file = Global::applicationFileName( "todolist", "todolist.xml" );
}
OPimTodoAccessXML::~OPimTodoAccessXML() {

}
bool OPimTodoAccessXML::load() {
    rec = 0;
    m_opened = true;
    m_changed = false;
    /* initialize dict */
    /*
     * UPDATE dict if you change anything!!!
     */
    QAsciiDict<int> dict(26);
    dict.setAutoDelete( TRUE );
    dict.insert("Categories" ,     new int(OPimTodo::Category)         );
    dict.insert("Uid" ,            new int(OPimTodo::Uid)              );
    dict.insert("HasDate" ,        new int(OPimTodo::HasDate)          );
    dict.insert("Completed" ,      new int(OPimTodo::Completed)        );
    dict.insert("Description" ,    new int(OPimTodo::Description)      );
    dict.insert("Summary" ,        new int(OPimTodo::Summary)          );
    dict.insert("Priority" ,       new int(OPimTodo::Priority)         );
    dict.insert("DateDay" ,        new int(OPimTodo::DateDay)          );
    dict.insert("DateMonth" ,      new int(OPimTodo::DateMonth)        );
    dict.insert("DateYear" ,       new int(OPimTodo::DateYear)         );
    dict.insert("Progress" ,       new int(OPimTodo::Progress)         );
    dict.insert("CompletedDate",   new int(OPimTodo::CompletedDate)    );
    dict.insert("StartDate",       new int(OPimTodo::StartDate)        );
    dict.insert("CrossReference",  new int(OPimTodo::CrossReference)   );
    dict.insert("State",           new int(OPimTodo::State)            );
    dict.insert("Alarms",          new int(OPimTodo::Alarms)           );
    dict.insert("Reminders",       new int(OPimTodo::Reminders)        );
    dict.insert("Maintainer",      new int(OPimTodo::Maintainer)       );
    dict.insert("rtype",           new int(FRType)                  );
    dict.insert("rweekdays",       new int(FRWeekdays)              );
    dict.insert("rposition",       new int(FRPosition)              );
    dict.insert("rfreq",           new int(FRFreq)                  );
    dict.insert("start",           new int(FRStart)                 );
    dict.insert("rhasenddate",     new int(FRHasEndDate)            );
    dict.insert("enddt",           new int(FREndDate)               );

    // here the custom XML parser from TT it's GPL
    // but we want to push OpiePIM... to TT.....
    // mmap part from zecke :)
    int fd = ::open( QFile::encodeName(m_file).data(), O_RDONLY );
    struct stat attribut;
    if ( fd < 0 ) return false;

    if ( fstat(fd, &attribut ) == -1 ) {
        ::close( fd );
        return false;
    }
    void* map_addr = ::mmap(NULL,  attribut.st_size, PROT_READ, MAP_SHARED, fd, 0 );
    if ( map_addr == ( (caddr_t)-1) ) {
        ::close(fd );
        return false;
    }
    /* advise the kernel who we want to read it */
    ::madvise( map_addr,  attribut.st_size,  MADV_SEQUENTIAL );
    /* we do not the file any more */
    ::close( fd );

    char* dt = (char*)map_addr;
    int len = attribut.st_size;
    int i = 0;
    char *point;
    const char* collectionString = "<Task ";
    int strLen = strlen(collectionString);
    while ( ( point = strstrlen( dt+i, len -i, collectionString, strLen ) ) != 0l ) {
        i = point -dt;
        i+= strLen;

        OPimTodo ev;
        m_year = m_month = m_day = 0;

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
            todo( &dict, ev, attr, str );

        }
        /*
         * now add it
         */
        if (m_events.contains( ev.uid() ) || ev.uid() == 0) {
            ev.setUid( 1 );
            m_changed = true;
        }
        if ( ev.hasDueDate() ) {
            ev.setDueDate( QDate(m_year, m_month, m_day) );
        }
        if ( rec && rec->doesRecur() ) {
            OPimTimeZone utc = OPimTimeZone::utc();
            OPimRecurrence recu( *rec ); // call copy c'tor
            recu.setEndDate( utc.fromUTCDateTime( rp_end ).date() );
            recu.setStart( ev.dueDate() );
            ev.setRecurrence( recu );
        }
        m_events.insert(ev.uid(), ev );
        m_year = m_month = m_day = -1;
        delete rec;
        rec = 0;
    }

    munmap(map_addr, attribut.st_size );

    return true;
}
bool OPimTodoAccessXML::reload() {
    m_events.clear();
    return load();
}
bool OPimTodoAccessXML::save() {
    if (!m_opened || !m_changed ) {
        return true;
    }
    QString strNewFile = m_file + ".new";
    QFile f( strNewFile );
    if (!f.open( IO_WriteOnly|IO_Raw ) )
        return false;

    int written;
    QString out;
    out = "<!DOCTYPE Tasks>\n<Tasks>\n";

    // for all todos
    QMap<int, OPimTodo>::Iterator it;
    for (it = m_events.begin(); it != m_events.end(); ++it ) {
        out+= "<Task " + toString( (*it) ) + " />\n";
        QCString cstr = out.utf8();
        written = f.writeBlock( cstr.data(),  cstr.length() );

        /* less written then we wanted */
        if ( written != (int)cstr.length() ) {
            f.close();
            QFile::remove( strNewFile );
            return false;
        }
        out = QString::null;
    }

    out +=  "</Tasks>";
    QCString cstr = out.utf8();
    written = f.writeBlock( cstr.data(), cstr.length() );

    if ( written != (int)cstr.length() ) {
        f.close();
        QFile::remove( strNewFile );
        return false;
    }
    /* flush before renaming */
    f.close();

    if( ::rename( strNewFile.latin1(),  m_file.latin1() ) < 0 ) {
        QFile::remove( strNewFile );
    }

    m_changed = false;
    return true;
}
QArray<int> OPimTodoAccessXML::allRecords()const {
    QArray<int> ids( m_events.count() );
    QMap<int, OPimTodo>::ConstIterator it;
    int i = 0;

    for ( it = m_events.begin(); it != m_events.end(); ++it )
        ids[i++] = it.key();


    return ids;
}

OPimTodo OPimTodoAccessXML::find( int uid )const {
    OPimTodo todo;
    todo.setUid( 0 ); // isEmpty()
    QMap<int, OPimTodo>::ConstIterator it = m_events.find( uid );
    if ( it != m_events.end() )
        todo = it.data();

    return todo;
}
void OPimTodoAccessXML::clear() {
    if (m_opened )
        m_changed = true;

    m_events.clear();
}
bool OPimTodoAccessXML::add( const OPimTodo& todo ) {
    m_changed = true;
    m_events.insert( todo.uid(), todo );

    return true;
}
bool OPimTodoAccessXML::remove( int uid ) {
    m_changed = true;
    m_events.remove( uid );

    return true;
}
bool OPimTodoAccessXML::replace( const OPimTodo& todo) {
    m_changed = true;
    m_events.replace( todo.uid(), todo );

    return true;
}
QArray<int> OPimTodoAccessXML::effectiveToDos( const QDate& start,
                                            const QDate& end,
                                            bool includeNoDates )const {
    QArray<int> ids( m_events.count() );
    QMap<int, OPimTodo>::ConstIterator it;

    int i = 0;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( !it.data().hasDueDate() && includeNoDates) {
                ids[i++] = it.key();
        }else if ( it.data().dueDate() >= start &&
                   it.data().dueDate() <= end ) {
            ids[i++] = it.key();
        }
    }
    ids.resize( i );
    return ids;
}
QArray<int> OPimTodoAccessXML::overDue()const {
    QArray<int> ids( m_events.count() );
    int i = 0;

    QMap<int, OPimTodo>::ConstIterator it;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().isOverdue() ) {
            ids[i] = it.key();
            i++;
        }
    }
    ids.resize( i );
    return ids;
}


/* private */
void OPimTodoAccessXML::todo( QAsciiDict<int>* dict, OPimTodo& ev,
                            const QCString& attr, const QString& val) {

    int *find=0;

    find = (*dict)[ attr.data() ];
    if (!find ) {
        ev.setCustomField( attr, val );
        return;
    }

    switch( *find ) {
    case OPimTodo::Uid:
        ev.setUid( val.toInt() );
        break;
    case OPimTodo::Category:
        ev.setCategories( ev.idsFromString( val ) );
        break;
    case OPimTodo::HasDate:
        ev.setHasDueDate( val.toInt() );
        break;
    case OPimTodo::Completed:
        ev.setCompleted( val.toInt() );
        break;
    case OPimTodo::Description:
        ev.setDescription( val );
        break;
    case OPimTodo::Summary:
        ev.setSummary( val );
        break;
    case OPimTodo::Priority:
        ev.setPriority( val.toInt() );
        break;
    case OPimTodo::DateDay:
        m_day = val.toInt();
        break;
    case OPimTodo::DateMonth:
        m_month = val.toInt();
        break;
    case OPimTodo::DateYear:
        m_year = val.toInt();
        break;
    case OPimTodo::Progress:
        ev.setProgress( val.toInt() );
        break;
    case OPimTodo::CompletedDate:
        ev.setCompletedDate( OPimDateConversion::dateFromString( val ) );
        break;
    case OPimTodo::StartDate:
        ev.setStartDate( OPimDateConversion::dateFromString( val ) );
        break;
    case OPimTodo::State:
        ev.setState( val.toInt() );
        break;
    case OPimTodo::Alarms:{
        OPimNotifyManager &manager = ev.notifiers();
        QStringList als = QStringList::split(";", val );
        for (QStringList::Iterator it = als.begin(); it != als.end(); ++it ) {
            QStringList alarm = QStringList::split(":", (*it), TRUE ); // allow empty
            OPimAlarm al( alarm[2].toInt(), OPimDateConversion::dateTimeFromString( alarm[0] ), alarm[1].toInt() );
            manager.add( al );
        }
    }
        break;
    case OPimTodo::Reminders:{
        OPimNotifyManager &manager = ev.notifiers();
        QStringList rems = QStringList::split(";", val );
        for (QStringList::Iterator it = rems.begin(); it != rems.end(); ++it ) {
            OPimReminder rem( (*it).toInt() );
            manager.add( rem );
        }
    }
        break;
    case OPimTodo::CrossReference:
    {
        /*
         * A cross refernce looks like
         * appname,id;appname,id
         * we need to split it up
         */
        QStringList  refs = QStringList::split(';', val );
        QStringList::Iterator strIt;
        for (strIt = refs.begin(); strIt != refs.end(); ++strIt ) {
            int pos = (*strIt).find(',');
            if ( pos > -1 )
                ; // ev.addRelation( (*strIt).left(pos),  (*strIt).mid(pos+1).toInt() );

        }
        break;
    }
    /* Recurrence stuff below + post processing later */
    case FRType:
        if ( val == "Daily" )
            recur()->setType( OPimRecurrence::Daily );
        else if ( val == "Weekly" )
            recur()->setType( OPimRecurrence::Weekly);
        else if ( val == "MonthlyDay" )
            recur()->setType( OPimRecurrence::MonthlyDay );
        else if ( val == "MonthlyDate" )
            recur()->setType( OPimRecurrence::MonthlyDate );
        else if ( val == "Yearly" )
            recur()->setType( OPimRecurrence::Yearly );
        else
            recur()->setType( OPimRecurrence::NoRepeat );
        break;
    case FRWeekdays:
        recur()->setDays( val.toInt() );
        break;
    case FRPosition:
        recur()->setPosition( val.toInt() );
        break;
    case FRFreq:
        recur()->setFrequency( val.toInt() );
        break;
    case FRHasEndDate:
        recur()->setHasEndDate( val.toInt() );
        break;
    case FREndDate: {
        rp_end = (time_t) val.toLong();
        break;
    }
    default:
        ev.setCustomField( attr, val );
        break;
    }
}

// from PalmtopRecord... GPL ### FIXME
namespace {
QString customToXml(const QMap<QString, QString>& customMap )
{
    QString buf(" ");
    for ( QMap<QString, QString>::ConstIterator cit = customMap.begin();
        cit != customMap.end(); ++cit) {
    buf += cit.key();
    buf += "=\"";
    buf += Qtopia::escapeString(cit.data());
    buf += "\" ";
    }
    return buf;
}


}

QString OPimTodoAccessXML::toString( const OPimTodo& ev )const {
    QString str;

    str += "Completed=\"" + QString::number( ev.isCompleted() ) + "\"";
    str += " HasDate=\"" + QString::number( ev.hasDueDate() ) + "\"";
    str += " Priority=\"" + QString::number( ev.priority() ) + "\"";
    str += " Progress=\"" + QString::number(ev.progress() ) + "\"";

    str += " Categories=\"" + toString( ev.categories() ) + "\"";
    str += " Description=\"" + Qtopia::escapeString( ev.description() ) + "\"";
    str += " Summary=\"" + Qtopia::escapeString( ev.summary() ) + "\"";

    if ( ev.hasDueDate() ) {
        str += " DateYear=\"" + QString::number( ev.dueDate().year() ) + "\"";
        str += " DateMonth=\"" + QString::number( ev.dueDate().month() ) + "\"";
        str += " DateDay=\"" + QString::number( ev.dueDate().day() ) + "\"";
    }
    str += " Uid=\"" + QString::number( ev.uid() ) + "\"";

// append the extra options
    /* FIXME Qtopia::Record this is currently not
     * possible you can set custom fields
     * but don' iterate over the list
     * I may do #define private protected
     * for this case - cough  --zecke
     */
    /*
    QMap<QString, QString> extras = ev.extras();
    QMap<QString, QString>::Iterator extIt;
    for (extIt = extras.begin(); extIt != extras.end(); ++extIt )
        str += " " + extIt.key() + "=\"" +  extIt.data() + "\"";
    */
    // cross refernce
    if ( ev.hasRecurrence() ) {
        str += ev.recurrence().toString();
    }
    if ( ev.hasStartDate() )
        str += " StartDate=\""+ OPimDateConversion::dateToString( ev.startDate() ) +"\"";
    if ( ev.hasCompletedDate() )
        str += " CompletedDate=\""+ OPimDateConversion::dateToString( ev.completedDate() ) +"\"";
    if ( ev.hasState() )
        str += " State=\""+QString::number( ev.state().state() )+"\"";

    /*
     * save reminders and notifiers!
     * DATE_TIME:DURATION:SOUND:NOT_USED_YET;OTHER_DATE_TIME:OTHER_DURATION:SOUND:....
     */
    if ( ev.hasNotifiers() ) {
        OPimNotifyManager manager = ev.notifiers();
        OPimNotifyManager::Alarms alarms = manager.alarms();
        if (!alarms.isEmpty() ) {
            QStringList als;
            OPimNotifyManager::Alarms::Iterator it = alarms.begin();
            for ( ; it != alarms.end(); ++it ) {
                /* only if time is valid */
                if ( (*it).dateTime().isValid() ) {
                    als << OPimDateConversion::dateTimeToString( (*it).dateTime() )
                        + ":" + QString::number( (*it).duration() )
                        + ":" + QString::number( (*it).sound() )
                        + ":";
                }
            }
            // now write the list
            str += " Alarms=\""+als.join(";") +"\"";
        }

        /*
         * now the same for reminders but more easy. We just save the uid of the OPimEvent.
         */
        OPimNotifyManager::Reminders reminders = manager.reminders();
        if (!reminders.isEmpty() ) {
            OPimNotifyManager::Reminders::Iterator it = reminders.begin();
            QStringList records;
            for ( ; it != reminders.end(); ++it ) {
                records << QString::number( (*it).recordUid() );
            }
            str += " Reminders=\""+ records.join(";") +"\"";
        }
    }
    str += customToXml( ev.toExtraMap() );


    return str;
}
QString OPimTodoAccessXML::toString( const QArray<int>& ints ) const {
    return Qtopia::Record::idsToString( ints );
}


QArray<int> OPimTodoAccessXML::sorted( const UIDArray& events, bool asc,
                                       int sortOrder,int sortFilter,
                                       const QArray<int>& categories )const {
    Internal::OPimTodoSortVector vector(events.count(), asc,sortOrder );
    int item = 0;

    bool bCat = sortFilter  & OPimTodoAccess::FilterCategory ? true : false;
    bool bOnly = sortFilter & OPimTodoAccess::OnlyOverDue ? true : false;
    bool comp = sortFilter  & OPimTodoAccess::DoNotShowCompleted ? true : false;
    bool catPassed = false;
    int cat;

    for ( uint i = 0; i < events.count(); ++i ) {
        /* Guard against creating a new item... */
        if ( !m_events.contains( events[i] ) )
            continue;

        OPimTodo todo = m_events[events[i]];

        /* show category */
        /* -1 == unfiled */
        catPassed = false;
        for ( uint cat_nu = 0; cat_nu < categories.count(); ++cat_nu ) {
            cat = categories[cat_nu];
            if ( bCat && cat == -1 ) {
                if(!todo.categories().isEmpty() )
                    continue;
            } else if ( bCat && cat != 0)
                if (!todo.categories().contains( cat ) )
                    continue;
            catPassed = true;
            break;
        }

        /*
         * If none of the Categories matched
         * continue
         */
        if ( !catPassed )
            continue;
        if ( !todo.isOverdue() && bOnly )
            continue;
        if (todo.isCompleted() && comp )
            continue;

        vector.insert(item++, todo );
    }

    vector.resize( item );
    /* sort it now */
    vector.sort();
    /* now get the uids */
    UIDArray array( vector.count() );
    for (uint i= 0; i < vector.count(); i++ )
        array[i] = vector.uidAt( i );

    return array;
}

void OPimTodoAccessXML::removeAllCompleted() {
    QMap<int, OPimTodo> events = m_events;
    for ( QMap<int, OPimTodo>::Iterator it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( (*it).isCompleted() )
            events.remove( it.key() );
    }
    m_events = events;
}

QArray<int> OPimTodoAccessXML::matchRegexp(  const QRegExp &r ) const
{
    QArray<int> currentQuery( m_events.count() );
    uint arraycounter = 0;

    QMap<int, OPimTodo>::ConstIterator it;
    for (it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().match( r ) )
            currentQuery[arraycounter++] = it.data().uid();

    }
    // Shrink to fit..
    currentQuery.resize(arraycounter);

    return currentQuery;
}

}
