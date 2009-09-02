/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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
/*
 * SQL Backend for the OPIE-Memo Database.
 */

#ifndef _OPimMemoAccessBackend_SQL_
#define _OPimMemoAccessBackend_SQL_

#include <opie2/omemoaccessbackend.h>
#include <opie2/omemoaccess.h>

#include <qlist.h>
#include <qdict.h>

/* aren't in namespace Opie yet - alwin */
namespace Opie {
namespace DB   {
class OSQLDriver;
class OSQLResult;
class OSQLResultItem;
}
}

namespace Opie {

/**
 * This class is the SQL implementation of a memo backend
 * it does implement everything available for OPimMemo.
 * @see OPimAccessBackend for more information of available methods
 */
class OPimMemoAccessBackend_SQL : public OPimMemoAccessBackend {
 public:
    OPimMemoAccessBackend_SQL ( const QString& appname, const QString& filename = QString::null );

    ~OPimMemoAccessBackend_SQL ();

    bool save();

    bool load ();

    void clear ();

    bool wasChangedExternally();

    UIDArray allRecords() const;

    OPimMemo find( int uid ) const;
    OPimMemo find( int uid, const UIDArray& items, uint cur, Frontend::CacheDirection ) const;

    UIDArray matchRegexp(  const QRegExp &r ) const;

    uint querySettings() const;

    bool hasQuerySettings (uint querySettings) const;

    UIDArray sorted( const UIDArray& ar, bool asc, int sortOrder,
             int filter, const QArray<int>& categories)const;

    bool add ( const OPimMemo &newmemo );

    bool replace ( const OPimMemo &memo );

    bool remove ( int uid );
    bool reload();

 private:
    UIDArray extractUids( Opie::DB::OSQLResult& res ) const;
    OPimMemo readMemo( const Opie::DB::OSQLResultItem& resultItem ) const;
    OPimMemo requestMemosAndCache( int uid, const QArray<int>& cachelist ) const;
    void update();

 protected:
    bool m_changed;
    QString m_fileName;
    UIDArray m_uids;

    Opie::DB::OSQLDriver* m_driver;
};

}

#endif
