/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2004 Rajko 'Alwin' Albrecht <alwin@handhelds.org>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :          the terms of the GNU Library General Public
.="- .-=="i,     .._        License as published by the Free Software
- .   .-<_>     .<>         Foundation; either version 2 of the License,
    ._= =}       :          or (at your option) any later version.
   .%`+i>       _;_.
   .i_,=:_.      -<s.       This program is distributed in the hope that
    +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
   : ..    .:,     . . .    without even the implied warranty of
   =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;     Library General Public License for more
++=   -.     .`     .:      details.
:     =  ...= . :.=-
-.   .:....=;==+<;          You should have received a copy of the GNU
 -_. . .   )=.  =           Library General Public License along with
   --        :-=`           this library; see the file COPYING.LIB.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

/* QT */
#include <qwindowsystem_qws.h>
#include <qvaluelist.h>

namespace Opie {
namespace Core {
    class ODevice;
namespace Internal {
    class iPAQ;
    class SIMpad;
}

/**
 * A singleton which will manage all possible keyboard filters inside opie.
 * It makes sure that key handlers of odevice are checked first than the
 * keyfilters of software.
 * @short a keyfilter proxy
 * @see QWSServer::KeyboardFilter
 * @author Rajko Albrecht
 * @version 1.0
 */
class OKeyFilter:public QWSServer::KeyboardFilter
{
    friend class Opie::Core::ODevice;
    friend class Opie::Core::Internal::iPAQ;
    friend class Opie::Core::Internal::SIMpad;

    static QValueList<QWSServer::KeyboardFilter*> filterList;
    static QValueList<QWSServer::KeyboardFilter*> preFilterList;

    OKeyFilter();
    OKeyFilter(const OKeyFilter&):QWSServer::KeyboardFilter(){};

protected:
    void addPreHandler(QWSServer::KeyboardFilter*);
    void remPreHandler(QWSServer::KeyboardFilter*);

public:
    virtual ~OKeyFilter();
    virtual bool filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );

    void addHandler(QWSServer::KeyboardFilter*);
    void remHandler(QWSServer::KeyboardFilter*);

    static OKeyFilter*inst();
};

}
}
