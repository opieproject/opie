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
#ifndef OPIE_PIM_MAINTAINER_H
#define OPIE_PIM_MAINTAINER_H

#include <qstring.h>

namespace Opie {
/**
 * Who maintains what?
 */
class OPimMaintainer {
public:
    enum Mode { Undefined = -1,
                Nothing = 0,
                Responsible,
                DoneBy,
                Coordinating,
    };
    OPimMaintainer( int mode = Undefined, int uid = 0);
    OPimMaintainer( const OPimMaintainer& );
    ~OPimMaintainer();

    OPimMaintainer &operator=( const OPimMaintainer& );
    bool operator==( const OPimMaintainer& );
    bool operator!=( const OPimMaintainer& );


    int mode()const;
    int uid()const;

    void setMode( int mode );
    void setUid( int uid );

private:
    int m_mode;
    int m_uid;
    class Private;
    Private *d;

};

}

#endif
