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
#ifndef OPIE_DATE_BOOK_ACCESS_BACKEND_XML__H
#define OPIE_DATE_BOOK_ACCESS_BACKEND_XML__H

#include <qmap.h>

#include <opie2/odatebookaccessbackend.h>

namespace Opie {
/**
 * This is the default XML implementation for DateBoook XML storage
 * It fully implements the interface
 * @see ODateBookAccessBackend
 * @see OPimAccessBackend
 */
class ODateBookAccessBackend_XML : public ODateBookAccessBackend {
public:
    ODateBookAccessBackend_XML( const QString& appName,
                                const QString& fileName = QString::null);
    ~ODateBookAccessBackend_XML();

    bool load();
    bool reload();
    bool save();

    QArray<int> allRecords()const;
    QArray<int> matchRegexp(const QRegExp &r) const;
    QArray<int> queryByExample( const OPimEvent&, int, const QDateTime& d = QDateTime() );
    OPimEvent find( int uid )const;
    void clear();
    bool add( const OPimEvent& ev );
    bool remove( int uid );
    bool replace( const OPimEvent& ev );

    QArray<UID> rawEvents()const;
    QArray<UID> rawRepeats()const;
    QArray<UID> nonRepeats()const;

    OPimEvent::ValueList directNonRepeats();
    OPimEvent::ValueList directRawRepeats();

private:
    bool m_changed :1 ;
    bool loadFile();
    inline void finalizeRecord( OPimEvent& ev );
    inline void setField( OPimEvent&, int field, const QString& val );
    QString m_name;
    QMap<int, OPimEvent> m_raw;
    QMap<int, OPimEvent> m_rep;

    struct Data;
    Data* data;
    class Private;
    Private *d;
};

}

#endif
