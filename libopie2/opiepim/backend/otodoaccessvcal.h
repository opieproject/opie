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
#ifndef OPIE_OTODO_ACCESS_VCAL_H
#define OPIE_OTODO_ACCESS_VCAL_H

#include <opie2/otodoaccessbackend.h>

namespace Opie {

class OTodoAccessVCal : public OTodoAccessBackend {
public:
    OTodoAccessVCal(const QString& );
    ~OTodoAccessVCal();

    bool load();
    bool reload();
    bool save();

    QArray<int> allRecords()const;
    QArray<int> matchRegexp(const QRegExp &r) const;
    QArray<int> queryByExample( const OTodo& t, int sort, const QDateTime& d = QDateTime() );
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end,
                                bool includeNoDates );
    QArray<int> overDue();
    QArray<int> sorted( bool asc, int sortOrder, int sortFilter,
                        int cat );
    OTodo find(int uid)const;
    void clear();
    bool add( const OTodo& );
    bool remove( int uid );
    bool replace( const OTodo& );

    void removeAllCompleted();
    virtual QBitArray supports()const;

private:
    static QBitArray sup();
    bool m_dirty : 1;
    QString m_file;
    QMap<int, OTodo> m_map;
};

}

#endif
