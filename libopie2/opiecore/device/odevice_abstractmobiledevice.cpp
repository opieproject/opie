/*
                             This file is part of the Opie Project
                             Copyright (C) 2004, 2005 Holger Hans Peter Freyther <freyther@handhelds.org>
                             Copyright (C) 2004, 2005 Michael 'mickey' Lauer <mickeyl@handhelds.org>
                             Copyright (C) 2002, 2003 Robert Griebl <sandman@handhelds.org>


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

#include "odevice_abstractmobiledevice.h"

#include <sys/time.h>
#include <sys/ioctl.h>

#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

namespace Opie {
namespace Core {
OAbstractMobileDevice::OAbstractMobileDevice()
    : m_timeOut( 1500 )
{}

/**
 * @short Time to wait for the asynchronos APM implementation to suspend
 *
 * Milli Seconds to wait before returning from the suspend method.
 * This is needed due asynchrnonus implementations of the APM bios.
 *
 */
void OAbstractMobileDevice::setAPMTimeOut( int time ) {
    m_timeOut = time;
}


bool OAbstractMobileDevice::suspend() {
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    bool res = false;
    ODevice::sendSuspendmsg();

    struct timeval tvs, tvn;
    ::gettimeofday ( &tvs, 0 );

    ::sync(); // flush fs caches
    res = ( ::system ( "apm --suspend" ) == 0 );

    // This is needed because some apm implementations are asynchronous and we
    // can not be sure when exactly the device is really suspended
    // This can be deleted as soon as a stable familiar with a synchronous apm implementation exists.

    if ( res ) {
        do { // wait at most 1.5 sec: either suspend didn't work or the device resumed
            ::usleep ( 200 * 1000 );
            ::gettimeofday ( &tvn, 0 );
        } while ((( tvn. tv_sec - tvs. tv_sec ) * 1000 + ( tvn. tv_usec - tvs. tv_usec ) / 1000 ) < m_timeOut );
    }

    return res;
}

//#include <linux/fb.h> better not rely on kernel headers in userspace ...

// _IO and friends are only defined in kernel headers ...
#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))
#define OD_IO(type,number)              OD_IOC(0,type,number,0)

#define FBIOBLANK             OD_IO( 'F', 0x11 ) // 0x4611

/* VESA Blanking Levels */
#define VESA_NO_BLANKING      0
#define VESA_VSYNC_SUSPEND    1
#define VESA_HSYNC_SUSPEND    2
#define VESA_POWERDOWN        3

bool OAbstractMobileDevice::setDisplayStatus ( bool on ) {
    bool res = false;
    int fd;

#ifdef QT_QWS_DEVFS
    if (( fd = ::open ( "/dev/fb/0", O_RDWR )) >= 0 ) {
#else
    if (( fd = ::open ( "/dev/fb0", O_RDWR )) >= 0 ) {
#endif
        res = ( ::ioctl ( fd, FBIOBLANK, on ? VESA_NO_BLANKING : VESA_POWERDOWN ) == 0 );
        ::close ( fd );
    }

    return res;
}
}
}
