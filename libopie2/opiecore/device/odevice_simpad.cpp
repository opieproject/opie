/*
                             This file is part of the Opie Project
             =.              (C) 2002-2005 The Opie Team <opie-devel@handhelds.org>
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

#include "odevice_simpad.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>
#include <qwidgetlist.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie2/okeyfilter.h>
#include <opie2/oresource.h>

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

namespace Opie {
namespace Core {
namespace Internal {
namespace {
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
//FIXME: buttons have changed to use evdev now
static struct s_button simpad_buttons [] = {
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Lower+Up"),
    "devicebuttons/simpad_lower_up",
    "datebook", "nextView()",
    "today", "raise()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Lower+Down"),
    "devicebuttons/simpad_lower_down",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Lower+Right"),
    "devicebuttons/simpad_lower_right",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Lower+Left"),
    "devicebuttons/simpad_lower_left",
    "opiemail", "raise()",
    "opiemail", "newMail()" },

    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F5, QT_TRANSLATE_NOOP("Button", "Upper+Up"),
    "devicebuttons/simpad_upper_up",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F6, QT_TRANSLATE_NOOP("Button", "Upper+Down"),
    "devicebuttons/simpad_upper_down",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F7, QT_TRANSLATE_NOOP("Button", "Upper+Right"),
    "devicebuttons/simpad_upper_right",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Upper+Left"),
    "devicebuttons/simpad_upper_left",
    "QPE/Rotation", "flip()",
    "QPE/Rotation", "flip()" },
    /*
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Lower+Upper"),
    "devicebuttons/simpad_lower_upper",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Model_SIMpad_CL4 | Model_SIMpad_SL4 | Model_SIMpad_SLC | Model_SIMpad_TSinus,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Lower+Upper"),
    "devicebuttons/simpad_upper_lower",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    */
};
}


void SIMpad::init(const QString&)
{
    d->m_vendorstr = "SIEMENS";
    d->m_vendor = Vendor_SIEMENS;


    //TODO Implement model checking
    //FIXME For now we assume an SL4

    d->m_modelstr = "SL4";
    d->m_model = Model_SIMpad_SL4;

    switch ( d->m_model ) {
        default:
            d->m_rotation = Rot0;
            d->m_direction = CCW;
            d->m_holdtime = 1000; // 1000ms

            break;
    }

    //Distribution detecting code is now in base class
    m_power_timer = 0;

}

void SIMpad::initButtons()
{
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList <ODeviceButton>;

    uint size = sizeof(simpad_buttons)/sizeof(s_button);
    for ( uint i = 0; i < size; i++ ) {
        s_button *sb = simpad_buttons + i;
        ODeviceButton b;

        if (( sb->model & d->m_model ) == d->m_model ) {
            b. setKeycode ( sb->code );
            b. setUserText ( QObject::tr ( "Button", sb->utext ));
            b. setPixmap ( OResource::loadPixmap ( sb->pix ));
            b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( sb->fpressedservice ), sb->fpressedaction ));
            b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( sb->fheldservice ), sb->fheldaction ));

            d->m_buttons->append ( b );
        }
    }
    reloadButtonMapping();
}

/*
 * The SIMpad exposes ChipSelect3 to userspace
 * via a sysfs filesystem files. Using this register
 * one can toggle power of serial, irda, dect circuits
 * change the video driver and display status and
 * many more things.
 *
 * FIXME: I only implemented display_on maybe someone want to
 * have other parts available?
 */

// SIMpad display switch
#define SIMPAD_DISPLAY_ON_CONTROLL "/sys/class/simpad/latch_cs3/display_on"

//SIMpad touchscreen backlight strength control
#define SIMPAD_BACKLIGHT_CONTROL "/sys/class/backlight/simpad-mq200-bl/brightness"

/*
 * Since LED1 is for charging only and LED2 is used by to show power on/off and we should not change the
 * LEDs I decided to remove this code from odevice_simpad.cpp. If you still want to get the led states
 * you will need to use /sys/class/simpad/latch_cs3/ for it.
 *
 */

void SIMpad::timerEvent ( QTimerEvent * )
{
    killTimer ( m_power_timer );
    m_power_timer = 0;
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, true, false );
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, false, false );
}


bool SIMpad::suspend() // Must override because SIMpad does NOT have apm
{
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    bool res  = OAbstractMobileDevice::suspend();

    /*
     * restore the screen content if we really
     * supended the device
     */
    if ( res )
        updateAllWidgets();

    return res;
}

bool SIMpad::setDisplayStatus ( bool on )
{
    qDebug( "ODevice for SIMpad: setDisplayStatus(%s)", on? "on" : "off" );
    bool res = false;
    int fd;
    int status=static_cast<int> (on);

    if (( fd = ::open ( SIMPAD_DISPLAY_ON_CONTROLL, O_WRONLY )) >= 0 ) {
        QCString str = QFile::encodeName( QString::number(status));
        res = ( ::write(fd, str, str.length()) != -1 );
        ::close ( fd );
    }
    return res;
}


bool SIMpad::setDisplayBrightness ( int bright )
{
    qDebug( "ODevice for SIMpad: setDisplayBrightness( %d )", bright );
    bool res = false;
    int fd;

    if ( bright > 254 )
        bright = 254;
    if ( bright < 1 )
        bright = 0;

    if (( fd = ::open ( SIMPAD_BACKLIGHT_CONTROL, O_WRONLY )) >= 0 ) {
        QCString str = QFile::encodeName( QString::number(bright));
        res = ( ::write(fd, str, str.length()) != -1 );
        ::close ( fd );
    }
    return res;
}


int SIMpad::displayBrightnessResolution() const
{
    return 254; // All SIMpad models share the same display
}


/*
 * The MQ200 DRAM content is lost during suspend
 * so we will just repaint every widget on resume
 */
void SIMpad::updateAllWidgets() {
    QWidgetList *list = QApplication::allWidgets();
    QWidgetListIt it( *list );
    QWidget *wid;

    while ((wid=it.current()) != 0 ) {
        wid->update();
        ++it;
    }

    delete list;
}

}
}
}
