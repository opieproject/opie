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

#include "odevice_ramses.h"

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

using namespace Opie::Core;
using namespace Opie::Core::Internal;


void Ramses::init(const QString&)
{
    d->m_vendorstr = "M und N";
    d->m_vendor = Vendor_MundN;

    // with old Prototype-LCD, /proc/sys/board/lcd_type = 1
    //d->m_modelstr = "Ramses";
    //d->m_model = Model_Ramses_MNCI;
    //d->m_rotation = Rot0;

    // all current Hardware, with /proc/sys/board/lcd_type = 2
    d->m_modelstr = "MNCIRX";
    d->m_model = Model_Ramses_MNCIRX;
    d->m_rotation = Rot90;

    d->m_holdtime = 500;


    // This table is true for a Intel XScale PXA 255

#ifdef QT_QWS_ALLOW_OVERCLOCK
#define OC(x...) x
#else
#define OC(x...)
#endif

    d->m_cpu_frequencies->append("99000");      // mem= 99, run= 99, turbo= 99, PXbus= 50
 OC(d->m_cpu_frequencies->append("118000"); )   // mem=118, run=118, turbo=118, PXbus= 59 OC'd mem
    d->m_cpu_frequencies->append("199100");     // mem= 99, run=199, turbo=199, PXbus= 99
 OC(d->m_cpu_frequencies->append("236000"); )   // mem=118, run=236, turbo=236, PXbus=118 OC'd mem
    d->m_cpu_frequencies->append("298600");     // mem= 99, run=199, turbo=298, PXbus= 99
 OC(d->m_cpu_frequencies->append("354000"); )   // mem=118, run=236, turbo=354, PXbus=118 OC'd mem
    d->m_cpu_frequencies->append("398099");     // mem= 99, run=199, turbo=398, PXbus= 99
    d->m_cpu_frequencies->append("398100");     // mem= 99, run=398, turbo=398, PXbus=196
 OC(d->m_cpu_frequencies->append("471000"); )   // mem=118, run=471, turbo=471, PXbus=236 OC'd mem/core/bus
}


void Ramses::playAlarmSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "alarm" );

    changeMixerForAlarm( 0, "/dev/sound/mixer" , &snd);
    snd.play();
#else
#error QT_NO_SOUND defined
#endif
}


bool Ramses::suspend()
{
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    sendSuspendmsg();
    ::sync();

    int fd;
    if ((fd = ::open("/proc/sys/pm/suspend", O_WRONLY)) >= 0) {
        char writeCommand[] = "1\n";
        ::write(fd, writeCommand, sizeof(writeCommand) );
        ::close(fd);
    }
    
    ::usleep ( 200 * 1000 );
    return true;
}


bool Ramses::setDisplayBrightness(int bright)
{
    //qDebug("Ramses::setDisplayBrightness(%d)", bright);
    bool res = false;
    int fd;

    // pwm1 brighness: 20 steps 500..0 (dunkel->hell)

    if (bright > 255 )
        bright = 255;
    if (bright < 0)
        bright = 0;

    // Turn backlight completely off if brightness=0
    if ((fd = ::open("/proc/sys/board/lcd_backlight", O_WRONLY)) >= 0) {
        char writeCommand[10];
        const int count = sprintf(writeCommand, "%d\n", bright ? 1 : 0);
        res = (::write(fd, writeCommand, count) != -1);
        ::close(fd);
    }

    if ((fd = ::open("/proc/sys/board/lcd_brightness", O_WRONLY)) >= 0) {
        char writeCommand[10];
        const int count = sprintf(writeCommand, "%d\n", bright);
        res = (::write(fd, writeCommand, count) != -1);
        ::close(fd);
    }
    return res;
}


int Ramses::displayBrightnessResolution() const
{
    return 32;
}


// TODO: add displayContrast for old MNCI
