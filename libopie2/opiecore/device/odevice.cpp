/*
                             This file is part of the Opie Project
                             Copyright (C) The Opie Team <opie-devel@handhelds.org>
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

#include "odevice.h"

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

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

// _IO and friends are only defined in kernel headers ...

#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

using namespace Opie;

class iPAQ;
class Zaurus;
class SIMpad;
class Ramses;
class Jornada;

ODevice *ODevice::inst()
{
    static ODevice *dev = 0;

    // rewrite this to only use /proc/devinfo or so

    /*
    if ( !dev ) {
        if ( QFile::exists ( "/proc/hal/model" ))
            dev = new iPAQ();
        else if ( Zaurus::isZaurus() )
            dev = new Zaurus();
        else if ( QFile::exists ( "/proc/ucb1x00" ) && QFile::exists ( "/proc/cs3" ))
            dev = new SIMpad();
        else if ( QFile::exists ( "/proc/sys/board/name" ))
            dev = new Ramses();
        else if ( Yopy::isYopy() )
                dev = new Yopy();
        else if ( Jornada::isJornada() )
            dev = new Jornada();
        else
            dev = new ODevice();
        dev->init();
    }
    */
    return dev;
}

ODevice::ODevice()
{
    d = new ODeviceData;

    d->m_modelstr = "Unknown";
    d->m_model = Model_Unknown;
    d->m_vendorstr = "Unknown";
    d->m_vendor = Vendor_Unknown;
    d->m_systemstr = "Unknown";
    d->m_system = System_Unknown;
    d->m_sysverstr = "0.0";
    d->m_rotation = Rot0;
    d->m_direction = CW;

    d->m_holdtime = 1000; // 1000ms
    d->m_buttons = 0;
    d->m_cpu_frequencies = new QStrList;
}

void ODevice::systemMessage ( const QCString &msg, const QByteArray & )
{
    if ( msg == "deviceButtonMappingChanged()" ) {
        reloadButtonMapping();
    }
}

void ODevice::init()
{
}

/**
* This method initialises the button mapping
*/
void ODevice::initButtons()
{
    if ( d->m_buttons )
        return;

    qDebug ( "init Buttons" );
    d->m_buttons = new QValueList <ODeviceButton>;

    reloadButtonMapping();

    QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
    connect ( sysch, SIGNAL( received( const QCString &, const QByteArray & )), this, SLOT( systemMessage ( const QCString &, const QByteArray & )));
}

ODevice::~ODevice()
{
// we leak m_devicebuttons and m_cpu_frequency
// but it's a singleton and it is not so importantant
// -zecke
    delete d;
}

bool ODevice::setSoftSuspend ( bool /*soft*/ )
{
    return false;
}

//#include <linux/apm_bios.h>

#define APM_IOC_SUSPEND          OD_IO( 'A', 2 )

/**
* This method will try to suspend the device
* It only works if the user is the QWS Server and the apm application
* is installed.
* It tries to suspend and then waits some time cause some distributions
* do have asynchronus apm implementations.
* This method will either fail and return false or it'll suspend the
* device and return once the device got woken up
*
* @return if the device got suspended
*/
bool ODevice::suspend()
{
    qDebug("ODevice::suspend");
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    if ( d->m_model == Model_Unknown ) // better don't suspend in qvfb / on unkown devices
        return false;

    bool res = false;

    struct timeval tvs, tvn;
    ::gettimeofday ( &tvs, 0 );

    ::sync(); // flush fs caches
    res = ( ::system ( "apm --suspend" ) == 0 );

    // This is needed because the iPAQ apm implementation is asynchronous and we
    // can not be sure when exactly the device is really suspended
    // This can be deleted as soon as a stable familiar with a synchronous apm implementation exists.

    if ( res ) {
        do { // wait at most 1.5 sec: either suspend didn't work or the device resumed
            ::usleep ( 200 * 1000 );
            ::gettimeofday ( &tvn, 0 );
        } while ((( tvn. tv_sec - tvs. tv_sec ) * 1000 + ( tvn. tv_usec - tvs. tv_usec ) / 1000 ) < 1500 );
    }

    return res;
}

//#include <linux/fb.h> better not rely on kernel headers in userspace ...

#define FBIOBLANK             OD_IO( 'F', 0x11 ) // 0x4611

/* VESA Blanking Levels */
#define VESA_NO_BLANKING      0
#define VESA_VSYNC_SUSPEND    1
#define VESA_HSYNC_SUSPEND    2
#define VESA_POWERDOWN        3

/**
* This sets the display on or off
*/
bool ODevice::setDisplayStatus ( bool on )
{
    qDebug("ODevice::setDisplayStatus(%d)", on);

    if ( d->m_model == Model_Unknown )
        return false;

    bool res = false;
    int fd;

    if (( fd = ::open ( "/dev/fb0", O_RDWR )) >= 0 ) {
        res = ( ::ioctl ( fd, FBIOBLANK, on ? VESA_NO_BLANKING : VESA_POWERDOWN ) == 0 );
        ::close ( fd );
    }
    return res;
}

/**
* This sets the display brightness
*
* @param p The brightness to be set on a scale from 0 to 255
* @return success or failure
*/
bool ODevice::setDisplayBrightness ( int p)
{
        Q_UNUSED( p )
    return false;
}

/**
* @return returns the number of steppings on the brightness slider
* in the Light-'n-Power settings.
*/
int ODevice::displayBrightnessResolution() const
{
    return 16;
}

/**
* This sets the display contrast
* @param p The contrast to be set on a scale from 0 to 255
* @return success or failure
*/
bool ODevice::setDisplayContrast ( int p)
{
        Q_UNUSED( p )
    return false;
}

/**
* @return return the max value for the brightness settings slider
*         or 0 if the device doesn't support setting of a contrast
*/
int ODevice::displayContrastResolution() const
{
    return 0;
}

/**
* This returns the vendor as string
* @return Vendor as QString
*/
QString ODevice::vendorString() const
{
    return d->m_vendorstr;
}

/**
* This returns the vendor as one of the values of OVendor
* @return OVendor
*/
OVendor ODevice::vendor() const
{
    return d->m_vendor;
}

/**
* This returns the model as a string
* @return A string representing the model
*/
QString ODevice::modelString() const
{
    return d->m_modelstr;
}

/**
* This does return the OModel used
*/
OModel ODevice::model() const
{
    return d->m_model;
}

/**
* This does return the systen name
*/
QString ODevice::systemString() const
{
    return d->m_systemstr;
}

/**
* Return System as OSystem value
*/
OSystem ODevice::system() const
{
    return d->m_system;
}

/**
* @return the version string of the base system
*/
QString ODevice::systemVersionString() const
{
    return d->m_sysverstr;
}

/**
*  @return the current Transformation
*/
Transformation ODevice::rotation() const
{
    return d->m_rotation;
}

/**
*  @return the current rotation direction
*/
ODirection ODevice::direction() const
{
    return d->m_direction;
}

/**
* This plays an alarmSound
*/
void ODevice::alarmSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "alarm" );

    if ( snd. isFinished())
        snd. play();
#endif
}

/**
* This plays a key sound
*/
void ODevice::keySound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "keysound" );

    if ( snd. isFinished())
        snd. play();
#endif
}

/**
* This plays a touch sound
*/
void ODevice::touchSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "touchsound" );

    if ( snd. isFinished())
        snd. play();
#endif
}

/**
* This method will return a list of leds
* available on this device
* @return a list of LEDs.
*/
QValueList <OLed> ODevice::ledList() const
{
    return QValueList <OLed>();
}

/**
* This does return the state of the LEDs
*/
QValueList <OLedState> ODevice::ledStateList ( OLed /*which*/ ) const
{
    return QValueList <OLedState>();
}

/**
* @return the state for a given OLed
*/
OLedState ODevice::ledState ( OLed /*which*/ ) const
{
    return Led_Off;
}

/**
* Set the state for a LED
* @param which Which OLed to use
* @param st The state to set
* @return success or failure
*/
bool ODevice::setLedState ( OLed which, OLedState st )
{
        Q_UNUSED( which )
        Q_UNUSED( st    )
    return false;
}

/**
* @return if the device has a light sensor
*/
bool ODevice::hasLightSensor() const
{
    return false;
}

/**
* @return a value from the light sensor
*/
int ODevice::readLightSensor()
{
    return -1;
}

/**
* @return the light sensor resolution
*/
int ODevice::lightSensorResolution() const
{
    return 0;
}

/**
* @return if the device has a hinge sensor
*/
bool ODevice::hasHingeSensor() const
{
    return false;
}

/**
* @return a value from the hinge sensor
*/
OHingeStatus ODevice::readHingeSensor()
{
    return CASE_UNKNOWN;
}

/**
* @return a list with CPU frequencies supported by the hardware
*/
const QStrList &ODevice::allowedCpuFrequencies() const
{
    return *d->m_cpu_frequencies;
}


/**
* Set desired CPU frequency
*
* @param index index into d->m_cpu_frequencies of the frequency to be set
*/
bool ODevice::setCurrentCpuFrequency(uint index)
{
    if (index >= d->m_cpu_frequencies->count())
        return false;

    char *freq = d->m_cpu_frequencies->at(index);
    qWarning("set freq to %s", freq);

    int fd;

    if ((fd = ::open("/proc/sys/cpu/0/speed", O_WRONLY)) >= 0) {
        char writeCommand[50];
        const int count = sprintf(writeCommand, "%s\n", freq);
        int res = (::write(fd, writeCommand, count) != -1);
        ::close(fd);
        return res;
    }

    return false;
}


/**
* @return a list of hardware buttons
*/
const QValueList <ODeviceButton> &ODevice::buttons()
{
    initButtons();

    return *d->m_buttons;
}

/**
* @return The amount of time that would count as a hold
*/
uint ODevice::buttonHoldTime() const
{
    return d->m_holdtime;
}

/**
* This method return a ODeviceButton for a key code
* or 0 if no special hardware button is available for the device
*
* @return The devicebutton or 0l
* @see ODeviceButton
*/
const ODeviceButton *ODevice::buttonForKeycode ( ushort code )
{
    initButtons();

    for ( QValueListConstIterator<ODeviceButton> it = d->m_buttons->begin(); it != d->m_buttons->end(); ++it ) {
        if ( (*it). keycode() == code )
            return &(*it);
    }
    return 0;
}

void ODevice::reloadButtonMapping()
{
    initButtons();

    Config cfg ( "ButtonSettings" );

    for ( uint i = 0; i < d->m_buttons->count(); i++ ) {
        ODeviceButton &b = ( *d->m_buttons ) [i];
        QString group = "Button" + QString::number ( i );

        QCString pch, hch;
        QCString pm, hm;
        QByteArray pdata, hdata;

        if ( cfg. hasGroup ( group )) {
            cfg. setGroup ( group );
            pch = cfg. readEntry ( "PressedActionChannel" ). latin1();
            pm  = cfg. readEntry ( "PressedActionMessage" ). latin1();
            // pdata = decodeBase64 ( buttonFile. readEntry ( "PressedActionArgs" ));

            hch = cfg. readEntry ( "HeldActionChannel" ). latin1();
            hm  = cfg. readEntry ( "HeldActionMessage" ). latin1();
            // hdata = decodeBase64 ( buttonFile. readEntry ( "HeldActionArgs" ));
        }

        b. setPressedAction ( OQCopMessage ( pch, pm, pdata ));

        b. setHeldAction ( OQCopMessage ( hch, hm, hdata ));
    }
}

void ODevice::remapPressedAction ( int button, const OQCopMessage &action )
{
    initButtons();

    QString mb_chan;

    if ( button >= (int) d->m_buttons->count())
        return;

    ODeviceButton &b = ( *d->m_buttons ) [button];
        b. setPressedAction ( action );

    mb_chan=b. pressedAction(). channel();

    Config buttonFile ( "ButtonSettings" );
    buttonFile. setGroup ( "Button" + QString::number ( button ));
    buttonFile. writeEntry ( "PressedActionChannel", (const char*) mb_chan);
    buttonFile. writeEntry ( "PressedActionMessage", (const char*) b. pressedAction(). message());

//	buttonFile. writeEntry ( "PressedActionArgs", encodeBase64 ( b. pressedAction(). data()));

    QCopEnvelope ( "QPE/System", "deviceButtonMappingChanged()" );
}

void ODevice::remapHeldAction ( int button, const OQCopMessage &action )
{
    initButtons();

    if ( button >= (int) d->m_buttons->count())
        return;

    ODeviceButton &b = ( *d->m_buttons ) [button];
        b. setHeldAction ( action );

    Config buttonFile ( "ButtonSettings" );
    buttonFile. setGroup ( "Button" + QString::number ( button ));
    buttonFile. writeEntry ( "HeldActionChannel", (const char *) b. heldAction(). channel());
    buttonFile. writeEntry ( "HeldActionMessage", (const char *) b. heldAction(). message());

//	buttonFile. writeEntry ( "HeldActionArgs", decodeBase64 ( b. heldAction(). data()));

    QCopEnvelope ( "QPE/System", "deviceButtonMappingChanged()" );
}
void ODevice::virtual_hook(int, void* ){

}
