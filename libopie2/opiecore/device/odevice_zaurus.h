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

#ifndef ODEVICE_ZAURUS
#define ODEVICE_ZAURUS

#include <opie2/odevice.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

// _IO and friends are only defined in kernel headers ...

#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))


namespace Opie {
namespace Core {
namespace Private {
class Zaurus : public ODevice
{

  protected:
    virtual void init();
    virtual void initButtons();

  public:
    virtual bool setSoftSuspend ( bool soft );

    virtual bool setDisplayBrightness ( int b );
    virtual int displayBrightnessResolution() const;

    virtual void playAlarmSound();
    virtual void playKeySound();
    virtual void playTouchSound();

    virtual QValueList <OLed> ledList() const;
    virtual QValueList <OLedState> ledStateList ( OLed led ) const;
    virtual OLedState ledState( OLed led ) const;
    virtual bool setLedState( OLed led, OLedState st );

    virtual bool hasHingeSensor() const;
    virtual OHingeStatus readHingeSensor();

    virtual bool suspend();
    virtual Transformation rotation() const;
    virtual ODirection direction() const;

  protected:
    virtual void buzzer ( int snd );

    OLedState m_leds [1];
    bool m_embedix;
};

struct z_button {
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
