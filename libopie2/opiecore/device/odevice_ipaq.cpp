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

#include "odevice_ipaq.h"

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

/* KERNEL */
#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

typedef struct {
  unsigned char OffOnBlink;       /* 0=off 1=on 2=Blink */
  unsigned char TotalTime;        /* Units of 5 seconds */
  unsigned char OnTime;           /* units of 100m/s */
  unsigned char OffTime;          /* units of 100m/s */
} LED_IN;

typedef struct {
        unsigned char mode;
        unsigned char pwr;
        unsigned char brightness;
} FLITE_IN;

#define LED_ON    OD_IOW( 'f', 5, LED_IN )
#define FLITE_ON  OD_IOW( 'f', 7, FLITE_IN )

struct i_button ipaq_buttons [] = {
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx | Model_iPAQ_H191x | Model_iPAQ_H22xx | Model_iPAQ_HX4700 | Model_iPAQ_H4xxx,
    Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
    "devicebuttons/ipaq_calendar",
    "datebook", "nextView()",
    "today", "raise()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx | Model_iPAQ_H191x | Model_iPAQ_H22xx | Model_iPAQ_HX4700 | Model_iPAQ_H4xxx,
    Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
    "devicebuttons/ipaq_contact",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Menu Button"),
    "devicebuttons/ipaq_menu",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx,
    Qt::Key_F13, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/ipaq_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx | Model_iPAQ_H191x | Model_iPAQ_H4xxx,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/ipaq_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx,
    Qt::Key_F24, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "devicebuttons/ipaq_record",
    "QPE/VMemo", "toggleRecord()",
    "sound", "raise()" },

    { Model_iPAQ_H191x | Model_iPAQ_H4xxx,
    Qt::Key_F8, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/ipaq_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { Model_iPAQ_H191x | Model_iPAQ_H4xxx,
    Qt::Key_F7, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "devicebuttons/ipaq_record",
    "QPE/VMemo", "toggleRecord()",
    "sound", "raise()" },

    //h2200 has different button mapping
    { Model_iPAQ_H22xx,
    Qt::Key_NumLock, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/ipaq_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { Model_iPAQ_H22xx,
    Qt::Key_ScrollLock, QT_TRANSLATE_NOOP("Button", "Menu Button"),
    "devicebuttons/ipaq_menu",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    // and hx4700 has different button mapping still
    { Model_iPAQ_HX4700,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/ipaq_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { Model_iPAQ_HX4700,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "devicebuttons/ipaq_record",
    "QPE/VMemo", "toggleRecord()",
    "sound", "raise()" },
    { Model_iPAQ_HX4700,
    Qt::Key_Home, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/ipaq_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },

};

void iPAQ::init(const QString& model)
{
    d->m_vendorstr = "HP";
    d->m_vendor = Vendor_HP;

    d->m_modelstr = model.mid(model.findRev('H'));

    if ( d->m_modelstr == "H3100" )
        d->m_model = Model_iPAQ_H31xx;
    else if ( d->m_modelstr == "H3600" )
        d->m_model = Model_iPAQ_H36xx;
    else if ( d->m_modelstr == "H3700" )
        d->m_model = Model_iPAQ_H37xx;
    else if ( d->m_modelstr == "H3800" )
        d->m_model = Model_iPAQ_H38xx;
    else if ( d->m_modelstr == "H3900" )
        d->m_model = Model_iPAQ_H39xx;
    else if ( d->m_modelstr == "H5400" )
        d->m_model = Model_iPAQ_H5xxx;
    else if ( d->m_modelstr == "H2200" )
        d->m_model = Model_iPAQ_H22xx;
    else if ( d->m_modelstr == "H1910" )
        d->m_model = Model_iPAQ_H191x;
    else if ( d->m_modelstr == "H1940" )
        d->m_model = Model_iPAQ_H1940;
    else if ( d->m_modelstr == "HX4700" )
        d->m_model = Model_iPAQ_HX4700;
    else if ( d->m_modelstr == "H4000" )
        d->m_model = Model_iPAQ_H4xxx;

    else
        d->m_model = Model_Unknown;

    switch ( d->m_model ) {
        case Model_iPAQ_H31xx:
        case Model_iPAQ_H38xx:
            d->m_rotation = Rot90;
            break;
        case Model_iPAQ_H5xxx:
        case Model_iPAQ_H22xx:
        case Model_iPAQ_H191x:
        case Model_iPAQ_H1940:
	case Model_iPAQ_HX4700:
	case Model_iPAQ_H4xxx:
            d->m_rotation = Rot0;
            break;
        case Model_iPAQ_H36xx:
        case Model_iPAQ_H37xx:
        case Model_iPAQ_H39xx:
        default:
            d->m_rotation = Rot270;
            break;

        }

    m_leds [0] = m_leds [1] = Led_Off;

    m_power_timer = 0;

}

void iPAQ::initButtons()
{
    if ( d->m_buttons )
        return;

    if ( isQWS( ) ) {
        addPreHandler(this);
    }

    d->m_buttons = new QValueList <ODeviceButton>;

    for ( uint i = 0; i < ( sizeof( ipaq_buttons ) / sizeof( i_button )); i++ ) {
        i_button *ib = ipaq_buttons + i;
        ODeviceButton b;

        if (( ib->model & d->m_model ) == d->m_model ) {
            b. setKeycode ( ib->code );
            b. setUserText ( QObject::tr ( "Button", ib->utext ));
            b. setPixmap ( OResource::loadPixmap ( ib->pix ));
            b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib->fpressedservice ), ib->fpressedaction ));
            b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib->fheldservice ), ib->fheldaction ));

            d->m_buttons->append ( b );
        }
    }
    reloadButtonMapping();
}

QValueList <OLed> iPAQ::ledList() const
{
    QValueList <OLed> vl;
    vl << Led_Power;

    if ( d->m_model == Model_iPAQ_H38xx )
        vl << Led_BlueTooth;
    return vl;
}

QValueList <OLedState> iPAQ::ledStateList ( OLed l ) const
{
    QValueList <OLedState> vl;

    if ( l == Led_Power )
        vl << Led_Off << Led_On << Led_BlinkSlow << Led_BlinkFast;
    else if ( l == Led_BlueTooth && d->m_model == Model_iPAQ_H38xx )
        vl << Led_Off; // << Led_On << ???

    return vl;
}

OLedState iPAQ::ledState ( OLed l ) const
{
    switch ( l ) {
        case Led_Power:
            return m_leds [0];
        case Led_BlueTooth:
            return m_leds [1];
        default:
            return Led_Off;
    }
}

bool iPAQ::setLedState ( OLed l, OLedState st )
{
    static int fd = ::open ( "/dev/touchscreen/0", O_RDWR | O_NONBLOCK );

    if ( l == Led_Power ) {
        if ( fd >= 0 ) {
            LED_IN leds;
            ::memset ( &leds, 0, sizeof( leds ));
            leds. TotalTime  = 0;
            leds. OnTime     = 0;
            leds. OffTime    = 1;
            leds. OffOnBlink = 2;

            switch ( st ) {
                case Led_Off      : leds. OffOnBlink = 0; break;
                case Led_On       : leds. OffOnBlink = 1; break;
                case Led_BlinkSlow: leds. OnTime = 10; leds. OffTime = 10; break;
                case Led_BlinkFast: leds. OnTime =  5; leds. OffTime =  5; break;
            }

            if ( ::ioctl ( fd, LED_ON, &leds ) >= 0 ) {
                m_leds [0] = st;
                return true;
            }
        }
    }
    return false;
}


bool iPAQ::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
    int newkeycode = keycode;

    switch ( keycode ) {
        // H38xx/H39xx have no "Q" key anymore - this is now the Mail key
        case HardKey_Menu: {
            if (( d->m_model == Model_iPAQ_H38xx ) ||
                ( d->m_model == Model_iPAQ_H39xx ) ||
                ( d->m_model == Model_iPAQ_H5xxx)) {
                newkeycode = HardKey_Mail;
            }
            break;
        }

        // Rotate cursor keys 180 or 270
        case Key_Left :
        case Key_Right:
        case Key_Up   :
        case Key_Down : {
            if (( d->m_model == Model_iPAQ_H31xx ) ||
                ( d->m_model == Model_iPAQ_H38xx )) {
                newkeycode = Key_Left + ( keycode - Key_Left + 2 ) % 4;
            }
            // Rotate the cursor keys by 270
            // keycode - Key_Left = position of the button starting from left clockwise
            // add the rotation to it and modolo. No we've the original offset
            // add the offset to the Key_Left key
            if (( d->m_model == Model_iPAQ_H5xxx ) ||
		( d->m_model == Model_iPAQ_H191x ) ||
		( d->m_model == Model_iPAQ_H4xxx ) ||
		( d->m_model == Model_iPAQ_H1940 ))
                newkeycode = Key_Left + ( keycode - Key_Left + 3 ) % 4;
            break;
        }

        // map Power Button short/long press to F6 for h191x
        case Key_F6:
            if ( d->m_model != Model_iPAQ_H191x && d->m_model != Model_iPAQ_H4xxx )
                break;
        // map Power Button short/long press to F34/F35
        case Key_SysReq: {
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

void iPAQ::timerEvent ( QTimerEvent * )
{
    killTimer ( m_power_timer );
    m_power_timer = 0;
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, true, false );
    QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, false, false );
}


void iPAQ::playAlarmSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "alarm" );
    if(!snd.isFinished())
	return;

    changeMixerForAlarm(0, "/dev/sound/mixer", &snd );
    snd. play();
#endif
}

bool iPAQ::setDisplayBrightness ( int bright )
{
    bool res = false;
    int fd;

    if ( bright > 255 )
        bright = 255;
    if ( bright < 0 )
        bright = 0;

    QString cmdline;

    switch ( model()) {
    case Model_iPAQ_H191x:
    case Model_iPAQ_H4xxx:
	{
	    QDir sysClass( "/sys/class/backlight/pxafb/" );
	    sysClass.setFilter(QDir::Dirs);
	    int fd;
	    if ( sysClass.exists() ) {
		QString sysClassPath = sysClass.absFilePath( "/sys/class/backlight/pxafb/power" );
		fd = ::open( sysClassPath, O_WRONLY | O_NONBLOCK );
		if ( fd ) {
		    char buf[10];
		    buf[0] = bright ? 0 : 4;
		    buf[1] = '\0';
		    res = ( ::write( fd, &buf[0], 2 ) == 0 );
		    ::close( fd );
		}
		sysClassPath = sysClass.absFilePath( "/sys/class/backlight/pxafb/brightness" );
		fd = ::open( sysClassPath, O_WRONLY | O_NONBLOCK );
		if ( fd ) {
		    char buf[100];
		    int len = ::snprintf( &buf[0], sizeof buf, "%d", bright );
		    res = ( ::write( fd, &buf[0], len ) == 0 );
		    ::close( fd );
		}
	    }
	}
        break; 
	
    case Model_iPAQ_HX4700:
            cmdline = QString::fromLatin1( "echo %1 > /sys/class/backlight/w100fb/brightness" ).arg( bright );
        // No Global::shellQuote as we gurantee it to be sane
        res = ( ::system( QFile::encodeName(cmdline) ) == 0 );
        break; 
	

    default:
        if (( fd = ::open ( "/dev/touchscreen/0", O_WRONLY )) >= 0 ) {
            FLITE_IN bl;
            bl. mode = 1;
            bl. pwr = bright ? 1 : 0;
            bl. brightness = ( bright * ( displayBrightnessResolution() - 1 ) + 127 ) / 255;
            res = ( ::ioctl ( fd, FLITE_ON, &bl ) == 0 );
            ::close ( fd );
        }
    }
    return res;
}

int iPAQ::displayBrightnessResolution() const
{
    switch ( model()) {
        case Model_iPAQ_H31xx:
        case Model_iPAQ_H36xx:
        case Model_iPAQ_H37xx:
            return 128;     // really 256, but >128 could damage the LCD

        case Model_iPAQ_H38xx:
        case Model_iPAQ_H39xx:
            return 64;
        case Model_iPAQ_H5xxx:
	case Model_iPAQ_HX4700:
	case Model_iPAQ_H4xxx:
            return 255;
        case Model_iPAQ_H191x:
            return 7;
        case Model_iPAQ_H1940:
            return 44;
        default:
            return 2;
    }
}

bool iPAQ::setDisplayStatus ( bool on )
{
    bool res = false;

    QString cmdline;

    if ( model() == Model_iPAQ_H191x ) {
	QDir sysClass( "/sys/class/lcd/pxafb/" );
	sysClass.setFilter(QDir::Dirs);
	if ( sysClass.exists() ) {
	    QString sysClassPath = sysClass.absFilePath( "/sys/class/lcd/pxafb/power" );
	    int fd = ::open( sysClassPath, O_WRONLY | O_NONBLOCK );
	    if ( fd ) {
		char buf[10];
		buf[0] = on ? 0 : 4;
		buf[1] = '\0';
		res = ( ::write( fd, &buf[0], 2 ) == 0 );
		::close( fd );
	    }
	}
	return res;
    } else {
        return OAbstractMobileDevice::setDisplayStatus(on);
    }

    res = ( ::system( QFile::encodeName(cmdline) ) == 0 );

    return res;
}

bool iPAQ::hasLightSensor() const
{
    switch (model()) {
	case Model_iPAQ_H191x:
	case Model_iPAQ_H4xxx:
	    return false;
	default:
	    return true;
    }
}

int iPAQ::readLightSensor()
{
    int fd;
    int val = -1;

    if (( fd = ::open ( "/proc/hal/light_sensor", O_RDONLY )) >= 0 ) {
        char buffer [8];

        if ( ::read ( fd, buffer, 5 ) == 5 ) {
            char *endptr;

            buffer [4] = 0;
            val = ::strtol ( buffer + 2, &endptr, 16 );

            if ( *endptr != 0 )
                val = -1;
        }
        ::close ( fd );
    }

    return val;
}

int iPAQ::lightSensorResolution() const
{
    return 256;
}
