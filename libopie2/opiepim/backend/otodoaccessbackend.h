/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
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
#ifndef OPIE_TODO_ACCESS_BACKEND_H
#define OPIE_TODO_ACCESS_BACKEND_H

#include <qbitarray.h>

#include <opie2/otodo.h>
#include <opie2/opimaccessbackend.h>

namespace Opie {
class OTodoAccessBackend : public OPimAccessBackend<OTodo> {
public:
    OTodoAccessBackend();
    ~OTodoAccessBackend();
    virtual QArray<int> effectiveToDos( const QDate& start,
                                        const QDate& end,
                                        bool includeNoDates ) = 0;
    virtual QArray<int> overDue() = 0;
    virtual QArray<int> sorted( bool asc, int sortOrder, int sortFilter,
                                int cat ) = 0;
    virtual void        removeAllCompleted() = 0;
    virtual QBitArray supports()const = 0;
    
private:
    class Private;
    Private *d;

};

}

#endif
