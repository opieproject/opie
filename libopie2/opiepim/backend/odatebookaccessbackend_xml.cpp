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
#include <opie2/opimnotifymanager.h>
#include <opie2/opimrecurrence.h>
#include <opie2/opimtimezone.h>
#include <opie2/odatebookaccessbackend_xml.h>
#include <opie2/odebug.h>

#include <qtopia/global.h>
#include <qtopia/stringutil.h>
#include <qtopia/timeconversion.h>

/* QT */
#include <qasciidict.h>
#include <qfile.h>
#include <qintdict.h>

/* STD */
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <unistd.h>


using namespace Opie;

namespace {
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

namespace {

    static void save( const OPimEvent& ev, const QIntDict<QString> &revdict, QString& buf ) {

        QMap<int, QString> map = ev.toMap();

        for ( QMap<int, QString>::ConstIterator it = map.begin(); it != map.end(); ++it ) {
            const QString &value = it.data();
            int key = it.key();
            if ( !value.isEmpty() ) {
                buf += " " + *revdict[ key ];
                buf += "=\"" + Qtopia::escapeString( value ) + "\"";
            }
        }

        // skip custom writing
    }

    static bool saveEachEvent( const QMap<int, OPimEvent>& list, QIntDict<QString> &revdict, QFile& file ) {
        QMap<int, OPimEvent>::ConstIterator it;
        QString buf;
        QCString str;
        int total_written;
        for ( it = list.begin(); it != list.end(); ++it ) {
            buf = "<event";
            save( it.data(), revdict, buf );
            buf += " />\n";
            str = buf.utf8();

            total_written = file.writeBlock(str.data(), str.length() );
            if ( total_written != int(str.length() ) )
                return false;
        }
        return true;
    }
}

namespace Opie {
ODateBookAccessBackend_XML::ODateBookAccessBackend_XML( const QString& ,
                                                        const QString& fileName )
    : ODateBookAccessBackend() {
    m_name = fileName.isEmpty() ? Global::applicationFileName( "datebook", "datebook.xml" ) : fileName;
    m_changed = false;
}

ODateBookAccessBackend_XML::~ODateBookAccessBackend_XML() {
}

void ODateBookAccessBackend_XML::initDict( QAsciiDict<int> &dict ) const {
    dict.setAutoDelete( true );
    dict.insert( "description", new int(OPimEvent::FDescription) );
    dict.insert( "location", new int(OPimEvent::FLocation) );
    dict.insert( "categories", new int(OPimEvent::FCategories) );
    dict.insert( "uid", new int(OPimEvent::FUid) );
    dict.insert( "type", new int(OPimEvent::FType) );
    dict.insert( "alarm", new int(OPimEvent::FAlarm) );
    dict.insert( "sound", new int(OPimEvent::FSound) );
    dict.insert( "rtype", new int(OPimEvent::FRType) );
    dict.insert( "rweekdays", new int(OPimEvent::FRWeekdays) );
    dict.insert( "rposition", new int(OPimEvent::FRPosition) );
    dict.insert( "rfreq", new int(OPimEvent::FRFreq) );
    dict.insert( "rhasenddate", new int(OPimEvent::FRHasEndDate) );
    dict.insert( "enddt", new int(OPimEvent::FREndDate) );
    dict.insert( "start", new int(OPimEvent::FStart) );
    dict.insert( "end", new int(OPimEvent::FEnd) );
    dict.insert( "note", new int(OPimEvent::FNote) );
    dict.insert( "created", new int(OPimEvent::FRCreated) );
    dict.insert( "recparent", new int(OPimEvent::FRecParent) );
    dict.insert( "recchildren", new int(OPimEvent::FRecChildren) );
    dict.insert( "exceptions", new int(OPimEvent::FRExceptions) );
    dict.insert( "timezone", new int(OPimEvent::FTimeZone) );
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

    int total_written;
    QString strFileNew = m_name + ".new";

    QFile f( strFileNew );
    if (!f.open( IO_WriteOnly | IO_Raw ) ) return false;

    QString buf( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    buf += "<!DOCTYPE DATEBOOK><DATEBOOK>\n";
    buf += "<events>\n";
    QCString str = buf.utf8();
    total_written = f.writeBlock( str.data(), str.length() );
    if ( total_written != int(str.length() ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }

    QAsciiDict<int> dict(OPimEvent::FRecChildren+1);
    initDict( dict );
    QIntDict<QString> revdict( dict.size() );
    dict.setAutoDelete( true );
    // Now we need to reverse the dictionary (!)
    for( QAsciiDictIterator<int> it( dict ); it.current(); ++it ) {
        revdict.insert( (*it), new QString( it.currentKey() ) );
    }

    if (!saveEachEvent( m_raw, revdict, f ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }
    if (!saveEachEvent( m_rep, revdict, f ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }

    buf = "</events>\n</DATEBOOK>\n";
    str = buf.utf8();
    total_written = f.writeBlock( str.data(), str.length() );
    if ( total_written != int(str.length() ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }
    f.close();

    if ( ::rename( strFileNew, m_name ) < 0 ) {
        QFile::remove( strFileNew );
        return false;
    }

    m_changed = false;
    return true;
}

QArray<int> ODateBookAccessBackend_XML::allRecords()const {
    QArray<int> ints( m_raw.count()+ m_rep.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

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

QArray<int> ODateBookAccessBackend_XML::queryByExample(const OPimEvent&, int,  const QDateTime& ) const {
    return QArray<int>();
}

void ODateBookAccessBackend_XML::clear() {
    m_changed = true;
    m_raw.clear();
    m_rep.clear();
}

OPimEvent ODateBookAccessBackend_XML::find( int uid ) const{
    if ( m_raw.contains( uid ) )
        return m_raw[uid];
    else
        return m_rep[uid];
}

bool ODateBookAccessBackend_XML::add( const OPimEvent& ev ) {
    m_changed = true;
    if (ev.hasRecurrence() )
        m_rep.insert( ev.uid(), ev );
    else
        m_raw.insert( ev.uid(), ev );

    return true;
}

bool ODateBookAccessBackend_XML::remove( int uid ) {
    m_changed = true;
    m_raw.remove( uid );
    m_rep.remove( uid );

    return true;
}

bool ODateBookAccessBackend_XML::replace( const OPimEvent& ev ) {
    remove( ev.uid() );
    return add( ev );
}

QArray<int> ODateBookAccessBackend_XML::rawRepeats()const {
    QArray<int> ints( m_rep.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_rep.begin(); it != m_rep.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}

QArray<int> ODateBookAccessBackend_XML::nonRepeats()const {
    QArray<int> ints( m_raw.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}

OPimEvent::ValueList ODateBookAccessBackend_XML::directNonRepeats()const {
    OPimEvent::ValueList list;
    QMap<int, OPimEvent>::ConstIterator it;
    for (it = m_raw.begin(); it != m_raw.end(); ++it )
        list.append( it.data() );

    return list;
}

OPimEvent::ValueList ODateBookAccessBackend_XML::directRawRepeats()const {
    OPimEvent::ValueList list;
    QMap<int, OPimEvent>::ConstIterator it;
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

    QAsciiDict<int> dict(OPimEvent::FRecChildren+1);
    initDict( dict );

    char* dt = (char*)map_addr;
    int len = attribute.st_size;
    int i = 0;
    char* point;
    const char* collectionString = "<event ";
    int strLen = ::strlen(collectionString);
    int *find;
    QMap<int, QString> map;
    while ( (  point = ::strstrlen( dt+i, len -i, collectionString, strLen ) ) != 0  ) {
        i = point -dt;
        i+= strLen;

        OPimEvent ev;
        map.clear();

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
                ev.setCustomField( attr, str );
            else {
                map[*find] = str;
            }
        }
        /* time to finalize */
        ev.fromMap(map);
        finalizeRecord(ev);
    }
    ::munmap(map_addr, attribute.st_size );
    m_changed = false; // changed during add

    return true;
}

void ODateBookAccessBackend_XML::finalizeRecord( OPimEvent& ev ) 
{
    if ( m_raw.contains( ev.uid() ) || m_rep.contains( ev.uid() ) ) {
        ev.setUid( 1 );
    }

    if ( ev.hasRecurrence() )
        m_rep.insert( ev.uid(), ev );
    else
        m_raw.insert( ev.uid(), ev );
}

QArray<int> ODateBookAccessBackend_XML::matchRegexp(  const QRegExp &r ) const
{
    QArray<int> m_currentQuery( m_raw.count()+ m_rep.count() );
    uint arraycounter = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it )
        if ( it.data().match( r ) )
            m_currentQuery[arraycounter++] = it.data().uid();
    for ( it = m_rep.begin(); it != m_rep.end(); ++it )
        if ( it.data().match( r ) )
            m_currentQuery[arraycounter++] = it.data().uid();

    // Shrink to fit..
    m_currentQuery.resize(arraycounter);

    return m_currentQuery;
}

}
