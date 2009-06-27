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
#include <qxml.h>

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


class OPimXmlParser : public QXmlDefaultHandler
{
public:
    bool startDocument();
    bool endElement( const QString&, const QString&, const QString &name );
    bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );

    virtual void foundItemElement( const QXmlAttributes &attrs ) = 0;

protected:
    void init( QString mainElement, QString itemElement );

private:
    QString m_mainElement;
    QString m_itemElement;
    bool m_inMain;
};


class OPimXmlTextStreamReader
{
public:
    OPimXmlTextStreamReader( OPimXmlParser &parser );
    bool read( QTextStream &stream );
private:
    OPimXmlParser &m_parser;
};

}

#endif