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
#ifndef TODO_TODO_SHOW_H
#define TODO_TODO_SHOW_H

#include <qstring.h>
#include <qwidget.h>

#include <opie/todoevent.h>

namespace Todo {
    /**
     * TodoShow is the baseclass of
     * of all TodoShows.
     * The first implementation is a QTextView
     * implementation showing the Todo as richtext
     */
    template<T = QWidget>
    class TodoShow : public T{
        Q_OBJECT
    public:
        TodoShow( QWidget* parent ) : T(parent) {}
        virtual ~TodoShow() {};
        virtual QString type()const = 0;

    protected slots:
        virtual void slotShow( const ToDoEvent& ev ) {};
    };
};

#endif
