/*
                     This file is part of the Opie Project
                      Copyright (C) The Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU Library General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      Library General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           Library General Public License along with
  --    :-=`           this library; see the file COPYING.LIB.
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

using namespace Opie;

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
    "mail", "raise()",
    "mail", "newMail()" },
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
//       Zaurus-Corgi  (PXA-model w/ 640x480 lcd, for C700, C750, C760, and C860)
//
//       Only question right now is: Do we really need to do it? Because as soon
//       as the OpenZaurus kernel is ready, there will be a unified interface for all
//       Zaurus models (concerning apm, backlight, buttons, etc.)
//
//       Comments? - mickeyl.

void Zaurus::init()
{
    d->m_vendorstr = "Sharp";
    d->m_vendor = Vendor_Sharp;
    m_embedix = true;  // Not openzaurus means: It has an embedix kernel !

    // QFile f ( "/proc/filesystems" );
    QString model;

    // It isn't a good idea to check the system configuration to
    // detect the distribution !
    // Otherwise it may happen that any other distribution is detected as openzaurus, just
    // because it uses a jffs2 filesystem..
    // (eilers)
    // if ( f. open ( IO_ReadOnly ) && ( QTextStream ( &f ). read(). find ( "\tjffs2\n" ) >= 0 )) {
    QFile f ("/etc/oz_version");
    if ( f.exists() ){
        d->m_vendorstr = "OpenZaurus Team";
        d->m_systemstr = "OpenZaurus";
        d->m_system = System_OpenZaurus;

        if ( f. open ( IO_ReadOnly )) {
            QTextStream ts ( &f );
            d->m_sysverstr = ts. readLine();//. mid ( 10 );
            f. close();
        }

        // Openzaurus sometimes uses the embedix kernel!
        // => Check whether this is an embedix kernel
        FILE *uname = popen("uname -r", "r");
        QString line;
        if ( f.open(IO_ReadOnly, uname) ) {
            QTextStream ts ( &f );
            line = ts. readLine();
            int loc = line. find ( "embedix" );
            if ( loc != -1 )
                m_embedix = true;
            else
                m_embedix = false;
            f. close();
        }
        pclose(uname);
    }
    else {
        d->m_systemstr = "Zaurus";
        d->m_system = System_Zaurus;
    }

    f. setName ( "/proc/cpuinfo" );
    if ( f. open ( IO_ReadOnly ) ) {
        QTextStream ts ( &f );
        QString line;
        while( line = ts. readLine() ) {
            if ( line. left ( 8 ) == "Hardware" )
                break;
        }
        int loc = line. find ( ":" );
        if ( loc != -1 )
            model = line. mid ( loc + 2 ). simplifyWhiteSpace( );
    }

    if ( model == "SHARP Corgi" ) {
        d->m_model = Model_Zaurus_SLC7x0;
        d->m_modelstr = "Zaurus SL-C700";
    } else if ( model == "SHARP Shepherd" ) {
        d->m_model = Model_Zaurus_SLC7x0;
        d->m_modelstr = "Zaurus SL-C750";
    } else if ( model == "SHARP Husky" ) {
        d->m_model = Model_Zaurus_SLC7x0;
        d->m_modelstr = "Zaurus SL-C760";
    } else if ( model == "SHARP Poodle" ) {
        d->m_model = Model_Zaurus_SLB600;
        d->m_modelstr = "Zaurus SL-B500 or SL-5600";
    } else if ( model == "Sharp-Collie" || model == "Collie" ) {
        d->m_model = Model_Zaurus_SL5500;
        d->m_modelstr = "Zaurus SL-5500 or SL-5000d";
    } else {
        d->m_model = Model_Zaurus_SL5500;
        d->m_modelstr = "Zaurus (Model unknown)";
    }

    bool flipstate = false;
    switch ( d->m_model ) {
        case Model_Zaurus_SLA300:
            d->m_rotation = Rot0;
            break;
        case Model_Zaurus_SLC7x0:
            d->m_rotation = rotation();
            d->m_direction = direction();
            break;
        case Model_Zaurus_SLB600:
        case Model_Zaurus_SL5500:
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

    QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
    connect ( sysch, SIGNAL( received(const QCString&,const QByteArray&)),
        this, SLOT( systemMessage(const QCString&,const QByteArray&)));
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

//#include <asm/sharp_char.h> // including kernel headers is evil ...

#define SHARP_DEV_IOCTL_COMMAND_START 0x5680

#define	SHARP_BUZZER_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
#define SHARP_BUZZER_MAKESOUND   (SHARP_BUZZER_IOCTL_START)

#define SHARP_BUZ_TOUCHSOUND       1  /* touch panel sound */
#define SHARP_BUZ_KEYSOUND         2  /* key sound */
#define SHARP_BUZ_SCHEDULE_ALARM  11  /* schedule alarm */

/* --- for SHARP_BUZZER device --- */

//#define	SHARP_BUZZER_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
//#define SHARP_BUZZER_MAKESOUND   (SHARP_BUZZER_IOCTL_START)

#define SHARP_BUZZER_SETVOLUME   (SHARP_BUZZER_IOCTL_START+1)
#define SHARP_BUZZER_GETVOLUME   (SHARP_BUZZER_IOCTL_START+2)
#define SHARP_BUZZER_ISSUPPORTED (SHARP_BUZZER_IOCTL_START+3)
#define SHARP_BUZZER_SETMUTE     (SHARP_BUZZER_IOCTL_START+4)
#define SHARP_BUZZER_STOPSOUND   (SHARP_BUZZER_IOCTL_START+5)

//#define SHARP_BUZ_TOUCHSOUND       1  /* touch panel sound */
//#define SHARP_BUZ_KEYSOUND         2  /* key sound */

//#define SHARP_PDA_ILLCLICKSOUND    3  /* illegal click */
//#define SHARP_PDA_WARNSOUND        4  /* warning occurred */
//#define SHARP_PDA_ERRORSOUND       5  /* error occurred */
//#define SHARP_PDA_CRITICALSOUND    6  /* critical error occurred */
//#define SHARP_PDA_SYSSTARTSOUND    7  /* system start */
//#define SHARP_PDA_SYSTEMENDSOUND   8  /* system shutdown */
//#define SHARP_PDA_APPSTART         9  /* application start */
//#define SHARP_PDA_APPQUIT         10  /* application ends */

//#define SHARP_BUZ_SCHEDULE_ALARM  11  /* schedule alarm */
//#define SHARP_BUZ_DAILY_ALARM     12  /* daily alarm */
//#define SHARP_BUZ_GOT_PHONE_CALL  13  /* phone call sound */
//#define SHARP_BUZ_GOT_MAIL        14  /* mail sound */
//

#define	SHARP_LED_IOCTL_START (SHARP_DEV_IOCTL_COMMAND_START)
#define SHARP_LED_SETSTATUS   (SHARP_LED_IOCTL_START+1)

#define  SHARP_IOCTL_GET_ROTATION 0x413c

typedef struct sharp_led_status {
int which;   /* select which LED status is wanted. */
int status;  /* set new led status if you call SHARP_LED_SETSTATUS */
} sharp_led_status;

#define SHARP_LED_MAIL_EXISTS  9       /* mail status (exists or not) */

#define LED_MAIL_NO_UNREAD_MAIL  0   /* for SHARP_LED_MAIL_EXISTS */
#define LED_MAIL_NEWMAIL_EXISTS  1   /* for SHARP_LED_MAIL_EXISTS */
#define LED_MAIL_UNREAD_MAIL_EX  2   /* for SHARP_LED_MAIL_EXISTS */

// #include <asm/sharp_apm.h> // including kernel headers is evil ...

#define APM_IOCGEVTSRC          OD_IOR( 'A', 203, int )
#define APM_IOCSEVTSRC          OD_IORW( 'A', 204, int )
#define APM_EVT_POWER_BUTTON    (1 << 0)

#define FL_IOCTL_STEP_CONTRAST    100


void Zaurus::buzzer ( int sound )
{
#ifndef QT_NO_SOUND
    QString soundname;

    // Not all devices have real sound
    if ( d->m_model == Model_Zaurus_SLC7x0
        || d->m_model == Model_Zaurus_SLB600 ){

        switch ( sound ){
        case SHARP_BUZ_SCHEDULE_ALARM:
            soundname = "alarm";
            break;
        case SHARP_BUZ_TOUCHSOUND:
            soundname = "touchsound";
            break;
        case SHARP_BUZ_KEYSOUND:
            soundname = "keysound";
            break;
        default:
            soundname = "alarm";

        }
    }

    // If a soundname is defined, we expect that this device has
    // sound capabilities.. Otherwise we expect to have the buzzer
    // device..
    if ( !soundname.isEmpty() ){
        int fd;
        int vol;
        bool vol_reset = false;

        Sound snd ( soundname );

        if (( fd = ::open ( "/dev/sound/mixer", O_RDWR )) >= 0 ) {
            if ( ::ioctl ( fd, MIXER_READ( 0 ), &vol ) >= 0 ) {
                Config cfg ( "qpe" );
                cfg. setGroup ( "Volume" );

                int volalarm = cfg. readNumEntry ( "AlarmPercent", 50 );
                if ( volalarm < 0 )
                    volalarm = 0;
                else if ( volalarm > 100 )
                    volalarm = 100;
                volalarm |= ( volalarm << 8 );

                if ( ::ioctl ( fd, MIXER_WRITE( 0 ), &volalarm ) >= 0 )
                    vol_reset = true;
            }
        }

        snd. play();
        while ( !snd. isFinished())
            qApp->processEvents();

        if ( fd >= 0 ) {
            if ( vol_reset )
                ::ioctl ( fd, MIXER_WRITE( 0 ), &vol );
            ::close ( fd );
        }
    } else {		
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
    buzzer ( SHARP_BUZ_SCHEDULE_ALARM );
}

void Zaurus::playTouchSound()
{
    buzzer ( SHARP_BUZ_TOUCHSOUND );
}

void Zaurus::playKeySound()
{
    buzzer ( SHARP_BUZ_KEYSOUND );
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


bool Zaurus::setDisplayBrightness ( int bright )
{
    //qDebug( "Zaurus::setDisplayBrightness( %d )", bright );
    bool res = false;
    int fd;

    if ( bright > 255 ) bright = 255;
    if ( bright < 0 ) bright = 0;

    if ( m_embedix )
    {
        if ( d->m_model == Model_Zaurus_SLC7x0 )
        {
            //qDebug( "using special treatment for devices with the corgi backlight interface" );
            // special treatment for devices with the corgi backlight interface
            if (( fd = ::open ( "/proc/driver/fl/corgi-bl", O_WRONLY )) >= 0 )
            {
                int value = ( bright == 1 ) ? 1 : bright * ( 17.0 / 255.0 );
                char writeCommand[100];
                const int count = sprintf( writeCommand, "0x%x\n", value );
                res = ( ::write ( fd, writeCommand, count ) != -1 );
                ::close ( fd );
            }
            return res;
        }
        else
        {
            // standard treatment for devices with the dumb embedix frontlight interface
            if (( fd = ::open ( "/dev/fl", O_WRONLY )) >= 0 ) {
                int bl = ( bright * 4 + 127 ) / 255; // only 4 steps on zaurus
                if ( bright && !bl )
                    bl = 1;
                res = ( ::ioctl ( fd, FL_IOCTL_STEP_CONTRAST, bl ) == 0 );
                ::close ( fd );
            }
        }
    }
    else
    {
        // special treatment for the OpenZaurus unified interface
        #define FB_BACKLIGHT_SET_BRIGHTNESS     _IOW('F', 1, u_int)             /* set brightness */
        if (( fd = ::open ( "/dev/fb0", O_WRONLY )) >= 0 ) {
            res = ( ::ioctl ( fd , FB_BACKLIGHT_SET_BRIGHTNESS, bright ) == 0 );
            ::close ( fd );
        }
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

    // This is needed because the iPAQ apm implementation is asynchronous and we
    // can not be sure when exactly the device is really suspended
    // This can be deleted as soon as a stable familiar with a synchronous apm implementation exists.

    if ( res ) {
        do { // Yes, wait 15 seconds. This APM bug sucks big time.
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
            handle = ::open("/dev/apm_bios", O_RDWR|O_NONBLOCK);
            if (handle == -1) {
                dir = CW;
            } else {
                retval = ::ioctl(handle, SHARP_IOCTL_GET_ROTATION);
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
        default:
            dir = d->m_direction;
            break;
    }
    return dir;

}

int Zaurus::displayBrightnessResolution() const
{
    if (m_embedix)
        return d->m_model == Model_Zaurus_SLC7x0 ? 18 : 5;
    else
        return 256;
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
