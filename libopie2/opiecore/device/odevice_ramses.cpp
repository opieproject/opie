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

struct r_button ramses_buttons [] = {
    { Model_Ramses_MNCI,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Menu Button"),
    "devicebuttons/z_menu",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Model_Ramses_MNCI,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/ipaq_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
};

void Ramses::init()
{
    d->m_vendorstr = "M und N";
    d->m_vendor = Vendor_MundN;

    QFile f("/proc/sys/board/ramses");

    d->m_modelstr = "Ramses";
    d->m_model = Model_Ramses_MNCI;

    d->m_rotation = Rot0;
    d->m_holdtime = 1000;

    f.setName("/etc/oz_version");

    if (f.open(IO_ReadOnly)) {
        d->m_systemstr = "OpenEmbedded/Ramses";
            d->m_system = System_OpenZaurus;

        QTextStream ts(&f);
        ts.setDevice(&f);
        d->m_sysverstr = ts.readLine();
        f.close();
    }

    m_power_timer = 0;

#ifdef QT_QWS_ALLOW_OVERCLOCK
#warning *** Overclocking enabled - this may fry your hardware - you have been warned ***
#define OC(x...)        x
#else
#define OC(x...)
#endif

    // This table is true for a Intel XScale PXA 255

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

bool Ramses::filter(int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
        Q_UNUSED( keycode );
        Q_UNUSED( modifiers );
        Q_UNUSED( isPress );
        Q_UNUSED( autoRepeat );
    return false;
}

void Ramses::timerEvent(QTimerEvent *)
{
    killTimer(m_power_timer);
    m_power_timer = 0;
    QWSServer::sendKeyEvent(-1, HardKey_Backlight, 0, true, false);
    QWSServer::sendKeyEvent(-1, HardKey_Backlight, 0, false, false);
}


bool Ramses::setSoftSuspend(bool soft)
{
    qDebug("Ramses::setSoftSuspend(%d)", soft);
#if 0
    bool res = false;
    int fd;

    if (((fd = ::open("/dev/apm_bios", O_RDWR)) >= 0) ||
        ((fd = ::open("/dev/misc/apm_bios",O_RDWR)) >= 0)) {

        int sources = ::ioctl(fd, APM_IOCGEVTSRC, 0); // get current event sources

        if (sources >= 0) {
            if (soft)
                sources &= ~APM_EVT_POWER_BUTTON;
            else
                sources |= APM_EVT_POWER_BUTTON;

            if (::ioctl(fd, APM_IOCSEVTSRC, sources) >= 0) // set new event sources
                res = true;
            else
                perror("APM_IOCGEVTSRC");
        }
        else
            perror("APM_IOCGEVTSRC");

        ::close(fd);
    }
    else
        perror("/dev/apm_bios or /dev/misc/apm_bios");

    return res;
#else
    return true;
#endif
}

bool Ramses::suspend()
{
    qDebug("Ramses::suspend");
    return false;
}

/**
* This sets the display on or off
*/
bool Ramses::setDisplayStatus(bool on)
{
    qDebug("Ramses::setDisplayStatus(%d)", on);
#if 0
    bool res = false;
    int fd;

    if ((fd = ::open ("/dev/fb/0", O_RDWR)) >= 0) {
        res = (::ioctl(fd, FBIOBLANK, on ? VESA_NO_BLANKING : VESA_POWERDOWN) == 0);
        ::close(fd);
    }
    return res;
#else
    return true;
#endif
}


/*
* We get something between 0..255 into us
*/
bool Ramses::setDisplayBrightness(int bright)
{
    qDebug("Ramses::setDisplayBrightness(%d)", bright);
    bool res = false;
    int fd;

    // pwm1 brighness: 20 steps 500..0 (dunkel->hell)

    if (bright > 255 )
        bright = 255;
    if (bright < 0)
        bright = 0;

    // Turn backlight completely off
    if ((fd = ::open("/proc/sys/board/lcd_backlight", O_WRONLY)) >= 0) {
        char writeCommand[10];
        const int count = sprintf(writeCommand, "%d\n", bright ? 1 : 0);
        res = (::write(fd, writeCommand, count) != -1);
        ::close(fd);
    }

    // scale backlight brightness to hardware
    bright = 500-(bright * 500 / 255);
    if ((fd = ::open("/proc/sys/board/pwm1", O_WRONLY)) >= 0) {
        qDebug(" %d ->pwm1", bright);
        char writeCommand[100];
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

bool Ramses::setDisplayContrast(int contr)
{
    qDebug("Ramses::setDisplayContrast(%d)", contr);
    bool res = false;
    int fd;

    // pwm0 contrast: 20 steps 79..90 (dunkel->hell)

    if (contr > 255 )
        contr = 255;
    if (contr < 0)
        contr = 0;
    contr = 90 - (contr * 20 / 255);

    if ((fd = ::open("/proc/sys/board/pwm0", O_WRONLY)) >= 0) {
        qDebug(" %d ->pwm0", contr);
        char writeCommand[100];
        const int count = sprintf(writeCommand, "%d\n", contr);
        res = (::write(fd, writeCommand, count) != -1);
        res = true;
        ::close(fd);
    }
    return res;
}


int Ramses::displayContrastResolution() const
{
    return 20;
}

