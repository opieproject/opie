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
#ifndef OPIE_DATE_BOOK_ACCESS_BACKEND_SQL__H
#define OPIE_DATE_BOOK_ACCESS_BACKEND_SQL__H

#include <qmap.h>
#include <opie2/osqlresult.h>

#include <opie2/odatebookaccessbackend.h>

namespace Opie {
namespace DB {
class OSQLDriver;
}
}

namespace Opie {
/**
 * This is the default SQL implementation for DateBoook SQL storage
 * It fully implements the interface
 * @see ODateBookAccessBackend
 * @see OPimAccessBackend
 */
class ODateBookAccessBackend_SQL : public ODateBookAccessBackend {
public:
    ODateBookAccessBackend_SQL( const QString& appName,
                                const QString& fileName = QString::null);
    ~ODateBookAccessBackend_SQL();

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
    bool loadFile();
    QString m_fileName;
    QArray<int> m_uids;

    QMap<int, QString> m_fieldMap;
    QMap<QString, int> m_reverseFieldMap;

    Opie::DB::OSQLDriver* m_driver;

    class Private;
    Private *d;

    void initFields();
    void update();

    QArray<int> extractUids( Opie::DB::OSQLResult& res ) const;
    QMap<QString, QString> requestCustom( int uid ) const;

};

}

#endif
