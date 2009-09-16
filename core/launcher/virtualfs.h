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

#ifndef VIRTUALFS_H
#define VIRTUALFS_H

#include <qtextstream.h>
#include <qstring.h>
#include <qlist.h>
#include <qdatetime.h>

#include <opie2/opimio.h>

#include "syncaccessmanager.h"

using namespace Opie;

class VirtualReader
{
public:
    virtual ~VirtualReader() {};
    virtual void read( QTextStream &stream ) = 0;
};

class VirtualWriter
{
public:
    virtual ~VirtualWriter() {};
    virtual void write( OPimXmlReader &reader ) = 0;
};

class VirtualFile
{
public:
    VirtualFile( const QString &path, VirtualReader *rd, VirtualWriter *wr );
    virtual ~VirtualFile();

    const QString &filePath() const;
    VirtualReader *reader() const;
    VirtualWriter *writer() const;
private:
    VirtualReader *m_reader;
    VirtualWriter *m_writer;
    QString m_filePath;
};


class VirtualFS
{
public:
    VirtualFS();

    void init( const QString &basedir, SyncAccessManager *syncAccessManager );
    void fileListing( const QString &path, QTextStream &stream );
    bool canRead( const QString &file );
    bool canWrite( const QString &file );
    bool isVirtual( const QString &file );

    bool deleteFile( const QString &file );
    VirtualReader *readFile( const QString &file );
    VirtualWriter *writeFile( const QString &file );

private:
    QString fileListingEntry( const QString &name, bool dir, const QDateTime &lastModified );
    
    QList<VirtualFile> m_files;
};

class VirtualPimReader: public VirtualReader
{
public:
    VirtualPimReader( SyncAccessManager *manager );
protected:
    SyncAccessManager *m_manager;
};

class VirtualPimWriter: public VirtualWriter
{
public:
    VirtualPimWriter( SyncAccessManager *manager );
protected:
    SyncAccessManager *m_manager;
};

class VirtualDateBookReader: public VirtualPimReader
{
public:
    VirtualDateBookReader( SyncAccessManager *manager );
    void read( QTextStream &stream );
};

class VirtualDateBookWriter: public VirtualPimWriter
{
public:
    VirtualDateBookWriter( SyncAccessManager *manager );
    void write( OPimXmlReader &reader );
};

class VirtualContactReader: public VirtualPimReader
{
public:
    VirtualContactReader( SyncAccessManager *manager );
    void read( QTextStream &stream );
};

class VirtualContactWriter: public VirtualPimWriter
{
public:
    VirtualContactWriter( SyncAccessManager *manager );
    void write( OPimXmlReader &reader );
};

class VirtualTodoListReader: public VirtualPimReader
{
public:
    VirtualTodoListReader( SyncAccessManager *manager );
    void read( QTextStream &stream );
};

class VirtualTodoListWriter: public VirtualPimWriter
{
public:
    VirtualTodoListWriter( SyncAccessManager *manager );
    void write( OPimXmlReader &reader );
};

class VirtualNotesReader: public VirtualPimReader
{
public:
    VirtualNotesReader( SyncAccessManager *manager );
    void read( QTextStream &stream );
};

class VirtualNotesWriter: public VirtualPimWriter
{
public:
    VirtualNotesWriter( SyncAccessManager *manager );
    void write( OPimXmlReader &reader );
};

#endif
