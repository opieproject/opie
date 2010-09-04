/*
                             This file is part of the Opie Project
              =.             (C) 2006 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "odevice_motorola_ezx.h"

/* OPIE */
#include <opie2/oinputsystem.h>
#include <opie2/oresource.h>

#include <qpe/config.h>
#include <qpe/sound.h>

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>
#include <qcopchannel_qws.h>

/* STD */
#include <assert.h>
#include <string.h>
#include <errno.h>
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

struct ezx_button ezx_buttons [] = {
    { Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
    "devicebuttons/z_calendar",
    "datebook", "nextView()",
    "today", "raise()" },
    { Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
    "devicebuttons/z_contact",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/z_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Menu Button"),
    "devicebuttons/z_menu",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/z_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
};

void Motorola_EZX::init(const QString& cpu_info)
{
    qDebug( "Motorola_EZX::init()" );

    // No need to wait after apm --suspend
    setAPMTimeOut( 0 );

    d->m_vendorstr = "OpenEZX Team";
    d->m_systemstr = "OpenEZX";
    d->m_system = System_OpenEZX;
    // sysver already gathered

    // check the Motorola_EZX model
    QString model;
    int loc = cpu_info.find( ":" );
    if ( loc != -1 )
        model = cpu_info.mid( loc+2 ).simplifyWhiteSpace();
    else
        model = cpu_info;

    if ( model == "Motorola Ezx Platform" ) {
        d->m_model = Model_Motorola_EZX;
        d->m_modelstr = "Motorola_EZX";
    } else assert( 0 );

    // set path to backlight device in kernel 2.6
    m_backlightdev = "/sys/class/backlight/ezx-bl/";
    d->m_rotation = Rot0;
    //initHingeSensor();

    // set default qte driver and led state
    d->m_qteDriver = "Transformed";
    m_leds[0] = Led_Off;

    qDebug( "Motorola_EZX::init() - Using the 2.6 OpenEZX HAL on a %s", (const char*) d->m_modelstr );
}

void Motorola_EZX::initButtons()
{
    qDebug( "Motorola_EZX::initButtons()" );
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList <ODeviceButton>;
    reloadButtonMapping();
}

QValueList <OLed> Motorola_EZX::ledList() const
{
    QValueList <OLed> vl;
    vl << Led_Mail;
    return vl;
}

QValueList <OLedState> Motorola_EZX::ledStateList( OLed l ) const
{
    QValueList <OLedState> vl;

    if ( l == Led_Mail )
        vl << Led_Off << Led_On << Led_BlinkSlow;
    return vl;
}

OLedState Motorola_EZX::ledState( OLed which ) const
{
    if ( which == Led_Mail )
        return m_leds [0];
    else
        return Led_Off;
}

bool Motorola_EZX::setLedState( OLed, OLedState )
{
     // Currently not supported
    qDebug( "Motorola_EZX::setLedState: ODevice handling not yet implemented" );
    return false;
}

int Motorola_EZX::displayBrightnessResolution() const
{
    int res = 1;
    int fd = ::open( m_backlightdev + "max_brightness", O_RDONLY|O_NONBLOCK );
    if ( fd >= 0)
    {
        char buf[100];
        if ( ::read( fd, &buf[0], sizeof buf ) ) ::sscanf( &buf[0], "%d", &res );
        ::close( fd );
    }
    return res;
}

bool Motorola_EZX::setDisplayBrightness( int bright )
{
    return false;
}

bool Motorola_EZX::setDisplayStatus( bool on )
{
    bool res = false;
    int fd = ::open( m_backlightdev + "power", O_WRONLY|O_NONBLOCK );
    if ( fd >= 0 )
    {
        char buf[10];
        buf[0] = on ? FB_BLANK_UNBLANK : FB_BLANK_POWERDOWN;
        buf[1] = '\0';
        res = ( ::write( fd, &buf[0], 2 ) == 0 );
        ::close( fd );
    }
    return res;
}

void Motorola_EZX::systemMessage( const QCString &msg, const QByteArray & )
{
    if ( msg == "deviceButtonMappingChanged()" ) {
        reloadButtonMapping();
    }
}
