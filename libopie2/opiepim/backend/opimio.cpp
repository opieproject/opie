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

#include <string.h>

#include <qtextstream.h>
#include <qdatastream.h>
#include <qfile.h>

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

void OPimXmlParser::init( QString mainElement, QString itemElement ) 
{
    m_mainElement = mainElement;
    m_itemElement = itemElement;
}

bool OPimXmlParser::startDocument()
{ 
    m_inMain = false;
    return true; 
}

bool OPimXmlParser::endElement( const QString&, const QString&, const QString &name )
{
    if( name == m_mainElement )
        m_inMain = false;

    return true;
}

bool OPimXmlParser::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
{
    if( m_inMain && name == m_itemElement )
        foundItemElement( attrs );
    else if( name == m_mainElement )
        m_inMain = true;

    return true;
}
 
///////////////////////////////////////////////////////////////////////

OPimXmlTextStreamReader::OPimXmlTextStreamReader( OPimXmlParser &parser )
    : m_parser( parser )
{
}

bool OPimXmlTextStreamReader::read( QTextStream &stream )
{
    QXmlInputSource source( stream );
    QXmlSimpleReader reader;
    reader.setContentHandler( &m_parser );
    return reader.parse( source );
}

}