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

#ifndef ODEVICE_SIMPAD
#define ODEVICE_SIMPAD

#include <opie2/odevice.h>

/* QT */
#include <qwindowsystem_qws.h>

namespace Opie {
namespace Core {
namespace Internal {

class SIMpad : public ODevice, public QWSServer::KeyboardFilter
{
  protected:
    virtual void init();
    virtual void initButtons();

  public:
    virtual bool setSoftSuspend( bool soft );
    virtual bool suspend();

    virtual bool setDisplayStatus( bool on );
    virtual bool setDisplayBrightness( int b );
    virtual int displayBrightnessResolution() const;

    virtual void playAlarmSound();

    virtual QValueList <OLed> ledList() const;
    virtual QValueList <OLedState> ledStateList( OLed led ) const;
    virtual OLedState ledState( OLed led ) const;
    virtual bool setLedState( OLed led, OLedState st );

  protected:
    virtual bool filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
    virtual void timerEvent( QTimerEvent *te );

    int m_power_timer;

    OLedState m_leds [1];
};

struct s_button {
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
