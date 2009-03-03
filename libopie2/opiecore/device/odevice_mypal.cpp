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

#include "odevice_mypal.h"

/* QT */
#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>

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


using namespace Opie::Core;
using namespace Opie::Core::Internal;

#define Model_Keyboardless_2_6 (Model_MyPal_716)

struct m_button mypal_buttons [] = {
    // Common button map for all keyboardless devices with 2.6 kernel
    { Model_Keyboardless_2_6,
    Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
    "devicebuttons/ipaq_calendar",
    "datebook", "nextView()",
    "today", "raise()" },
    { Model_Keyboardless_2_6,
    Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
    "devicebuttons/ipaq_contact",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Model_Keyboardless_2_6,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/ipaq_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { Model_Keyboardless_2_6,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/ipaq_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Model_Keyboardless_2_6,
    Qt::Key_F24, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "devicebuttons/ipaq_record",
    "QPE/VMemo", "toggleRecord()",
    "sound", "raise()" },
};

void MyPal::init(const QString& model)
{
    d->m_vendorstr = "Asus";
    d->m_vendor = Vendor_Asus;

    d->m_modelstr = model.mid(model.findRev('A'));
    if ( d->m_modelstr == "A716" )
        d->m_model = Model_MyPal_716;
    else
        d->m_model = Model_Unknown;

    d->m_rotation = Rot0;

    m_power_timer = 0;
}

void MyPal::initButtons()
{
    if ( d->m_buttons )
        return;

    if ( isQWS( ) ) {
        addPreHandler(this);
    }

    d->m_buttons = new QValueList <ODeviceButton>;

    for ( uint i = 0; i < ( sizeof( mypal_buttons ) / sizeof( m_button )); i++ ) {
        m_button *mb = mypal_buttons + i;
        ODeviceButton b;

        if (( mb->model & d->m_model ) == d->m_model ) {
            b. setKeycode ( mb->code );
            b. setUserText ( QObject::tr ( "Button", mb->utext ));
            b. setPixmap ( OResource::loadPixmap ( mb->pix ));
            b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( mb->fpressedservice ), mb->fpressedaction ));
            b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( mb->fheldservice ), mb->fheldaction ));

            d->m_buttons->append ( b );
        }
    }
    reloadButtonMapping();
}

bool MyPal::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
    int newkeycode = keycode;

    switch ( keycode ) {
        // QT has strange screen coordinate system, so depending
        // on native device screen orientation, we need to rotate cursor keys
        case Key_Left :
        case Key_Right:
        case Key_Up   :
        case Key_Down : {
            int quarters;
            switch (d->m_rotation) {
                case Rot0:   quarters = 3/*270deg*/; break;
                case Rot90:  quarters = 2/*270deg*/; break;
                case Rot180: quarters = 1/*270deg*/; break;
                case Rot270: quarters = 0/*270deg*/; break;
            }
            newkeycode = Key_Left + ( keycode - Key_Left + quarters ) % 4;
            break;
        }

        // map Power Button short/long press
        case HardKey_Suspend: { // Hope we don't have infinite recursion here:
            if ( isPress ) {
                if ( m_power_timer )
                    killTimer ( m_power_timer );
                m_power_timer = startTimer ( 500 );
            }
            else if ( m_power_timer ) {
                killTimer ( m_power_timer );
                m_power_timer = 0;
                QWSServer::sendKeyEvent ( -1, HardKey_Suspend, 0, true, false );
                QWSServer::sendKeyEvent ( -1, HardKey_Suspend, 0, false, false );
            }
            newkeycode = Key_unknown;
            break;
        }
    }

    if ( newkeycode != keycode ) {
        if ( newkeycode != Key_unknown )
            QWSServer::sendKeyEvent ( -1, newkeycode, modifiers, isPress, autoRepeat );
        return true;
    }
    else
        return false;
}

void MyPal::timerEvent ( QTimerEvent * )
{
    killTimer ( m_power_timer );
    m_power_timer = 0;
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, true, false );
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, false, false );
}

void MyPal::playAlarmSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "alarm" );
    if(!snd.isFinished())
        return;

    changeMixerForAlarm(0, "/dev/sound/mixer", &snd );
    snd. play();
#endif
}

bool MyPal::setDisplayBrightness ( int bright )
{
    bool res = false;

    if ( bright > 255 )
        bright = 255;
    if ( bright < 0 )
        bright = 0;

    QDir sysClass( "/sys/class/backlight/" );
    sysClass.setFilter(QDir::Dirs);
    if ( sysClass.exists() && sysClass.count() > 2 ) {
        QString sysClassPath = sysClass.absFilePath( sysClass[2] + "/brightness" );
        int fd = ::open( sysClassPath, O_WRONLY|O_NONBLOCK );
        if ( fd >= 0 ) {
            char buf[100];
            int val = bright * displayBrightnessResolution() / 255;
            int len = ::snprintf( &buf[0], sizeof buf, "%d", val );
            if (len > 0)
                res = ( ::write( fd, &buf[0], len ) == 0 );
            ::close( fd );
        }
    }

    return res;
}

int MyPal::displayBrightnessResolution() const
{
    switch ( model()) {
        case Model_MyPal_716:
            return 255;
        default:
            return OAbstractMobileDevice::displayBrightnessResolution();
    }
}

bool MyPal::setDisplayStatus ( bool on )
{
    bool res = false;

    QDir sysClass( "/sys/class/lcd/" );
    sysClass.setFilter(QDir::Dirs);
    if ( sysClass.exists() && sysClass.count() > 2 ) {
        QString sysClassPath = sysClass.absFilePath( sysClass[2] + "/power" );
        int fd = ::open( sysClassPath, O_WRONLY|O_NONBLOCK );
        if ( fd >= 0 ) {
            char buf[10];
            buf[0] = on ? 0 : 4;
            buf[1] = '\0';
            res = ( ::write( fd, &buf[0], 2 ) == 0 );
            ::close( fd );
        }
    } 
    else {
        res = OAbstractMobileDevice::setDisplayStatus(on);
    }

    return res;
}
