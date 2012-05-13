/*
                             This file is part of the Opie Project
                             Copyright (C) 2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
              =.             Copyright (C) 2002-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
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

#include "odevice_abstractmobiledevice.h"

/* QT */
#include <qfile.h>
#include <qwindowsystem_qws.h>

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

namespace Opie {
namespace Core {
namespace Internal {

class Zaurus : public OAbstractMobileDevice, public QWSServer::KeyboardFilter
{
  Q_OBJECT

  protected:
    virtual void init(const QString&);
    virtual void initButtons();
    void initButtonCombos();
    void initHingeSensor();

  protected slots:
    void hingeSensorTriggered();
    void systemMessage( const QCString &msg, const QByteArray & );

  public:
    virtual bool hasWaveSpeaker() const;
    virtual void playAlarmSound();
    virtual void playKeySound();
    virtual void playTouchSound();

    virtual bool hasHingeSensor() const;
    virtual OHingeStatus readHingeSensor() const;

    virtual Transformation rotation() const;
    virtual ODirection direction() const;

  protected:
    virtual void buzzer( int snd );
    virtual bool filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );

    QFile m_hinge;
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
