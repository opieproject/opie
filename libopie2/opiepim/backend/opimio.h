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

#ifndef OPIMIO_H
#define OPIMIO_H

#include <qcstring.h>
#include <qasciidict.h>
#include <qmap.h>
#include <qsocket.h>

class QFile;
class QDataStream;
class QTextStream;

namespace Opie
{

class OAbstractWriter
{
public:
    virtual bool writeString( const char *str ) = 0;
    virtual bool writeString( const QCString &str ) = 0;
};


class OTextStreamWriter : public OAbstractWriter
{
public:
    OTextStreamWriter( QTextStream *stream );

    bool writeString( const char *str );
    bool writeString( const QCString &str );
private:
    QTextStream *m_stream;
};


class OFileWriter : public OAbstractWriter
{
public:
    OFileWriter( QFile *file );

    bool writeString( const char *str );
    bool writeString( const QCString &str );
private:
    QFile *m_file;
};


class OPimXmlHandler
{
public:
    OPimXmlHandler( const char *marker, QAsciiDict<int> &dict );
    const char *itemMarker();
    QAsciiDict<int> &dict();

    virtual void handleItem( QMap<int, QString> &map, QMap<QString, QString> &extramap ) = 0;
private:
    QCString m_itemMarker;
    QAsciiDict<int> &m_dict;
};


class OPimXmlMmapParser
{
public:
    OPimXmlMmapParser( OPimXmlHandler &handler );
    bool parse( const QString &file );
private:
    QCString m_itemMarker;
    OPimXmlHandler &m_handler;
};


class OPimXmlStreamParser
{
    enum ParserMode {
        MODE_COLLECTION,
        MODE_ITEM
    };
public:
    OPimXmlStreamParser( OPimXmlHandler &handler );
    void reset();
    void parseBuffer( char *buffer, uint bufferlen );

private:
    void parseItem();
    int findStr( const char *buf, uint buflen, const char *substr, uint substrlen );
    QCString m_item;
    QCString m_itemMarker;
    uint m_findpos;
    ParserMode m_mode;
    OPimXmlHandler &m_handler;
};


class OPimXmlReader
{
public:
    virtual void read( OPimXmlStreamParser &parser ) = 0;
};


class OPimXmlFileReader: public OPimXmlReader
{
public:
    OPimXmlFileReader( const QString &file );
    void read( OPimXmlStreamParser &parser );
protected:
    QString m_file;
};


class OPimXmlSocketReader: public OPimXmlReader
{
public:
    OPimXmlSocketReader( QSocket *socket );
    void read( OPimXmlStreamParser &parser );
    void readData();
protected:
    QSocket *m_socket;
    OPimXmlStreamParser *m_parser;
};


}

#endif