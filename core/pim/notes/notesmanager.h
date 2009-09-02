/*
                             This file is part of the Opie Project

                             Copyright (C) 2009 Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OPIE_NOTES_MANAGER_H
#define OPIE_NOTES_MANAGER_H

#include <opie2/opimtodo.h>
#include <opie2/omemoaccess.h>

#include <qpe/categories.h>

#include <qarray.h>
#include <qobject.h>
#include <qstring.h>

namespace Opie {
namespace Notes {
    class NotesManager : public QObject{
        Q_OBJECT
    public:
        bool isLoaded() const;
        void load();
        NotesManager(QObject* obj = 0);
        ~NotesManager();

        QStringList categories()/*const*/;
        int catId(const QString&);
        OPimMemo memo(int uid );

        void updateList();
        /** returns the iterator sorted if set sorted*/
        OPimMemoAccess::List list() const;
        OPimMemoAccess::List sorted( bool asc, int so, int f,  int cat );

        OPimMemoAccess::List::Iterator query( const OPimMemo& ev, int query );

        void setCategory( bool sort, int category = -1);
        void setSortOrder( int sortOrder );
        void setSortASC( bool );
        void sort();

        OPimMemoAccess* memoDB();
        bool saveAll();


    signals:
        void update();
        void updateCategories();

    public slots:
        void add( const OPimMemo& );
        void update( int uid, const OPimMemo& );
        bool remove( int uid );
        void remove( const QArray<int>& );

        void removeAll();
        void reload();
        void save();

    private:
        OPimMemoAccess* m_db;
        OPimMemoAccess::List m_list;
        OPimMemoAccess::List::Iterator m_it;
        OPimMemoAccess::List m_sorted;
        Categories m_cat;
        int m_ca;
        /* sort cat */
        bool m_shCat;
        int m_sortOrder;
        bool m_asc;

};

}
}

#endif
