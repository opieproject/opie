/*
                             This file is part of the Opie Project
                             Copyright (C) The Opie Team <opie-devel@handhelds.org>
              =.             Copyright (C) 2003-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

// Audio
#define SHARP_DEV_IOCTL_COMMAND_START 0x5680

#define SHARP_BUZZER_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
#define SHARP_BUZZER_MAKESOUND   (SHARP_BUZZER_IOCTL_START)

#define SHARP_BUZ_TOUCHSOUND       1  /* touch panel sound */
#define SHARP_BUZ_KEYSOUND         2  /* key sound */
#define SHARP_BUZ_SCHEDULE_ALARM  11  /* schedule alarm */

#define SHARP_BUZZER_SETVOLUME   (SHARP_BUZZER_IOCTL_START+1)
#define SHARP_BUZZER_GETVOLUME   (SHARP_BUZZER_IOCTL_START+2)
#define SHARP_BUZZER_ISSUPPORTED (SHARP_BUZZER_IOCTL_START+3)
#define SHARP_BUZZER_SETMUTE     (SHARP_BUZZER_IOCTL_START+4)
#define SHARP_BUZZER_STOPSOUND   (SHARP_BUZZER_IOCTL_START+5)

// LED
#define SHARP_LED_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
#define SHARP_LED_SETSTATUS   (SHARP_LED_IOCTL_START+1)
#define SHARP_LED_MAIL_EXISTS  9     /* mail status (exists or not) */

#define LED_MAIL_NO_UNREAD_MAIL  0   /* for SHARP_LED_MAIL_EXISTS */
#define LED_MAIL_NEWMAIL_EXISTS  1   /* for SHARP_LED_MAIL_EXISTS */
#define LED_MAIL_UNREAD_MAIL_EX  2   /* for SHARP_LED_MAIL_EXISTS */

// Rotation and Power Management
#define SHARP_IOCTL_GET_ROTATION 0x413c

#define APM_IOCGEVTSRC          OD_IOR( 'A', 203, int )
#define APM_IOCSEVTSRC          OD_IORW( 'A', 204, int )
#define APM_EVT_POWER_BUTTON    (1 << 0)

// Brightness
#define SHARP_FL_IOCTL_DEVICE   "/dev/sharp_fl"
#define SHARP_FL_IOCTL_ON                  1
#define SHARP_FL_IOCTL_OFF                 2
#define SHARP_FL_IOCTL_STEP_CONTRAST     100
#define SHARP_FL_IOCTL_GET_STEP_CONTRAST 101
#define SHARP_FL_IOCTL_GET_STEP          102

namespace Opie {
namespace Core {
namespace Internal {
class Zaurus : public ODevice
{
  protected:
    virtual void init(const QString&);
    virtual void initButtons();

  public:
    virtual bool setSoftSuspend( bool soft );

    virtual bool setDisplayBrightness( int b );
    virtual bool setDisplayStatus( bool on );
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
