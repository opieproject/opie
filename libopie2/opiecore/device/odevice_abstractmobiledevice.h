/*
                     This file is part of the Opie Project
                      Copyright (C) 2004, 2005 Holger Hans Peter Freyther <freyther@handhelds.org>
                             Copyright (C) 2004, 2005 Michael 'mickey' Lauer <mickeyl@handhelds.org>


              =.
            .=l.
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU Library General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      Library General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           Library General Public License along with
  --    :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OPIE_CORE_DEVICE_ABSTRACT_MOBILE_DEVICE
#define OPIE_CORE_DEVICE_ABSTRACT_MOBILE_DEVICE

#include "odevice.h"

namespace Opie {
namespace Core {
/**
 * @short Common Implementations for Linux Handheld Devices
 *
 * Abstract Class with implementation for suspending using
 * asynchrnonus apm implementations and displaystatus using
 * the Linux Frame Buffer API
 *
 */
class OAbstractMobileDevice : public ODevice {
    Q_OBJECT
protected:
    OAbstractMobileDevice();
    void setAPMTimeOut( int time );
public:
    virtual bool suspend();
    virtual bool setDisplayStatus(bool);

protected:
    int m_timeOut;
};
}
}


#endif
