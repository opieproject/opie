/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Holger Freyther <freyther@handhelds.org>
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

#include "opimtodosortvector.h"
#include <opie2/otodoaccess.h>

namespace Opie {
namespace Internal {

inline QString string( const OPimTodo& todo) {
    return  todo.summary().isEmpty() ?
        todo.description().left(20 ) :
        todo.summary();
}

inline int completed( const OPimTodo& todo1, const OPimTodo& todo2) {
    int ret = 0;
    if ( todo1.isCompleted() ) ret++;
    if ( todo2.isCompleted() ) ret--;
    return ret;
}

inline int priority( const OPimTodo& t1, const OPimTodo& t2) {
    return ( t1.priority() - t2.priority() );
}

inline int summary( const OPimTodo& t1, const OPimTodo& t2) {
    return QString::compare( string(t1), string(t2) );
}

inline int deadline( const OPimTodo& t1, const OPimTodo& t2) {
    int ret = 0;
    if ( t1.hasDueDate() &&
         t2.hasDueDate() )
        ret = t2.dueDate().daysTo( t1.dueDate() );
    else if ( t1.hasDueDate() )
        ret = -1;
    else if ( t2.hasDueDate() )
        ret = 1;
    else
        ret = 0;

    return ret;
}


OPimTodoSortVector::OPimTodoSortVector( uint size, bool asc, int sort )
    : OPimSortVector<OPimTodo>( size, asc, sort )
{}

int OPimTodoSortVector::compareItems( const OPimTodo& con1, const OPimTodo& con2 ) {
    bool seComp, sePrio, seSum, seDeadline;
    seComp = sePrio = seDeadline = seSum = false;
    int ret =0;
    bool asc = sortAscending();

    /* same item */
    if ( con1.uid() == con2.uid() )
        return 0;

    switch ( sortOrder() ) {
    case OPimTodoAccess::Completed: {
        ret = completed( con1, con2 );
        seComp = TRUE;
        break;
    }
    case OPimTodoAccess::Priority: {
        ret = priority( con1, con2 );
        sePrio = TRUE;
        break;
    }
    case OPimTodoAccess::SortSummary: {
        ret  = summary( con1, con2 );
        seSum = TRUE;
        break;
    }
    case OPimTodoAccess::SortByDate:
    case OPimTodoAccess::Deadline: {
        ret = deadline( con1, con2 );
        seDeadline = TRUE;
        break;
    }
    default:
        ret = 0;
        break;
    };
    /*
     * FIXME do better sorting if the first sort criteria
     * ret equals 0 start with complete and so on...
     */

    /* twist it we're not ascending*/
    if (!asc)
        ret = ret * -1;

    if ( ret )
        return ret;

    // default did not gave difference let's try it other way around
    /*
     * General try if already checked if not test
     * and return
     * 1.Completed
     * 2.Priority
     * 3.Description
     * 4.DueDate
     */
    if (!seComp ) {
        if ( (ret = completed( con1, con2 ) ) ) {
            if (!asc ) ret *= -1;
            return ret;
        }
    }
    if (!sePrio ) {
        if ( (ret = priority( con1, con2 ) ) ) {
            if (!asc ) ret *= -1;
            return ret;
        }
    }
    if (!seSum ) {
        if ( (ret = summary(con1, con2 ) ) ) {
            if (!asc) ret *= -1;
            return ret;
        }
    }
    if (!seDeadline) {
        if ( (ret = deadline( con1, con2 ) ) ) {
            if (!asc) ret *= -1;
            return ret;
        }
    }

    return 0;
}

}
}
