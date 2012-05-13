/*
                             This file is part of the Opie Project
                             Copyright (C) 2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
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

#include "odevice_zaurus.h"

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

struct z_button z_buttons_c3000 [] = {
    { Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
    "devicebuttons/z_calendar2",
    "datebook", "nextView()",
    "today", "raise()" },
    { Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
    "devicebuttons/z_contact2",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/z_mail2",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/z_home2",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Menu Button"),
    "devicebuttons/z_menu2",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },

    { Qt::Key_F15, QT_TRANSLATE_NOOP("Button", "Home Softkey"),
    "devicebuttons/z_silk_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Qt::Key_F16, QT_TRANSLATE_NOOP("Button", "Mail Softkey"),
    "devicebuttons/z_silk_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { Qt::Key_F17, QT_TRANSLATE_NOOP("Button", "Contacts Softkey"),
    "devicebuttons/z_silk_contact",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Qt::Key_F18, QT_TRANSLATE_NOOP("Button", "Calendar Softkey"),
    "devicebuttons/z_silk_calendar",
    "datebook", "nextView()",
    "today", "raise()" },
    { Qt::Key_F19, QT_TRANSLATE_NOOP("Button", "Dictionary Softkey"),
    "devicebuttons/z_silk_dict",
    "odict", "raise()",
    "today", "raise()" },
};

struct z_button z_buttons_6000 [] = {
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
    { Qt::Key_F15, QT_TRANSLATE_NOOP("Button", "Rotate Button"),
    "devicebuttons/z_rotate",
    0, "QPE/Rotation",
    "rotateDefault()", 0 },
    { Qt::Key_F24, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "devicebuttons/z_hinge3",
    "QPE/VMemo", "toggleRecord()",
    "sound", "raise()" },
};

struct ODeviceButtonComboStruct z_combos[] = {
    // Centre of joypad (OK) + Calendar -> recalibrate
    { Model_Zaurus_All,
    Qt::Key_Return, Qt::Key_F9, Qt::Key_unknown, false, QT_TRANSLATE_NOOP("Button", "OK + Calendar"),
    "QPE/Application/calibrate", "raise()", QT_TRANSLATE_NOOP("ComboAction", "Recalibrate screen"), false },
};


// FIXME This gets unnecessary complicated. We should think about splitting the Zaurus
//       class up into individual classes. We would need three classes
//
//       Zaurus-Collie (SA-model  w/ 320x240 lcd, for SL5500 and SL5000)
//       Zaurus-Poodle (PXA-model w/ 320x240 lcd, for SL5600)
//       Zaurus-Corgi  (PXA-model w/ 640x480 lcd, for C700, C750, C760, C860, C3000, C1000, C3100)
//       Zaurus-Tosa   (PXA-model w/ 480x640 lcd, for SL6000)

void Zaurus::init(const QString& cpu_info)
{
    qDebug( "Zaurus::init()" );

    // No need to wait after apm --suspend
    setAPMTimeOut( 0 );

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
    } else if ( model == "SHARP Boxer" ) {
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
    } else if ( model == "SHARP Spitz" ) {
        d->m_model = Model_Zaurus_SLC3000;
        d->m_modelstr = "Zaurus SL-C3000";
    } else if ( model == "SHARP Akita" ) {
        d->m_model = Model_Zaurus_SLC1000;
        d->m_modelstr = "Zaurus SL-C1000";
    } else if ( model == "SHARP Borzoi" ) {
        d->m_model = Model_Zaurus_SLC3100;
        d->m_modelstr = "Zaurus SL-C3100";
    } else {
        d->m_model = Model_Zaurus_SL5500;
        d->m_modelstr = "Unknown Zaurus";
    }

    // Ensure vendor is set
    if( d->m_vendor == Vendor_Unknown ) {
        d->m_vendorstr = "SHARP";
        d->m_vendor = Vendor_Sharp;
    }

    // set initial rotation
    switch( d->m_model )
    {
        case Model_Zaurus_SL6000: // fallthrough
        case Model_Zaurus_SLA300:
            d->m_rotation = Rot0;
            break;
        case Model_Zaurus_SLC3100: // fallthrough
        case Model_Zaurus_SLC3000: // fallthrough
        case Model_Zaurus_SLC1000: // fallthrough
        case Model_Zaurus_SLC7x0:
            initHingeSensor();
            d->m_rotation = rotation();
            d->m_direction = direction();
            break;
        case Model_Zaurus_SLB600: // fallthrough
        case Model_Zaurus_SL5000: // fallthrough
        case Model_Zaurus_SL5500: // fallthrough
        default:
            d->m_rotation = Rot270;
    }

    // set default qte driver
/*    switch( d->m_model )
    {
        case Model_Zaurus_SLC7x0:
            d->m_qteDriver = "W100";
            break;
        default:*/
            d->m_qteDriver = "Transformed";
//    }

    qDebug( "Zaurus::init() - Using the 2.6 OpenZaurus HAL on a %s", (const char*) d->m_modelstr );
}

void Zaurus::initButtons()
{
    qDebug( "Zaurus::initButtons()" );
    if ( d->m_buttons )
        return;

    d->m_buttons = new QValueList <ODeviceButton>;

    struct z_button * pz_buttons;
    int buttoncount;
    switch ( d->m_model )
    {
        case Model_Zaurus_SL6000:
            pz_buttons = z_buttons_6000;
            buttoncount = ARRAY_SIZE(z_buttons_6000);
            break;
        case Model_Zaurus_SLC3000:
            pz_buttons = z_buttons_c3000;
            buttoncount = ARRAY_SIZE(z_buttons_c3000);
            break;
        case Model_Zaurus_SLC3100: // fallthrough
        case Model_Zaurus_SLC1000: // fallthrough
        case Model_Zaurus_SLC7x0:
            if ( isQWS( ) )
            {
                addPreHandler(this);
            }
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

        b.setKeycode( zb->code );
        b.setUserText( QObject::tr( "Button", zb->utext ));
        b.setPixmap( OResource::loadPixmap( zb->pix ));
        b.setFactoryPresetPressedAction( OQCopMessage( makeChannel ( zb->fpressedservice ), zb->fpressedaction ));
        b.setFactoryPresetHeldAction( OQCopMessage( makeChannel ( zb->fheldservice ), zb->fheldaction ));
        d->m_buttons->append( b );
    }

    reloadButtonMapping();
}

void Zaurus::initButtonCombos()
{
    if ( d->m_buttonCombos )
        return;

    d->m_buttonCombos = new QValueList<ODeviceButtonCombo>;
    loadButtonCombos( z_combos, sizeof( z_combos ) / sizeof( ODeviceButtonComboStruct ) );
}


void Zaurus::buzzer( int sound )
{
#ifndef QT_NO_SOUND
    Sound *snd = 0;

    // All devices except collie have a DSP device
    if ( d->m_model != Model_Zaurus_SL5000
      && d->m_model != Model_Zaurus_SL5500 ) {

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

bool Zaurus::hasWaveSpeaker() const
{
    // All devices except collie have a DSP device
    return ( d->m_model != Model_Zaurus_SL5000
        && d->m_model != Model_Zaurus_SL5500 );
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


Transformation Zaurus::rotation() const
{
    qDebug( "Zaurus::rotation()" );
    Transformation rot;

    switch ( d->m_model ) {
        case Model_Zaurus_SLC3100: // fallthrough
        case Model_Zaurus_SLC3000: // fallthrough
        case Model_Zaurus_SLC1000:
        {
            OHingeStatus hs = readHingeSensor();
            qDebug( "Zaurus::rotation() - hinge sensor = %d", (int) hs );
            if ( hs == CASE_PORTRAIT ) rot = Rot0;
            else if ( hs == CASE_UNKNOWN ) rot = Rot270;
            else rot = Rot270;
        }
        break;

        // SLC7x0 needs a special case here, because we were able to set the W100
        // hardware default rotation on kernel 2.6 to Rot0
        case Model_Zaurus_SLC7x0:
        {
            OHingeStatus hs = readHingeSensor();
            qDebug( "Zaurus::rotation() - hinge sensor = %d", (int) hs );

            if ( hs == CASE_PORTRAIT ) rot = Rot90;
            else if ( hs == CASE_UNKNOWN ) rot = Rot0;
            else rot = Rot0;
        }
        break;
        case Model_Zaurus_SL6000:
        case Model_Zaurus_SLB600:
        case Model_Zaurus_SLA300:
        case Model_Zaurus_SL5500:
        case Model_Zaurus_SL5000:
        default:
            rot = d->m_rotation;
            break;
    }

    qDebug( "Zaurus::rotation() - returning '%d'", rot );
    return rot;
}
ODirection Zaurus::direction() const
{
    ODirection dir;

    switch ( d->m_model ) {
        case Model_Zaurus_SLC3100: // fallthrough
        case Model_Zaurus_SLC3000: // fallthrough
        case Model_Zaurus_SLC1000: // fallthrough
        case Model_Zaurus_SLC7x0: {
                OHingeStatus hs = readHingeSensor();
                if ( hs == CASE_PORTRAIT ) dir = CCW;
                else if ( hs == CASE_UNKNOWN ) dir = CCW;
                else dir = CW;
            }
            break;
        case Model_Zaurus_SL6000:
        case Model_Zaurus_SLA300:
        case Model_Zaurus_SLB600:
        case Model_Zaurus_SL5500:
        case Model_Zaurus_SL5000:
        default: dir = d->m_direction;
            break;
    }
    return dir;

}

bool Zaurus::hasHingeSensor() const
{
    return d->m_model == Model_Zaurus_SLC7x0 ||
           d->m_model == Model_Zaurus_SLC3100 ||
           d->m_model == Model_Zaurus_SLC3000 ||
           d->m_model == Model_Zaurus_SLC1000;
}

OHingeStatus Zaurus::readHingeSensor() const
{
    /*
        * The corgi keyboard is event source 0 in OZ kernel 2.6.
        * Hinge status is reported via Input System Switchs 0 and 1 like that:
        *
        * -------------------------
        * | SW0 | SW1 |    CASE   |
        * |-----|-----|-----------|
        * |  0     0    Landscape |
        * |  0     1    Portrait  |
        * |  1     0    Unknown   |
        * |  1     1    Closed    |
        * -------------------------
        */
    OInputDevice* keyboard = OInputSystem::instance()->device( "event0" );
    bool switch0 = true;
    bool switch1 = false;
    if ( keyboard )
    {
        switch0 = keyboard->isHeld( OInputDevice::Switch0 );
        switch1 = keyboard->isHeld( OInputDevice::Switch1 );
    }
    if ( switch0 )
    {
        return switch1 ? CASE_CLOSED : CASE_UNKNOWN;
    }
    else
    {
        return switch1 ? CASE_PORTRAIT : CASE_LANDSCAPE;
    }
}

void Zaurus::initHingeSensor()
{
    m_hinge.setName( "/dev/input/event0" );
    if ( !m_hinge.open( IO_ReadOnly ) )
    {
        qWarning( "Zaurus::init() - Couldn't open /dev/input/event0 for read (%s)", strerror( errno ) );
        return;
    }

    QSocketNotifier* sn = new QSocketNotifier( m_hinge.handle(), QSocketNotifier::Read, this );
    QObject::connect( sn, SIGNAL(activated(int)), this, SLOT(hingeSensorTriggered()) );

    qDebug( "Zaurus::init() - Hinge Sensor Initialization successfully completed" );
}

void Zaurus::hingeSensorTriggered()
{
    qDebug( "%s - got event", __PRETTY_FUNCTION__ );
    struct input_event e;
    int hingeFd = m_hinge.handle();
    if (hingeFd == -1)
    {
	qDebug( "%s - No file descriptor for the hinge", __PRETTY_FUNCTION__);
	return;
    }

    if ( ::read( m_hinge.handle(), &e, sizeof e ) > 0 )
    {
        qDebug( "%s - event has type %d, code %d, value %d", __PRETTY_FUNCTION__, e.type, e.code, e.value );
        if ( e.type != EV_SW )
	    return;

        if ( readHingeSensor() != CASE_UNKNOWN )
        {
            qDebug( "%s - got valid switch event, calling rotateDefault()", __PRETTY_FUNCTION__ );
            QCopChannel::send( "QPE/Rotation", "rotateDefault()" );
        }
    }
}

void Zaurus::systemMessage( const QCString &msg, const QByteArray & )
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
bool Zaurus::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
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
            if (rotation()==Rot90)
                newkeycode = Key_Left + ( keycode - Key_Left + 3 ) % 4;
            break;

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

