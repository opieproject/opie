/*
                             This file is part of the Opie Project
                             Copyright (C) The Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
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

#ifndef ODEVICE_RAMSES
#define ODEVICE_RAMSES

#include <opie2/odevice.h>
/* QT */
#include <qwindowsystem_qws.h>

namespace Opie {
namespace Core {
namespace Internal {

class Ramses : public ODevice, public QWSServer::KeyboardFilter
{
  protected:
    virtual void init(const QString&);

  public:
    virtual bool setSoftSuspend( bool soft );
    virtual bool suspend();

    virtual bool setDisplayStatus( bool on );
    virtual bool setDisplayBrightness( int b );
    virtual int displayBrightnessResolution() const;
    virtual bool setDisplayContrast( int b );
    virtual int displayContrastResolution() const;

  protected:
    virtual bool filter ( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
    virtual void timerEvent ( QTimerEvent *te );

    int m_power_timer;
};

struct r_button {
    uint model;
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
};
}
}
}

#endif
