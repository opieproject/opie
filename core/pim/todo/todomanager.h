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

#include <opie/tododb.h>
#include <opie/todoevent.h>

#include "smalltodo.h"

using namespace Opie;

namespace Todo {
    class TodoManager : public QObject{
        Q_OBJECT
    public:
        TodoManager(QObject* obj = 0);
        ~TodoManager();

        QStringList categories()/*const*/;
        int catId(const QString&);
        ToDoEvent event(int uid );
        ToDoDB::Iterator begin();
        ToDoDB::Iterator end();

        ToDoDB::Iterator overDue();
        ToDoDB::Iterator fromTo( const QDate& start,
                                 const QDate& end );
        ToDoDB::Iterator query( const ToDoEvent& ev, int query );
        ToDoDB* todoDB();
        bool saveAll();


    signals:
        void update();
        void updateCategories();

    public slots:
        void add( const ToDoEvent& );
        void update( int uid,  const SmallTodo& );
        void update( int uid, const ToDoEvent& );
        void remove( int uid );
        void remove( const QArray<int>& );
        void removeAll();
        void reload();
        void save();

    private:
        ToDoDB* m_db;
        Categories m_cat;

    };
};

#endif
