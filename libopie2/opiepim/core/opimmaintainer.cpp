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

#include <opie2/opimmaintainer.h>

namespace Opie {
OPimMaintainer::OPimMaintainer( int mode, int uid )
    :  m_mode(mode), m_uid(uid )
{}
OPimMaintainer::~OPimMaintainer() {
}
OPimMaintainer::OPimMaintainer( const OPimMaintainer& main ) {
    *this = main;
}
OPimMaintainer &OPimMaintainer::operator=( const OPimMaintainer& main ) {
    m_mode = main.m_mode;
    m_uid  = main.m_uid;

    return *this;
}
bool OPimMaintainer::operator==( const OPimMaintainer& main ) {
    if (m_mode != main.m_mode ) return false;
    if (m_uid  != main.m_uid  ) return false;

    return true;
}
bool OPimMaintainer::operator!=( const OPimMaintainer& main ) {
    return !(*this == main );
}
int OPimMaintainer::mode()const {
    return m_mode;
}
int OPimMaintainer::uid()const {
    return m_uid;
}
void OPimMaintainer::setMode( int mo) {
    m_mode = mo;
}
void OPimMaintainer::setUid( int uid ) {
    m_uid = uid;
}

}
