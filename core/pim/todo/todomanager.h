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

#include <qarray.h>
#include <qobject.h>
#include <qstring.h>

#include <qpe/categories.h>

#include <opie/otodo.h>
#include <opie/otodoaccess.h>

#include "smalltodo.h"


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
        OTodo event(int uid );

        void updateList();
        /** returns the iterator sorted if set sorted*/
        OTodoAccess::List list()const;
        OTodoAccess::List sorted( bool asc, int so, int f,  int cat );

        OTodoAccess::List::Iterator overDue();
        OTodoAccess::List::Iterator fromTo( const QDate& start,
                                 const QDate& end );
        OTodoAccess::List::Iterator query( const OTodo& ev, int query );

        void setCategory( bool sort, int category = -1);
        void setShowOverDue(  bool show );
        void setSortOrder( int sortOrder );
        void setSortASC( bool );
        void sort();

        OTodoAccess* todoDB();
        bool saveAll();


    signals:
        void update();
        void updateCategories();

    public slots:
        void add( const OTodo& );
        void update( int uid,  const SmallTodo& );
        void update( int uid, const OTodo& );
        void remove( int uid );
        void remove( const QArray<int>& );
        void removeAll();
        void reload();
        void save();

    private:
        OTodoAccess* m_db;
        OTodoAccess::List m_list;
        OTodoAccess::List::Iterator m_it;
        OTodoAccess::List m_sorted;
        Categories m_cat;
        int m_ca;
        /* sort cat */
        bool m_shCat;
        int m_sortOrder;
        bool m_asc;

    };
};

#endif
