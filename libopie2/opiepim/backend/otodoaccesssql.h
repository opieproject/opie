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
#ifndef OPIE_PIM_ACCESS_SQL_H
#define OPIE_PIM_ACCESS_SQL_H

#include <qasciidict.h>

#include <opie2/otodoaccessbackend.h>

namespace Opie {
namespace DB   {
class OSQLDriver;
class OSQLResult;
class OSQLResultItem;
}
}

namespace Opie {

class OPimTodoAccessBackendSQL : public OPimTodoAccessBackend {
public:
    OPimTodoAccessBackendSQL( const QString& file );
    ~OPimTodoAccessBackendSQL();

    bool load();
    bool reload();
    bool save();
    QArray<int> allRecords()const;

    QArray<int> queryByExample( const OPimTodo& t, int settings, const QDateTime& d = QDateTime() );
    OPimTodo find(int uid)const;
    OPimTodo find(int uid, const QArray<int>&, uint cur, Frontend::CacheDirection )const;
    void clear();
    bool add( const OPimTodo& t );
    bool remove( int uid );
    bool replace( const OPimTodo& t );

    QArray<int> overDue();
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end, bool includeNoDates );
    QArray<int> sorted(bool asc, int sortOrder, int sortFilter, int cat );

    QBitArray supports()const;
    QArray<int> matchRegexp(  const QRegExp &r ) const;
    void removeAllCompleted();
    

private:
    void update()const;
    void fillDict();
    inline bool date( QDate& date, const QString& )const;
    inline OPimTodo todo( const Opie::DB::OSQLResult& )const;
    inline OPimTodo todo( Opie::DB::OSQLResultItem& )const;
    inline QArray<int> uids( const Opie::DB::OSQLResult& )const;
    OPimTodo todo( int uid )const;
    QBitArray sup() const;

    QAsciiDict<int> m_dict;
    Opie::DB::OSQLDriver* m_driver;
    QArray<int> m_uids;
    bool m_dirty : 1;
};

}

#endif
