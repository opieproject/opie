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
#include <qshared.h>

#include "opimstate.h"

namespace Opie {
/*
 * for one int this does not make
 * much sense but never the less
 * we will do it for the future
 */
struct OPimState::Data : public QShared {
    Data() : QShared(),state(Undefined) {
    }
    int state;
};

OPimState::OPimState( int state ) {
    data = new Data;
    data->state = state;
}
OPimState::OPimState( const OPimState& st) :
    data( st.data ) {
    /* ref up */
    data->ref();
}
OPimState::~OPimState() {
    if ( data->deref() ) {
        delete data ;
        data = 0;
    }
}
bool OPimState::operator==( const OPimState& st) {
    if ( data->state == st.data->state ) return true;

    return false;
}
OPimState &OPimState::operator=( const OPimState& st) {
    st.data->ref();
    deref();
    data = st.data;

    return *this;
}
void OPimState::setState( int st) {
    copyInternally();
    data->state = st;
}
int OPimState::state()const {
    return data->state;
}
void OPimState::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}
void OPimState::copyInternally() {
    /* we need to change it */
    if ( data->count != 1 ) {
        data->deref();
        Data* d2 = new Data;
        d2->state = data->state;
        data = d2;
    }
}

}
