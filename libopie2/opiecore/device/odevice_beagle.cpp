/*
                             This file is part of the Opie Project

                             Copyright (C)2004 The Opie Team <opie-devel@lists.sourceforge.net>
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

#include "odevicebutton.h"
#include "odevice_beagle.h"

#include <opie2/oresource.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>


/*
 * Intel Assabat FrontLight Control
 */
#define _SA1100_FL_IOCTL_ON		1
#define _SA1100_FL_IOCTL_OFF		2
#define _SA1100_FL_IOCTL_INTENSITY	3
#define _SA1100_FL_IOCTL_BACKLIGHT  4
#define _SA1100_FL_IOCTL_CONTRAST	5
#define _SA1100_FL_IOCTL_GET_BACKLIGHT  6
#define _SA1100_FL_IOCTL_GET_CONTRAST  7
// added by Sean Hsieh
#define _SA1100_FL_IOCTL_PWR_TOGGLE		8
#define _SA1100_FL_IOCTL_AUTOLIGHT		10

/*
 * The device
 */
#define FL_MAJOR   60
#define FL_NAME   "sa1100-fl"
#define FL_FILE   "/dev/sa1100-fl"

namespace Opie {
namespace Core {
namespace Internal {

Beagle::Beagle()  {}

Beagle::~Beagle() {}

/*
 * Simply set the Tradesquare.NL data
 */
void Beagle::init( const QString&) {
    /*
     * No other assabat model yet
     */
    d->m_vendorstr = "Tradesquare.NL";
    d->m_vendor = Vendor_MasterIA;
    d->m_modelstr = "Tuxpda 1";
    d->m_rotation = Rot0;
    d->m_model = Model_Beagle_PA100;
}


/*
 * Turn the display on. We do it by ioctl on FL_FILE
 */
bool Beagle::setDisplayStatus( bool on ) {
    int fd = ::open(FL_FILE, O_WRONLY);

    if ( fd < 0 )
        return false;

    return ( ::ioctl(fd, on ? _SA1100_FL_IOCTL_ON : _SA1100_FL_IOCTL_OFF, 0 ) == 0 );
}

/*
 * 0-100 are legal steps of the frontlight.
 */
int Beagle::displayBrightnessResolution()const {
    return 100;
}

/*
 * Opie uses the values 0-255 for the frontlight
 * intensity and we need to map it to the range
 * of 0-100.
 * But first we do some sanity of the range of brightness
 *
 */
bool Beagle::setDisplayBrightness( int brightness ) {
    if ( brightness > 255 )
        brightness = 255;
    else if ( brightness < 0 )
        brightness = 0;
    brightness = (100*brightness)/255;

    int fd = ::open(FL_FILE, O_WRONLY);

    if ( fd < 0 )
        return false;

    return ( ::ioctl(fd, _SA1100_FL_IOCTL_INTENSITY, brightness ) == 0 );
}

}
}
}
