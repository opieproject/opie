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

#include "odevice_jornada.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>

/* STD */
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif

/* KERNEL */
#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

typedef struct {
  unsigned char OffOnBlink;       /* 0=off 1=on 2=Blink */
  unsigned char TotalTime;        /* Units of 5 seconds */
  unsigned char OnTime;           /* units of 100m/s */
  unsigned char OffTime;          /* units of 100m/s */
} LED_IN;

typedef struct {
        unsigned char mode;
        unsigned char pwr;
        unsigned char brightness;
} FLITE_IN;

#define LED_ON    OD_IOW( 'f', 5, LED_IN )
#define FLITE_ON  OD_IOW( 'f', 7, FLITE_IN )

using namespace Opie::Core::Internal;

void Jornada::init(const QString&)
{
    d->m_vendorstr = "HP";
    d->m_vendor = Vendor_HP;
    d->m_modelstr = "Jornada 56x";
    d->m_model = Model_Jornada_56x;
    d->m_systemstr = "Familiar";
    d->m_system = System_Familiar;
    d->m_rotation = Rot0;

    QFile f ( "/etc/familiar-version" );
    f.setName ( "/etc/familiar-version" );
    if ( f.open ( IO_ReadOnly )) {

        QTextStream ts ( &f );
        d->m_sysverstr = ts.readLine().mid( 10 );

        f. close();
    }
}


int Jornada::displayBrightnessResolution() const
{
    return 0;
}


bool Jornada::setDisplayBrightness( int bright )
{
    bool res = false;
    int fd;

    if ( bright > 255 )
        bright = 255;
    if ( bright < 0 )
        bright = 0;

    if (( fd = ::open ( "/dev/touchscreen/0", O_WRONLY )) >= 0 ) {
        FLITE_IN bl;
        bl. mode = 1;
        bl. pwr = bright ? 1 : 0;
        bl. brightness = ( bright * ( displayBrightnessResolution() - 1 ) + 127 ) / 255;
        res = ( ::ioctl ( fd, FLITE_ON, &bl ) == 0 );
        ::close ( fd );
    }
    return res;
}


bool Jornada::setSoftSuspend( bool soft )
{
    bool res = false;
    int fd;

    if (( fd = ::open ( "/proc/sys/ts/suspend_button_mode", O_WRONLY )) >= 0 ) {
        if ( ::write ( fd, soft ? "1" : "0", 1 ) == 1 )
            res = true;
        else
            ::perror ( "write to /proc/sys/ts/suspend_button_mode" );

        ::close ( fd );
    }
    else
        ::perror ( "/proc/sys/ts/suspend_button_mode" );

    return res;
}

