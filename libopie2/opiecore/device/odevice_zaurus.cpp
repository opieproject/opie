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

#include "odevice_zaurus.h"

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

struct z_button z_buttons [] = {
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

struct z_button z_buttons_c700 [] = {
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
    { Qt::Key_F14, QT_TRANSLATE_NOOP("Button", "Display Rotate"),
    "devicebuttons/z_hinge",
    "QPE/Rotation", "rotateDefault()",
    "QPE/Dummy", "doNothing()" },
};

// FIXME This gets unnecessary complicated. We should think about splitting the Zaurus
//       class up into individual classes. We need three classes
//
//       Zaurus-Collie (SA-model  w/ 320x240 lcd, for SL5500 and SL5000)
//       Zaurus-Poodle (PXA-model w/ 320x240 lcd, for SL5600)
//       Zaurus-Corgi  (PXA-model w/ 640x480 lcd, for C700, C750, C760, C860, C3000)
//       Zaurus-Tosa   (PXA-model w/ 480x640 lcd, for SL6000)
//
//       Only question right now is: Do we really need to do it? Because as soon
//       as the OpenZaurus kernel is ready, there will be a unified interface for all
//       Zaurus models (concerning apm, backlight, buttons, etc.)
//
//       Comments? - mickeyl.

void Zaurus::init(const QString& cpu_info)
{
    // generic distribution code already scanned /etc/issue at that point -
    // embedix releases contain "Embedix <version> | Linux for Embedded Devices"
    if ( d->m_sysverstr.contains( "embedix", false ) )
    {
        d->m_vendorstr = "Sharp";
        d->m_vendor = Vendor_Sharp;
        d->m_systemstr = "Zaurus";
        d->m_system = System_Zaurus;
        m_embedix = true;
    }
    else
    {
        d->m_vendorstr = "OpenZaurus Team";
        d->m_systemstr = "OpenZaurus";
        d->m_system = System_OpenZaurus;
        // sysver already gathered

        // OpenZaurus sometimes uses the embedix kernel, check if this is one
        FILE *uname = popen("uname -r", "r");
        QFile f;
        QString line;
        if ( f.open(IO_ReadOnly, uname) ) {
            QTextStream ts ( &f );
            line = ts. readLine();
            int loc = line. find ( "embedix" );
            if ( loc != -1 )
                m_embedix = true;
            else
                m_embedix = false;
            f.close();
        }
        pclose(uname);
    }

    // check the Zaurus model
    QString model;
    int loc = cpu_info.find( ":" );
    if ( loc != -1 )
        model = cpu_info.mid( loc+2 ).simplifyWhiteSpace();
    else
        model = cpu_info;

    if ( model == "SHARP Corgi" ) {
        d->m_model = Model_Zaurus_SLC7x0;
        d->m_modelstr = "Zaurus SL-C700";
    } else if ( model == "SHARP Shepherd" ) {
        d->m_model = Model_Zaurus_SLC7x0;
        d->m_modelstr = "Zaurus SL-C750";
    } else if ( model == "SHARP Husky" ) {
        d->m_model = Model_Zaurus_SLC7x0;
        d->m_modelstr = "Zaurus SL-C760 or SL-C860";
    } else if ( model == "SHARP Poodle" ) {
        d->m_model = Model_Zaurus_SLB600;
        d->m_modelstr = "Zaurus SL-B500 or SL-5600";
    } else if ( model == "Sharp-Collie" || model == "Collie" ) {
        d->m_model = Model_Zaurus_SL5500;
        d->m_modelstr = "Zaurus SL-5500 or SL-5000d";
    } else if ( model == "SHARP Tosa" ) {
        d->m_model = Model_Zaurus_SL6000;
        d->m_modelstr = "Zaurus SL-6000";
    } else {
        d->m_model = Model_Zaurus_SL5500;
        d->m_modelstr = "Unkown Zaurus";
    }

    // set initial rotation
    switch( d->m_model ) {
        case Model_Zaurus_SL6000: // fallthrough
        case Model_Zaurus_SLA300:
            d->m_rotation = Rot0;
            break;
        case Model_Zaurus_SLC7x0:
            d->m_rotation = rotation();
            d->m_direction = direction();
            break;
        case Model_Zaurus_SLB600: // fallthrough
        case Model_Zaurus_SL5500: // fallthrough
        case Model_Zaurus_SL5000:
        default:
            d->m_rotation = Rot270;
            break;
    }
    m_leds [0] = Led_Off;
}

void Zaurus::initButtons()
{
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList <ODeviceButton>;

    struct z_button * pz_buttons;
    int buttoncount;
    switch ( d->m_model ) {
        case Model_Zaurus_SLC7x0:
            pz_buttons = z_buttons_c700;
            buttoncount = ARRAY_SIZE(z_buttons_c700);
            break;
        default:
            pz_buttons = z_buttons;
            buttoncount = ARRAY_SIZE(z_buttons);
            break;
    }

    for ( int i = 0; i < buttoncount; i++ ) {
        struct z_button *zb = pz_buttons + i;
        ODeviceButton b;

        b. setKeycode ( zb->code );
        b. setUserText ( QObject::tr ( "Button", zb->utext ));
        b. setPixmap ( Resource::loadPixmap ( zb->pix ));
        b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( zb->fpressedservice ),
                                zb->fpressedaction ));
        b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( zb->fheldservice ),
                                zb->fheldaction ));

        d->m_buttons->append ( b );
    }

    reloadButtonMapping();
}



typedef struct sharp_led_status {
    int which;   /* select which LED status is wanted. */
    int status;  /* set new led status if you call SHARP_LED_SETSTATUS */
} sharp_led_status;

void Zaurus::buzzer( int sound )
{
#ifndef QT_NO_SOUND
    Sound *snd = 0;

    // Not all devices have real sound
    if ( d->m_model == Model_Zaurus_SLC7x0
      || d->m_model == Model_Zaurus_SLB600 
      || d->m_model == Model_Zaurus_SL6000 ) {

        switch ( sound ){
        case SHARP_BUZ_TOUCHSOUND: {
            static Sound touch_sound("touchsound");
            snd = &touch_sound;
	}
            break;
        case SHARP_BUZ_KEYSOUND: {
            static Sound key_sound( "keysound" );
            snd = &key_sound;
	}
            break;
        case SHARP_BUZ_SCHEDULE_ALARM:
        default: {
            static Sound alarm_sound("alarm");
            snd = &alarm_sound;
	}	    
            break;
        }
    }

    // If a soundname is defined, we expect that this device has
    // sound capabilities.. Otherwise we expect to have the buzzer
    // device..
    if ( snd && snd->isFinished() ){
        changeMixerForAlarm( 0, "/dev/sound/mixer", snd );
        snd->play();
    } else if( !snd ) {
        int fd = ::open ( "/dev/sharp_buz", O_WRONLY|O_NONBLOCK );

        if ( fd >= 0 ) {
            ::ioctl ( fd, SHARP_BUZZER_MAKESOUND, sound );
            ::close ( fd );
        }

    }
#endif
}


void Zaurus::playAlarmSound()
{
    buzzer( SHARP_BUZ_SCHEDULE_ALARM );
}

void Zaurus::playTouchSound()
{
    buzzer( SHARP_BUZ_TOUCHSOUND );
}

void Zaurus::playKeySound()
{
    buzzer( SHARP_BUZ_KEYSOUND );
}


QValueList <OLed> Zaurus::ledList() const
{
    QValueList <OLed> vl;
    vl << Led_Mail;
    return vl;
}

QValueList <OLedState> Zaurus::ledStateList ( OLed l ) const
{
    QValueList <OLedState> vl;

    if ( l == Led_Mail )
        vl << Led_Off << Led_On << Led_BlinkSlow;
    return vl;
}

OLedState Zaurus::ledState ( OLed which ) const
{
    if ( which == Led_Mail )
        return m_leds [0];
    else
        return Led_Off;
}

bool Zaurus::setLedState ( OLed which, OLedState st )
{
    if (!m_embedix) // Currently not supported on non_embedix kernels
        return false;

    static int fd = ::open ( "/dev/sharp_led", O_RDWR|O_NONBLOCK );

    if ( which == Led_Mail ) {
        if ( fd >= 0 ) {
            struct sharp_led_status leds;
            ::memset ( &leds, 0, sizeof( leds ));
            leds. which = SHARP_LED_MAIL_EXISTS;
            bool ok = true;

            switch ( st ) {
                case Led_Off      : leds. status = LED_MAIL_NO_UNREAD_MAIL; break;
                case Led_On       : leds. status = LED_MAIL_NEWMAIL_EXISTS; break;
                case Led_BlinkSlow: leds. status = LED_MAIL_UNREAD_MAIL_EX; break;
                default            : ok = false;
            }

            if ( ok && ( ::ioctl ( fd, SHARP_LED_SETSTATUS, &leds ) >= 0 )) {
                m_leds [0] = st;
                return true;
            }
        }
    }
    return false;
}

bool Zaurus::setSoftSuspend ( bool soft )
{
    if (!m_embedix) {
        /* non-Embedix kernels dont have kernel autosuspend */
        return ODevice::setSoftSuspend( soft );
    }

    bool res = false;
    int fd;

    if ((( fd = ::open ( "/dev/apm_bios", O_RDWR )) >= 0 ) ||
        (( fd = ::open ( "/dev/misc/apm_bios",O_RDWR )) >= 0 )) {

        int sources = ::ioctl ( fd, APM_IOCGEVTSRC, 0 ); // get current event sources

        if ( sources >= 0 ) {
            if ( soft )
                sources &= ~APM_EVT_POWER_BUTTON;
            else
                sources |= APM_EVT_POWER_BUTTON;

            if ( ::ioctl ( fd, APM_IOCSEVTSRC, sources ) >= 0 ) // set new event sources
                res = true;
            else
                perror ( "APM_IOCGEVTSRC" );
        }
        else
            perror ( "APM_IOCGEVTSRC" );

        ::close ( fd );
    }
    else
        perror ( "/dev/apm_bios or /dev/misc/apm_bios" );

    return res;
}

bool Zaurus::setDisplayBrightness( int bright )
{
    //qDebug( "Zaurus::setDisplayBrightness( %d )", bright );
    bool res = false;

    if ( bright > 255 ) bright = 255;
    if ( bright < 0 ) bright = 0;

    if ( m_embedix )
    {
        int numberOfSteps = displayBrightnessResolution();
        int fd = ::open( SHARP_FL_IOCTL_DEVICE, O_WRONLY|O_NONBLOCK );       
        if ( fd )
        {
            int val = ( bright * numberOfSteps ) / 255;
            res = ( ::ioctl ( fd, SHARP_FL_IOCTL_STEP_CONTRAST, val ) == 0 );
            ::close ( fd );
        }
    }
    else
    {
        qDebug( "ODevice handling for non-embedix kernels not yet implemented" );
    }
    return res;
}

bool Zaurus::setDisplayStatus( bool on )
{
    bool res = false;
    if ( m_embedix )
    {
        int fd = ::open( SHARP_FL_IOCTL_DEVICE, O_WRONLY|O_NONBLOCK );       
        if ( fd )
        {
            int ioctlnum = on ? SHARP_FL_IOCTL_ON : SHARP_FL_IOCTL_OFF;
            res = ( ::ioctl ( fd, ioctlnum, 0 ) == 0 );
            ::close ( fd );
        }
    }
    else
    {
        qDebug( "ODevice handling for non-embedix kernels not yet implemented" );
    }
    return res;
}    

bool Zaurus::suspend()
{
    qDebug("ODevice::suspend");
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    if ( d->m_model == Model_Unknown ) // better don't suspend in qvfb / on unkown devices
        return false;

    bool res = false;
    ODevice::sendSuspendmsg();

    struct timeval tvs, tvn;
    ::gettimeofday ( &tvs, 0 );

    ::sync(); // flush fs caches
    res = ( ::system ( "apm --suspend" ) == 0 );

    // This is needed because the apm implementation is asynchronous and we
    // can not be sure when exactly the device is really suspended
    if ( res ) {
        do { // Yes, wait 15 seconds. This APM sucks big time.
            ::usleep ( 200 * 1000 );
            ::gettimeofday ( &tvn, 0 );
        } while ((( tvn. tv_sec - tvs. tv_sec ) * 1000 + ( tvn. tv_usec - tvs. tv_usec ) / 1000 ) < 15000 );
    }

    QCopEnvelope ( "QPE/Rotation", "rotateDefault()" );
    return res;
}


Transformation Zaurus::rotation() const
{
    Transformation rot;
    int handle = 0;
    int retval = 0;

    switch ( d->m_model ) {
        case Model_Zaurus_SLC7x0:
            handle = ::open("/dev/apm_bios", O_RDWR|O_NONBLOCK);
            if (handle == -1) {
                return Rot270;
            } else {
                retval = ::ioctl(handle, SHARP_IOCTL_GET_ROTATION);
                ::close (handle);

                if (retval == 2 )
                    rot = Rot0;
                else
                    rot = Rot270;
            }
            break;
        case Model_Zaurus_SLA300:
        case Model_Zaurus_SLB600:
        case Model_Zaurus_SL5500:
        case Model_Zaurus_SL5000:
        default:
            rot = d->m_rotation;
            break;
    }

    return rot;
}
ODirection Zaurus::direction() const
{
    ODirection dir;
    int handle = 0;
    int retval = 0;
    switch ( d->m_model ) {
        case Model_Zaurus_SLC7x0:
            handle = ::open( "/dev/apm_bios", O_RDWR|O_NONBLOCK );
            if (handle == -1) {
                dir = CW;
            } else {
                retval = ::ioctl( handle, SHARP_IOCTL_GET_ROTATION );
                ::close (handle);
                if (retval == 2 )
                    dir = CCW;
                else
                    dir = CW;
            }
            break;
        case Model_Zaurus_SLA300:
        case Model_Zaurus_SLB600:
        case Model_Zaurus_SL5500:
        case Model_Zaurus_SL5000:
        default: dir = d->m_direction; 
            break;
    }
    return dir;

}

int Zaurus::displayBrightnessResolution() const
{
    if (m_embedix)
    {
        int handle = ::open( SHARP_FL_IOCTL_DEVICE, O_RDWR|O_NONBLOCK );
        if ( handle != -1 ) return ::ioctl( handle, SHARP_FL_IOCTL_GET_STEP, 0 );
        else return 1;
    }
    else
    {
        qDebug( "ODevice handling for non-embedix kernels not yet implemented" );
        return 1;
    }
}

bool Zaurus::hasHingeSensor() const
{
    return d->m_model == Model_Zaurus_SLC7x0;
}

OHingeStatus Zaurus::readHingeSensor()
{
    int handle = ::open("/dev/apm_bios", O_RDWR|O_NONBLOCK);
    if (handle == -1)
    {
        qWarning("Zaurus::readHingeSensor() - failed (%s)", "unknown reason" ); //FIXME: use strerror
        return CASE_UNKNOWN;
    }
    else
    {
        int retval = ::ioctl(handle, SHARP_IOCTL_GET_ROTATION);
        ::close (handle);
        if ( retval == CASE_CLOSED || retval == CASE_PORTRAIT || retval == CASE_LANDSCAPE )
        {
            qDebug( "Zaurus::readHingeSensor() - result = %d", retval );
            return static_cast<OHingeStatus>( retval );
        }
        else
        {
            qWarning("Zaurus::readHingeSensor() - couldn't compute hinge status!" );
            return CASE_UNKNOWN;
        }
    }
}
