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
#include <qintdict.h>


using namespace Opie;

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

void OPimTodoAccessXML::initDict( QAsciiDict<int> &dict ) const
{
    /*
     * UPDATE this if you change anything!!!
     */
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
    dict.insert("rtype",           new int(OPimTodo::FRType)           );
    dict.insert("rweekdays",       new int(OPimTodo::FRWeekdays)       );
    dict.insert("rposition",       new int(OPimTodo::FRPosition)       );
    dict.insert("rfreq",           new int(OPimTodo::FRFreq)           );
    dict.insert("start",           new int(OPimTodo::FRStart)          );
    dict.insert("rhasenddate",     new int(OPimTodo::FRHasEndDate)     );
    dict.insert("enddt",           new int(OPimTodo::FREndDate)        );
}

bool OPimTodoAccessXML::load() {
    m_opened = true;
    m_changed = false;

    // initialize dict
    QAsciiDict<int> dict(26);
    initDict(dict);

    // parse
    OPimTodoXmlHandler handler( dict, *this );
    OPimXmlMmapParser parser( handler );
    return parser.parse( m_file );
}

inline void OPimTodoAccessXML::finalizeRecord( OPimTodo& ev )
{
    if (m_events.contains( ev.uid() ) || ev.uid() == 0) {
        ev.setUid( 1 );
        m_changed = true;
    }

    m_events.insert(ev.uid(), ev );
}

bool OPimTodoAccessXML::reload() {
    m_events.clear();
    return load();
}

bool OPimTodoAccessXML::read( OPimXmlReader &rd )
{
    QAsciiDict<int> dict(26);
    initDict( dict );

    OPimTodoXmlHandler handler( dict, *this );
    OPimXmlStreamParser parser( handler );
    rd.read( parser );
    return true;
}

bool OPimTodoAccessXML::write( OAbstractWriter &wr )
{
    QString out;
    out = "<!DOCTYPE Tasks>\n<Tasks>\n";
    QCString cstr = out.utf8();
    if ( !wr.writeString( cstr ) )
        return false;

    QAsciiDict<int> dict(26);
    initDict( dict );
    QIntDict<QString> revdict( dict.size() );
    revdict.setAutoDelete( true );
    // Now we need to reverse the dictionary (!)
    for( QAsciiDictIterator<int> it( dict ); it.current(); ++it ) {
        revdict.insert( (*it), new QString( it.currentKey() ) );
    }

    // for all todos
    QMap<int, OPimTodo>::Iterator it;
    for (it = m_events.begin(); it != m_events.end(); ++it ) {
        out = "<Task " + toString( (*it), revdict ) + " />\n";
        cstr = out.utf8();
        if ( !wr.writeString( cstr ) )
            return false;
    }

    out =  "</Tasks>";
    cstr = out.utf8();
    if ( !wr.writeString( cstr ) )
        return false;

    return true;
}

bool OPimTodoAccessXML::save() {
    if (!m_opened || !m_changed ) {
        return true;
    }
    QString strNewFile = m_file + ".new";
    QFile f( strNewFile );
    if (!f.open( IO_WriteOnly|IO_Raw ) )
        return false;

    OFileWriter fw( &f );
    if( !write( fw ) ) {
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
        }
        else if ( it.data().dueDate() >= start &&
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

QString OPimTodoAccessXML::toString( const OPimTodo& ev, const QIntDict<QString> &revdict )const {
    QString str;

    QMap<int, QString> map = ev.toMap();

    for ( QMap<int, QString>::ConstIterator it = map.begin(); it != map.end(); ++it ) {
        const QString &value = it.data();
        int key = it.key();
        if ( !value.isEmpty() ) {
            str += " " + *revdict[ key ];
            str += "=\"" + Qtopia::escapeString( value ) + "\"";
        }
    }
    if( str[0] == ' ' )
        str = str.mid(1);

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
            } 
            else if ( bCat && cat != 0)
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


////////////////////////////////////////////////////////////////////////////

OPimTodoXmlHandler::OPimTodoXmlHandler( QAsciiDict<int> &dict, OPimTodoAccessXML &backend )
    : OPimXmlHandler( "Task", dict ), m_backend( backend )
{
}

void OPimTodoXmlHandler::handleItem( QMap<int, QString> &map, QMap<QString, QString> &extramap )
{
    OPimTodo todo;
    todo.fromMap( map );
    for( QMap<QString, QString>::Iterator it = extramap.begin(); it != extramap.end(); ++it )
        todo.setCustomField(it.key(), it.data() );
    
    m_backend.finalizeRecord( todo );
}

}
