/*
                             This file is part of the Opie Project
                             Copyright (C) 2002-2005 The Opie Team <opie-devel@handhelds.org>
              =.             Copyright (C) 2002-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "odevice_htc.h"

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

struct htc_button htc_buttons [] = {
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

struct htc_button htc_buttons_universal [] = {
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

    { Qt::Key_F15, QT_TRANSLATE_NOOP("Button", "Hinge1"),
    "devicebuttons/z_hinge1",
    "QPE/Rotation", "rotateDefault()",0,0},
    { Qt::Key_F16, QT_TRANSLATE_NOOP("Button", "Hinge2"),
    "devicebuttons/z_hinge2",
    "QPE/Rotation", "rotateDefault()",0,0},
    { Qt::Key_F17, QT_TRANSLATE_NOOP("Button", "Hinge3"),
    "devicebuttons/z_hinge3",
    "QPE/Rotation", "rotateDefault()",0,0},
};


//
//       HTC-Universal (PXA-model w/ 480x640 3.6" lcd)
//       HTC-Alpine    (PXA-model w/ 240x320 3.5" lcd)
//       HTC-Apache    (PXA-model w/ 240x320 2.8" lcd)
//       HTC-Beetles   (PXA-model w/ 240x240 3.0" lcd)
//       HTC-Blueangel (PXA-model w/ 240x320 3.5" lcd)
//       HTC-Himalaya  (PXA-model w/ 240x320 3.5" lcd)
//       HTC-Magician  (PXA-model w/ 240x320 2.8" lcd)

void HTC::init(const QString& cpu_info)
{
    qDebug( "HTC::init()" );
    // Set the time to wait until the system is really suspended
    // the delta between apm --suspend and sleeping
    setAPMTimeOut( 15000 );

    d->m_vendorstr = "Xanadux Team";
    d->m_vendor = Vendor_HTC;
    d->m_systemstr = "Familiar";
    d->m_system = System_Familiar;

    // check the HTC model
    QString model;
    int loc = cpu_info.find( ":" );
    if ( loc != -1 )
        model = cpu_info.mid( loc+2 ).simplifyWhiteSpace();
    else
        model = cpu_info;

    d->m_model = Model_HTC_Universal;
    d->m_modelstr = "Unknown HTC";

    if ( model == "HTC Universal" ) {
        d->m_model = Model_HTC_Universal;
        d->m_modelstr = "HTC Universal";
    }
    if ( model == "HTC Alpine" ) {
        d->m_model = Model_HTC_Alpine;
        d->m_modelstr = "HTC Alpine";
    }
    if ( model == "HTC Apache" ) {
        d->m_model = Model_HTC_Apache;
        d->m_modelstr = "HTC Apache";
    }
    if ( model == "HTC Beetles" ) {
        d->m_model = Model_HTC_Beetles;
        d->m_modelstr = "HTC Beetles";
    }
    if ( model == "HTC Blueangel" ) {
        d->m_model = Model_HTC_Blueangel;
        d->m_modelstr = "HTC Blueangel";
    }
    if ( model == "HTC Himalaya" ) {
        d->m_model = Model_HTC_Himalaya;
        d->m_modelstr = "HTC Himalaya";
    }
    if ( model == "HTC Magician" ) {
        d->m_model = Model_HTC_Magician;
        d->m_modelstr = "HTC Magician";
    }


    // set path to backlight device
    switch ( d->m_model )
    {
        case Model_HTC_Universal:
        case Model_HTC_Magician:
        case Model_HTC_Alpine:
        case Model_HTC_Beetles:
        case Model_HTC_Apache:
            m_backlightdev = "/sys/class/backlight/corgi-bl/";
        break;
        case Model_HTC_Blueangel:
        case Model_HTC_Himalaya:
            m_backlightdev = "/sys/class/backlight/w100fb/";
        break;
        default:
            m_backlightdev = "/sys/class/backlight/corgi-bl/";
    }

    // set initial rotation
    switch( d->m_model )
    {
        case Model_HTC_Universal:
            initHingeSensor();
            d->m_rotation = rotation();
            d->m_direction = direction();
            break;
        default:
            d->m_rotation = Rot270;
    }

    // set default qte driver
    switch( d->m_model )
    {
        default:
            d->m_qteDriver = "Transformed";
    }

    m_leds[0] = Led_Off;

    qDebug( "HTC::init() - Using the 2.6 Xanadux on a %s", (const char*) d->m_modelstr );
}

void HTC::initButtons()
{
    qDebug( "HTC::initButtons()" );
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList <ODeviceButton>;

    struct htc_button * phtc_buttons;
    int buttoncount;
    switch ( d->m_model )
    {
        case Model_HTC_Universal:
            if ( isQWS( ) )
            {
                addPreHandler(this);
            }
            phtc_buttons = htc_buttons_universal;
            buttoncount = ARRAY_SIZE(htc_buttons_universal);
            break;
        default:
            phtc_buttons = htc_buttons;
            buttoncount = ARRAY_SIZE(htc_buttons);
            break;
    }

    for ( int i = 0; i < buttoncount; i++ ) {
        struct htc_button *zb = phtc_buttons + i;
        ODeviceButton b;

        b.setKeycode( zb->code );
        b.setUserText( QObject::tr( "Button", zb->utext ));
        b.setPixmap( OResource::loadPixmap( zb->pix ));
        b.setFactoryPresetPressedAction( OQCopMessage( makeChannel ( zb->fpressedservice ), zb->fpressedaction ));
        b.setFactoryPresetHeldAction( OQCopMessage( makeChannel ( zb->fheldservice ), zb->fheldaction ));
        d->m_buttons->append( b );
    }

    reloadButtonMapping();
}



typedef struct sharp_led_status {
    int which;   /* select which LED status is wanted. */
    int status;  /* set new led status if you call SHARP_LED_SETSTATUS */
} sharp_led_status;

void HTC::buzzer( int sound )
{
#ifndef QT_NO_SOUND
    Sound *snd = 0;

    // All devices except SL5500 have a DSP device
    if ( d->m_model == Model_HTC_Universal ) {

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
        snd->play();
    } else if( !snd ) {
        int fd = ::open ( "/dev/sharp_buz", O_WRONLY|O_NONBLOCK );

        if ( fd >= 0 ) {
            if (::ioctl ( fd, SHARP_BUZZER_MAKESOUND, sound ) == -1)
                qWarning( "HTC::buzzer() - Couldn't make the buzzer buzz (%s)",
                strerror( errno ) );
            ::close ( fd );
        }

    }
#endif
}


void HTC::playAlarmSound()
{
    buzzer( SHARP_BUZ_SCHEDULE_ALARM );
}

void HTC::playTouchSound()
{
    buzzer( SHARP_BUZ_TOUCHSOUND );
}

void HTC::playKeySound()
{
    buzzer( SHARP_BUZ_KEYSOUND );
}


QValueList <OLed> HTC::ledList() const
{
    QValueList <OLed> vl;
    vl << Led_Mail;
    return vl;
}

QValueList <OLedState> HTC::ledStateList( OLed l ) const
{
    QValueList <OLedState> vl;

    if ( l == Led_Mail )
        vl << Led_Off << Led_On << Led_BlinkSlow;
    return vl;
}

OLedState HTC::ledState( OLed which ) const
{
    if ( which == Led_Mail )
        return m_leds [0];
    else
        return Led_Off;
}

bool HTC::setLedState( OLed, OLedState )
{
    qDebug( "HTC::setLedState: ODevice handling not yet implemented" );
    return false;
}

int HTC::displayBrightnessResolution() const
{
    int res = 1;

    int fd = ::open( m_backlightdev + "max_brightness", O_RDONLY|O_NONBLOCK );

        if ( fd >= 0 )
        {
            char buf[100];
            if ( ::read( fd, &buf[0], sizeof buf ) ) ::sscanf( &buf[0], "%d", &res );
            ::close( fd );
        }

    return res;
}

bool HTC::setDisplayBrightness( int bright )
{

    //qDebug( "HTC::setDisplayBrightness( %d )", bright );
    bool res = false;

    if ( bright > 255 ) bright = 255;
    if ( bright < 0 ) bright = 0;

    int numberOfSteps = displayBrightnessResolution();
    int val = ( bright == 1 ) ? 1 : ( bright * numberOfSteps ) / 255;

    int fd = ::open( m_backlightdev + "brightness", O_WRONLY|O_NONBLOCK );
    if ( fd >= 0 )
    {
        char buf[100];
        int len = ::snprintf( &buf[0], sizeof buf, "%d", val );
        if (len > 0)
            res = ( ::write( fd, &buf[0], len ) == 0 );
        ::close( fd );
    }
    return res;
}

bool HTC::setDisplayStatus( bool on )
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

Transformation HTC::rotation() const
{
    qDebug( "HTC::rotation()" );
    Transformation rot = Rot270;

    switch ( d->m_model ) {
        case Model_HTC_Universal:
            {
                OHingeStatus hs = readHingeSensor();
                qDebug( "HTC::rotation() - hinge sensor = %d", (int) hs );
                if ( hs == CASE_PORTRAIT ) rot = Rot0;
                else if ( hs == CASE_UNKNOWN ) rot = Rot270;
            }
            break;
        default:
            break;
    }

    qDebug( "HTC::rotation() - returning '%d'", rot );
    return rot;
}

ODirection HTC::direction() const
{
    ODirection dir;

    switch ( d->m_model ) {
        case Model_HTC_Universal: 
            {
                OHingeStatus hs = readHingeSensor();
                if ( hs == CASE_PORTRAIT ) dir = CCW;
                else if ( hs == CASE_UNKNOWN ) dir = CCW;
                else dir = CW;
            }
            break;
        default: 
            dir = d->m_direction;
            break;
    }
    return dir;

}

bool HTC::hasHingeSensor() const
{
    return d->m_model == Model_HTC_Universal;
}

OHingeStatus HTC::readHingeSensor() const
{
    /*
        * The HTC Universal keyboard is event source 1 in kernel 2.6.
        * Hinge status is reported via Input System Switchs 0 and 1 like that:
        *
        * -------------------------
        * | SW0 | SW1 |    CASE   |
        * |-----|-----|-----------|
        * |  0     0    Unknown   |
        * |  1     0    Portrait  |
        * |  0     1    Closed    |
        * |  1     1    Landscape |
        * -------------------------
        */
    OInputDevice* keyboard = OInputSystem::instance()->device( "event1" );
    bool switch0 = true;
    bool switch1 = false;

    if ( keyboard )
    {
        switch0 = keyboard->isHeld( OInputDevice::Switch0 );
        switch1 = keyboard->isHeld( OInputDevice::Switch1 );
    }

    if ( switch0 )
    {
        return switch1 ? CASE_LANDSCAPE : CASE_PORTRAIT;
    }
    else
    {
        return switch1 ? CASE_CLOSED : CASE_UNKNOWN;
    }
}

void HTC::initHingeSensor()
{
    if ( m_embedix ) return;

    m_hinge.setName( "/dev/input/event1" );
    if ( !m_hinge.open( IO_ReadOnly ) )
    {
        qWarning( "HTC::init() - Couldn't open /dev/input/event1 for read (%s)", strerror( errno ) );
        return;
    }

    QSocketNotifier* sn = new QSocketNotifier( m_hinge.handle(), QSocketNotifier::Read, this );
    QObject::connect( sn, SIGNAL(activated(int)), this, SLOT(hingeSensorTriggered()) );

    qDebug( "HTC::init() - Hinge Sensor Initialization successfully completed" );
}

void HTC::hingeSensorTriggered()
{
    qDebug( "HTC::hingeSensorTriggered() - got event" );
    struct input_event e;
    if ( ::read( m_hinge.handle(), &e, sizeof e ) > 0 )
    {
        qDebug( "HTC::hingeSensorTriggered() - event has type %d, code %d, value %d", e.type, e.code, e.value );
        if ( e.type != EV_SW ) return;
        if ( readHingeSensor() != CASE_UNKNOWN )
        {
            qDebug( "HTC::hingeSensorTriggered() - got valid switch event, calling rotateDefault()" );
            QCopChannel::send( "QPE/Rotation", "rotateDefault()" );
        }
    }
}

void HTC::systemMessage( const QCString &msg, const QByteArray & )
{
    if ( msg == "deviceButtonMappingChanged()" ) {
        reloadButtonMapping();
    }
}

/*
 * Take code from iPAQ device.
 * That way we switch the cursor directions depending on status of hinge sensor, eg. hardware direction.
 * I hope that is ok - Alwin
 */
bool HTC::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
    int newkeycode = keycode;

    if ( !hasHingeSensor() ) return false;

    /* map cursor keys depending on the hinge status */
    switch ( keycode ) {
        // Rotate cursor keys
        case Key_Left :
        case Key_Right:
        case Key_Up   :
        case Key_Down :
            {
                if (rotation()==Rot90) {
                    newkeycode = Key_Left + ( keycode - Key_Left + 3 ) % 4;
                }
            }
            break;

    }
    if (newkeycode!=keycode) {
        if ( newkeycode != Key_unknown ) {
            QWSServer::sendKeyEvent ( -1, newkeycode, modifiers, isPress, autoRepeat );
        }
        return true;
    }
    return false;
}

bool HTC::suspend() {
    return false;
}
