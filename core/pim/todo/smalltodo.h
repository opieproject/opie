/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <zecke@handhelds.org>
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

#ifndef TODO_SMALL_TODO_H
#define TODO_SMALL_TODO_H

#include <qdatetime.h>
#include <qstring.h>
#include <qstringlist.h>

//using namespace Opie;

namespace Todo {
    /**
     * SmallTodo holds everything necessary
     * to represent a ToDoEvent without holding
     * everything from a ToDoEvent
     */
    class SmallTodo {
    public:
        SmallTodo(int uid = 0,
                  bool comp = false,
                  const QDate& date = QDate::currentDate(),
                  const QString& = QString::null,
                  const QStringList& = QStringList() );
        SmallTodo( const SmallTodo& );
        ~SmallTodo();

        bool operator==( const SmallTodo& );
        bool operator==( const SmallTodo& ) const;

        SmallTodo &operator=( const SmallTodo& );
        QString name()const;
        QStringList categories()const;
        int uid()const;
        bool isCompleted()const;
        QDate date()const;
    private:
        struct SmallTodoPrivate;
        SmallTodoPrivate* d;
        void deref();
    };
};

#endif
