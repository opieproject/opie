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
#ifndef OPIE_PIM_STATE_H
#define OPIE_PIM_STATE_H

#include <qstring.h>

namespace Opie {
/**
 * The State of a Task
 * This class encapsules the state of a todo
 * and it's shared too
 */
/*
 * in c a simple struct would be enough ;)
 * g_new_state();
 * g_do_some_thing( state_t*  );
 * ;)
 */
class OPimState {
public:
    enum State {
        Started = 0,
        Postponed,
        Finished,
        NotStarted,
        Undefined
    };
    OPimState( int state = Undefined );
    OPimState( const OPimState& );
    ~OPimState();

    bool operator==( const OPimState& );
    OPimState &operator=( const OPimState& );
    void setState( int state);
    int state()const;
private:
    void deref();
    inline void copyInternally();

    struct Data;
    Data* data;

    class Private;
    Private *d;
};

}
#endif
