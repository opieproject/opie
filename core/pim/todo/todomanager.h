/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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

#ifndef OPIE_TODO_MANAGER_H
#define OPIE_TODO_MANAGER_H

#include <opie2/opimtodo.h>
#include <opie2/otodoaccess.h>

#include <qpe/categories.h>

#include <qarray.h>
#include <qobject.h>
#include <qstring.h>

#include "smalltodo.h"

using Opie::OPimTodo;
using Opie::OPimTodoAccess;

namespace Todo {
    class TodoManager : public QObject{
        Q_OBJECT
    public:
        bool isLoaded()const;
        void load();
        TodoManager(QObject* obj = 0);
        ~TodoManager();

        QStringList categories()/*const*/;
        int catId(const QString&);
        OPimTodo event(int uid );

        void updateList();
        /** returns the iterator sorted if set sorted*/
        OPimTodoAccess::List list()const;
        OPimTodoAccess::List sorted( bool asc, int so, int f,  int cat );

        OPimTodoAccess::List::Iterator overDue();
        OPimTodoAccess::List::Iterator fromTo( const QDate& start,
                                 const QDate& end );
        OPimTodoAccess::List::Iterator query( const OPimTodo& ev, int query );

        void setCategory( bool sort, int category = -1);
        void setShowOverDue(  bool show );
        void setSortOrder( int sortOrder );
        void setSortASC( bool );
        void sort();

        OPimTodoAccess* todoDB();
        bool saveAll();


    signals:
        void update();
        void updateCategories();

    public slots:
        void add( const OPimTodo& );
        void update( int uid,  const SmallTodo& );
        void update( int uid, const OPimTodo& );
        bool remove( int uid );
        void remove( const QArray<int>& );

        /**
         * remove all completed
         */
        void removeCompleted();
        void removeAll();
        void reload();
        void save();

    private:
        OPimTodoAccess* m_db;
        OPimTodoAccess::List m_list;
        OPimTodoAccess::List::Iterator m_it;
        OPimTodoAccess::List m_sorted;
        Categories m_cat;
        int m_ca;
        /* sort cat */
        bool m_shCat;
        int m_sortOrder;
        bool m_asc;

    };
};

#endif
