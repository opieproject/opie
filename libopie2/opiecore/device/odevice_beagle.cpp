/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 The Opie Team <opie-devel@handhelds.org>
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

#include <qpe/resource.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>



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

#define FL_MAJOR   60
#define FL_NAME   "sa1100-fl"
#define FL_FILE   "/dev/sa1100-fl"

namespace Opie {
namespace Core {
namespace Internal {

struct b_button {
    uint model;
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
};

struct b_button beagle_buttons [] = {
    { Model_Beagle_PA100,
      Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
      "devicebuttons/beagle_calendar",
      "datebook", "nextView()",
      "today", "raise()" },
    { Model_Beagle_PA100,
      Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
      "devicebuttons/beagle_contact",
      "addressbook", "raise()",
      "addressbook", "beamBusinessCard()" },
    { Model_Beagle_PA100,
      Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Todo Button"),
      "devicebuttons/beagle_todo",
      "todolist", "raise()",
      "QPE/TaskBar", "toggleMenu()" },
    { Model_Beagle_PA100,
      Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
      "devicebuttons/beagle_home",
      "QPE/Launcher", "home()",
      "buttonsettings", "raise()" },
};


Beagle::Beagle()
{
    qWarning( "Created Beagle" );
}
Beagle::~Beagle() {}

void Beagle::init( const QString&) {
    /*
     * No other assabat model yet
     */
    d->m_vendorstr = "Tradesquare.NL";
    d->m_vendor = Vendor_MasterIA;
    d->m_modelstr = "Tuxpda 1";
    d->m_rotation = Rot0;
}

void Beagle::initButtons() {
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList<ODeviceButton>;
    uint length = sizeof( beagle_buttons )/ sizeof( b_button );
    for ( uint i = 0; i < length; ++i ) {
        b_button *bb = &beagle_buttons[i];
        ODeviceButton b;
        b.setKeycode( bb->code );
        b.setUserText( QObject::tr( "Button", bb->utext ) );
        b.setPixmap( Resource::loadPixmap( bb->pix ) );
        b.setFactoryPresetPressedAction( OQCopMessage( makeChannel( bb->fpressedservice ), bb->fpressedaction ) );
        b.setFactoryPresetHeldAction( OQCopMessage( makeChannel( bb->fheldservice ), bb->fheldaction ) );
        d->m_buttons->append( b );
    }

    reloadButtonMapping();
}

bool Beagle::setDisplayStatus( bool on ) {
    int fd = ::open(FL_FILE, O_WRONLY);

    if ( fd < 0 )
        return false;

    return ( ::ioctl(fd, on ? _SA1100_FL_IOCTL_ON : _SA1100_FL_IOCTL_OFF, 0 ) == -1 );
}

int Beagle::displayBrightnessResolution()const {
    return 100;
}

bool Beagle::setDisplayBrightness( int brightness ) {
    int fd = ::open(FL_FILE, O_WRONLY);

    if ( fd < 0 )
        return false;

    return ( ::ioctl(fd, _SA1100_FL_IOCTL_INTENSITY, brightness%101 ) == 0 );
}

}
}
}
