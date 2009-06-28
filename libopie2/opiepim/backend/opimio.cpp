/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.             
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

#include "opimio.h"

#include <opie2/odebug.h>

#include <qtopia/stringutil.h>

#include <qtextstream.h>
#include <qdatastream.h>
#include <qfile.h>

#include <errno.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <string.h>


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


namespace Opie
{

///////////////////////////////////////////////////////////////////////

OTextStreamWriter::OTextStreamWriter( QTextStream *stream )
{
    m_stream = stream;
}

bool OTextStreamWriter::writeString( const char *str )
{
    *m_stream << str;
    return true;
}

bool OTextStreamWriter::writeString( const QCString &str )
{
    *m_stream << str;
    return true;
}

///////////////////////////////////////////////////////////////////////

OFileWriter::OFileWriter( QFile *file )
{
    m_file = file;
}

bool OFileWriter::writeString( const char *str )
{
    uint len = strlen( str );
    uint total_written = m_file->writeBlock( str, len );
    return ( total_written == len );
}

bool OFileWriter::writeString( const QCString &str )
{
    uint len = str.length();
    uint total_written = m_file->writeBlock( str.data(), len );
    return ( total_written == len );
}
 
///////////////////////////////////////////////////////////////////////

OPimXmlHandler::OPimXmlHandler( const char *marker, QAsciiDict<int> &dict )
    : m_itemMarker( marker ), m_dict( dict )
{
}

const char * OPimXmlHandler::itemMarker()
{
    return m_itemMarker;
}

QAsciiDict<int> & OPimXmlHandler::dict()
{
    return m_dict;
}


///////////////////////////////////////////////////////////////////////


OPimXmlStreamParser::OPimXmlStreamParser( OPimXmlHandler &handler )
    : m_handler( handler )
{
    m_itemMarker.sprintf("<%s ", handler.itemMarker() );
    reset();
}

void OPimXmlStreamParser::reset()
{
    m_findpos = 0;
    m_item = "";
    m_mode = MODE_COLLECTION;
}

int OPimXmlStreamParser::findStr( const char *buf, uint buflen, const char *substr, uint substrlen ) 
{
    for( uint i=0; i<buflen; i++ ) {
        if( buf[i] == substr[m_findpos] )
            m_findpos++;
        else
            m_findpos=0;
        if( m_findpos == substrlen ) {
            m_findpos=0;
            return i; 
        }
    }
    return -1;
}

void OPimXmlStreamParser::parseBuffer( char *buffer, uint bufferlen )
{
    char *buf = buffer;
    uint buflen = bufferlen;
    int pos = 0;
    int substrlen = m_itemMarker.length();

    while( buflen > 0 ) {
        if( m_mode == MODE_COLLECTION ) {
            pos = findStr( buf, buflen, m_itemMarker, substrlen );
            if( pos > -1 ) {
                m_mode = MODE_ITEM;
                buf += pos;
                buflen -= pos;
            }
            else
                break;
        }
        else if( m_mode == MODE_ITEM ) {
            pos = findStr( buf, buflen, "/>", 2 );
            if( pos > -1 ) {
                m_item += QCString( buf, pos );
                parseItem();
                m_item = "";
                m_mode = MODE_COLLECTION;
                buf += pos;
                buflen -= pos;
            }
            else {
                m_item += QCString( buf, buflen+1 );
                break;
            }
        }
    }
}

void OPimXmlStreamParser::parseItem() 
{
    int i = 0;
    int *find;
    QMap<int, QString> map;
    QMap<QString, QString> extramap;
    QAsciiDict<int> &dict = m_handler.dict();

    char *dt = m_item.data();
    int len = m_item.length();

    // borrowed from mmap parser
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

        // add key + value
        find = dict[attr.data()];
        if (!find)
            extramap[attr] = str;
        else
            map[*find] = str;
    }

    m_handler.handleItem( map, extramap );
}

///////////////////////////////////////////////////////////////////////

OPimXmlMmapParser::OPimXmlMmapParser( OPimXmlHandler &handler )
    : m_handler( handler )
{
    m_itemMarker.sprintf("<%s ", handler.itemMarker() );
}

bool OPimXmlMmapParser::parse( const QString &file )
{
    QMap<int, QString> map;
    QMap<QString, QString> extramap;
    QAsciiDict<int> &dict = m_handler.dict();

    // here the custom XML parser from TT it's GPL
    // but we want to push OpiePIM... to TT.....
    // mmap part from zecke :)
    int fd = ::open( QFile::encodeName( file ).data(), O_RDONLY );
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
    /* advise the kernel who we want to read it */
    ::madvise( map_addr, attribute.st_size, MADV_SEQUENTIAL );
    /* we do not the file any more */
    ::close( fd );

    char* dt = (char*)map_addr;
    int len = attribute.st_size;
    int i = 0;
    char *point;
    QCString itemMarker = m_itemMarker.data();
    int strLen = itemMarker.length();
    int *find;
    while ( ( point = strstrlen( dt+i, len -i, itemMarker, strLen ) ) != 0l ) {
        i = point -dt;
        i+= strLen;
        map.clear();
        extramap.clear();

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

            // add key + value
            find = dict[attr.data()];
            if (!find)
                extramap[attr] = str;
            else
                map[*find] = str;
        }

        m_handler.handleItem( map, extramap );
    }

    ::munmap(map_addr, attribute.st_size );

    return true;
}

///////////////////////////////////////////////////////////////////////

OPimXmlFileReader::OPimXmlFileReader( const QString &file )
    : m_file( file )
{
}

void OPimXmlFileReader::read( OPimXmlStreamParser &parser )
{
    QFile f( QFile::encodeName( m_file ) );
    f.open( IO_ReadOnly );

    QCString cstr;
    cstr.resize(11);
    char *buf = cstr.data();
    while ( !f.atEnd() ) {
        uint len = f.readBlock( buf, 10 );
        parser.parseBuffer( buf, len );
    }
    f.close();
}

///////////////////////////////////////////////////////////////////////

OPimXmlSocketReader::OPimXmlSocketReader( QSocket *socket )
    : m_socket( socket )
{
    m_parser = NULL;
}

void OPimXmlSocketReader::read( OPimXmlStreamParser &parser )
{
    m_parser = &parser;
    while( m_socket->state() != QSocket::Idle || m_socket->bytesAvailable() != 0 ) {
        if ( m_socket->waitForMore( 100 ) > 0 )
            readData();
    }
    m_parser = NULL;
}

void OPimXmlSocketReader::readData()
{
    if( m_parser ) {
        int len = m_socket->bytesAvailable();
        QCString buf;
        while( len > 0 ) {
            buf.resize( len + 1 );
            len = m_socket->readBlock( buf.data(), len );
            if( len == -1 ) {
                oerr << "error occurred during socket readBlock!" << oendl;
                return;
            }
            m_parser->parseBuffer( buf.data(), len ); 
            len = m_socket->bytesAvailable();
        }
    }
}

}