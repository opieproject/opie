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
#ifndef OPIE_MEMO_ACCESS_BACKEND_XML__H
#define OPIE_MEMO_ACCESS_BACKEND_XML__H

#include <qmap.h>
#include <qasciidict.h>

#include <opie2/omemoaccessbackend.h>
#include <opie2/opimio.h>

namespace Opie {

class OPimMemoAccessBackend_XML;

class OPimMemoXmlHandler : public OPimXmlHandler
{
public:
    OPimMemoXmlHandler( QAsciiDict<int> &dict, OPimMemoAccessBackend_XML &backend );
    void handleItem( QMap<int, QString> &map, QMap<QString, QString> &extramap );
protected:
    OPimMemoAccessBackend_XML &m_backend;
};


/**
 * This is the XML implementation for memo XML storage
 * Note: this is only intended to be used for synchronisation!
 * @see OPimMemoAccessBackend
 * @see OPimAccessBackend
 */
class OPimMemoAccessBackend_XML : public OPimMemoAccessBackend {
public:
    OPimMemoAccessBackend_XML();
    ~OPimMemoAccessBackend_XML();

    bool load();
    bool reload();
    bool save();
    bool wasChangedExternally();

    bool write( OAbstractWriter &wr );
    bool read( OPimXmlReader &rd );

    QArray<int> allRecords()const;
    QArray<int> matchRegexp(const QRegExp &r) const;
    QArray<int> queryByExample( const OPimMemo&, int, const QDateTime& d = QDateTime() )const;
    OPimMemo find( int uid )const;
    void clear();
    bool add( const OPimMemo& ev );
    bool remove( int uid );
    bool replace( const OPimMemo& ev );

    friend void OPimMemoXmlHandler::handleItem( QMap<int, QString> &map, QMap<QString, QString> &extramap );
private:

    void initDict( QAsciiDict<int> &dict ) const;
    inline void finalizeRecord( OPimMemo& ev );
    QMap<int, OPimMemo> m_items;

    struct Data;
    Data* data;
    class Private;
    Private *d;
};

}

#endif
