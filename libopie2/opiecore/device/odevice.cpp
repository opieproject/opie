/*
                             This file is part of the Opie Project
             =.              (C) 2002-2006 The Opie Team <opie-devel@lists.sourceforge.net>
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


#include "odevice_beagle.h"
#include "odevice_ipaq.h"
#include "odevice_mypal.h"
#include "odevice_jornada.h"
#include "odevice_simpad.h"
#include "odevice_yopy.h"
#include "odevice_zaurus.h"
#include "odevice_genuineintel.h"
#include "odevice_htc.h"
#include "odevice_motorola_ezx.h"
#include "odevice_palm.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>
#include <qdir.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/sound.h>

#include <opie2/okeyfilter.h>
#include <opie2/oresource.h>
#include <opie2/odebug.h>
#include <opie2/oprocess.h>

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
#include <errno.h>
#endif

namespace Opie {
namespace Core {

static const char* PATH_PROC_CPUINFO = "/proc/cpuinfo";


/* STATIC and common implementation */
/* EXPORT */ ODistribution distributions[] = {
    { System_Familiar,        "FamiliarLinux",   "/etc/familiar-version" },
    { System_OpenZaurus,      "OpenZaurus",      "/etc/openzaurus-version" },
    { System_OpenEmbedded,    "OpenEmbedded",    "/etc/oe-version" },
    { System_Angstrom,        "Angstrom",        "/etc/angstrom-version" },
    { System_Unknown,         "Linux",           "/etc/issue" },
};


/* EXPORT */ bool isQWS(){
    return qApp ? ( qApp->type() == QApplication::GuiServer ) : false;
}

/* EXPORT */ QCString makeChannel ( const char *str ){
    if ( str && !::strchr ( str, '/' ))
        return QCString ( "QPE/Application/" ) + str;
    else
        return str;
}



/* Now the default implementation of ODevice */

struct default_button default_buttons [] =  {
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

ODevice *ODevice::inst()
{
    static ODevice *dev = 0;
    QString cpu_info;

    if ( !dev )
    {
        QFile f( PATH_PROC_CPUINFO );
        if ( f.open( IO_ReadOnly ) )
        {
            QTextStream s( &f );
            while ( !s.atEnd() )
            {
                QString line;
                line = s.readLine();
                if ( line.startsWith( "Hardware" ) )
                {
                    qDebug( "ODevice() - found '%s'", (const char*) line );
                    cpu_info = line;
                    if ( line.contains( "sharp", false ) ) dev = new Internal::Zaurus();
                    else if ( line.contains( "ipaq", false ) ) dev = new Internal::iPAQ();
                    else if ( line.contains( "mypal", false ) ) dev = new Internal::MyPal();
                    else if ( line.contains( "simpad", false ) ) dev = new Internal::SIMpad();
                    else if ( line.contains( "jornada", false ) ) dev = new Internal::Jornada();
                    else if ( line.contains( "Tradesquare.NL", false ) ) dev = new Internal::Beagle();
                    else if ( line.contains( "HTC", false ) ) dev = new Internal::HTC();
                    else if ( line.contains( "Motorola", false ) ) dev = new Internal::Motorola_EZX();
                    else if ( line.contains( "Palm", false ) ) dev = new Internal::Palm();

                    else qWarning( "ODevice() - unknown hardware - using default." );
                    break;
                } else if ( line.startsWith( "vendor_id" ) ) {
                    qDebug( "ODevice() - found '%s'", (const char*) line );
                    cpu_info = line;
                    if( line.contains( "genuineintel", false ) ) {
                      dev = new Internal::GenuineIntel();
                      break;
                    }
                }
            }
        }
        else
        {
            qWarning( "ODevice() - can't open '%s' - unknown hardware - using default.", PATH_PROC_CPUINFO );
        }
        if ( !dev ) dev = new ODevice();
        dev->init(cpu_info);
    }
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
    d->m_qteDriver = "Transformed";

    d->m_holdtime = 1000; // 1000ms
    d->m_buttons = 0;
    d->m_buttonCombos = 0;
    d->m_cpu_frequencies = new QStrList;
    d->m_brightnessRes = 0;

    /* System QCopChannel created */
    d->m_initializedButtonQcop = false;

    // New distribution detection code first checks for legacy distributions,
    // identified by /etc/familiar-version or /etc/oz_version.
    // Then check for OpenEmbedded and lastly, read /etc/issue

    for ( unsigned int i = 0; i < sizeof(distributions)/sizeof(ODistribution); ++i )
    {
        if ( QFile::exists( distributions[i].sysvfile ) )
        {
            d->m_systemstr = distributions[i].sysstr;
            d->m_system = distributions[i].system;
            d->m_sysverstr = "<Unknown>";
            QFile f( distributions[i].sysvfile );
            if ( f.open( IO_ReadOnly ) )
            {
                QTextStream ts( &f );
                d->m_sysverstr = ts.readLine().replace( QRegExp( "\\\\." ), "" );
            }
            break;
        }
    }

    d->m_heldCombo = 0;
    d->m_heldComboTimer = new QTimer( this );
    connect( d->m_heldComboTimer, SIGNAL(timeout()), this, SLOT(holdCheckCombo()) );

    QDir sysClass( "/sys/class/backlight/" );
    sysClass.setFilter(QDir::Dirs);
    if ( sysClass.exists() && sysClass.count() > 2 )
        d->m_backlightDev = sysClass.absFilePath( sysClass[2] );

    sysClass.setPath( "/sys/class/leds/" );
    if( sysClass.exists() ) {
        sysClass.setFilter(QDir::Dirs);
        QStringList dirs = sysClass.entryList( QDir::Dirs );
        for ( QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
            if( ! (*it).startsWith("."))
                d->m_ledState.insert( (*it), Led_Off );
        }
    }

    d->m_ledTimers.setAutoDelete(true);
}

void ODevice::systemMessage( const QCString &msg, const QByteArray & )
{
    if ( msg == "deviceButtonMappingChanged()" ) {
        reloadButtonMapping();
    }
}

void ODevice::init(const QString&)
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
    for ( uint i = 0; i < ( sizeof( default_buttons ) / sizeof( default_button )); i++ ) {
        default_button *db = default_buttons + i;
        ODeviceButton b;
        b.setKeycode( db->code );
        b.setUserText( QObject::tr ( "Button", db->utext ));
        b.setPixmap( OResource::loadPixmap ( db->pix ));
        b.setFactoryPresetPressedAction( OQCopMessage ( makeChannel ( db->fpressedservice ), db->fpressedaction ));
        b.setFactoryPresetHeldAction( OQCopMessage ( makeChannel ( db->fheldservice ), db->fheldaction ));
        d->m_buttons->append( b );
    }

    reloadButtonMapping();
}

/**
* This method initialises the special combo mapping
*/
void ODevice::initButtonCombos()
{
    if ( d->m_buttonCombos )
        return;

    d->m_buttonCombos = new QValueList<ODeviceButtonCombo>;
}

void ODevice::loadButtonCombos( const ODeviceButtonComboStruct combos[], uint length )
{
    for ( uint i = 0; i < length; i++ ) {
        const ODeviceButtonComboStruct *cs = combos + i;
        ODeviceButtonCombo c;

        if (( cs->model & d->m_model ) == d->m_model ) {
            c.setKeycodes( cs->keycode1, cs->keycode2, cs->keycode3 );
            c.setHold( cs->hold );
            c.setKeycodeDesc( QObject::tr ( "Button", cs->keycodedesc ));
            c.setAction( OQCopMessage( makeChannel ( cs->actionchan ), cs->actionmsg ));
            c.setActionDesc( QObject::tr ( "ComboAction", cs->actiondesc ));
            c.setLockedOnly( cs->lockedonly );

            d->m_buttonCombos->append( c );
        }
    }
}

ODevice::~ODevice()
{
    // we leak m_devicebuttons and m_cpu_frequency
    // but it's a singleton and it is not so importantant
    // -zecke
    delete d;
}

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
    return false; // default implementation == unknown device or qvfb
}

/**
* This sets the display on or off
*/
bool ODevice::setDisplayStatus( bool on )
{
    QDir sysClass( "/sys/class/lcd/" );
    sysClass.setFilter(QDir::Dirs);
    if ( sysClass.exists() && sysClass.count() > 2 ) {
        QString sysClassPath = sysClass.absFilePath( sysClass[2] + "/power" );
        int fd = ::open( sysClassPath, O_WRONLY|O_NONBLOCK );
        if ( fd >= 0 ) {
            char buf[10];
            bool res;
            buf[0] = on ? 0 : 4;
            buf[1] = '\0';
            res = ( ::write( fd, &buf[0], 2 ) == 0 );
            ::close( fd );
            return res;
        }
    }

    return false; // don't do anything
}

/**
* This sets the display brightness
*
* @param b The brightness to be set on a scale from 0 to 255
* @return success or failure
*/
bool ODevice::setDisplayBrightness( int bright )
{
    bool res = false;

    if ( bright > 255 )
        bright = 255;
    if ( bright < 0 )
        bright = 0;

    if ( d->m_backlightDev != "" ) {
        int fd = ::open( d->m_backlightDev + "/brightness", O_WRONLY|O_NONBLOCK );
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

/**
 *
 * @returns the number of steppings on the brightness slider
 * in the Light-'n-Power settings. Values smaller than zero and bigger
 * than 255 do not make sense.
 *
 * \sa QSlider::setLineStep
 * \sa QSlider::setPageStep
 */
int ODevice::displayBrightnessResolution() const
{
    if( d->m_brightnessRes > 0 )
        return d->m_brightnessRes;

    int res = 16;
    if ( d->m_backlightDev != "" ) {
        int fd = ::open( d->m_backlightDev + "/max_brightness", O_RDONLY|O_NONBLOCK );
        if ( fd >= 0 ) {
            char buf[100];
            if ( ::read( fd, &buf[0], sizeof buf ) )
                ::sscanf( &buf[0], "%d", &res );
            ::close( fd );
        }
        d->m_brightnessRes = res;
        return res;
    }

    qDebug( "ODevice::displayBrightnessResolution() - no backlight device" );
    return 16;
}

/**
* This sets the display contrast
* @param p The contrast to be set on a scale from 0 to 255
* @returns success or failure
*/
bool ODevice::setDisplayContrast( int p )
{
    qDebug( "ODevice::setDisplayContrast( %d ) - please override me.", p );
    return false;
}

/**
* @returns the maximum value for the contrast settings slider
*         or 0 if the device doesn't support setting of a contrast
*/
int ODevice::displayContrastResolution() const
{
    qDebug( "ODevice::displayContrastResolution() - please override me." );
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
* This does return the system name
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

QString ODevice::qteDriver() const
{
    return d->m_qteDriver;
}

/**
* This plays an alarm sound
*/
void ODevice::playAlarmSound()
{
#ifndef QT_NO_SOUND
    odebug << "playAlarmSound" << oendl;
    static Sound snd ( "alarm" );

    if ( snd. isFinished())
        snd. play();
#endif
}

/**
* This plays a key sound
*/
void ODevice::playKeySound()
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
void ODevice::playTouchSound()
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
QStringList ODevice::ledList() const
{
    QDir sysClass( "/sys/class/leds/" );
    QStringList leds;
    if( sysClass.exists() ) {
        sysClass.setFilter(QDir::Dirs);
        QStringList dirs = sysClass.entryList( QDir::Dirs );
        for ( QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
            if( ! (*it).startsWith("."))
                leds += (*it);
        }
    }
    return leds;
}

/**
* @return the state for a given LED
*/
OLedState ODevice::ledState ( const QString &led ) const
{
    return d->m_ledState[led];
}

/**
* Set the state for a LED
* @param which Which LED to use
* @param st The state to set
* @return success or failure
*/
bool ODevice::setLedState ( const QString &led, OLedState st )
{
    bool res = setLedInternal( led, (st != Led_Off) );

    if( res && ( st == Led_BlinkSlow || st == Led_BlinkFast || st == Led_BlinkSuperSlow || st == Led_BlinkSuperFast )) {
        OLedTimer *timer = d->m_ledTimers.find(led);
        if( !timer ) {
            timer = new OLedTimer( led, this );
            d->m_ledTimers.insert( led, timer );
        }
        else
            timer->stop();

        if (st == Led_BlinkSlow)
            timer->start( 1000 );
        else if (st == Led_BlinkFast)
            timer->start( 250 );
        else if (st == Led_BlinkSuperSlow)
            timer->start( 2000 );
        else if (st == Led_BlinkSuperFast)
            timer->start( 100 );
    }
    else
        d->m_ledTimers.remove( led );

    return res;
}

bool ODevice::setLedInternal ( const QString &led, bool on )
{
    int bright = 0;
    int fd;
    bool res = true;

    if( on ) {
        res = false;
        bright = 255;
        fd = ::open( d->m_backlightDev + "/max_brightness", O_RDONLY|O_NONBLOCK );
        if ( fd >= 0 ) {
            char buf[100];
            if ( ::read( fd, &buf[0], 10 ) )
                res = (::sscanf( &buf[0], "%d", &bright ) != 0);
            ::close( fd );
        }
    }

    if( res ) {
        fd = ::open( "/sys/class/leds/" + led + "/brightness", O_WRONLY|O_NONBLOCK );
        if ( fd >= 0 ) {
            char buf[10];
            int len = ::snprintf(buf, 10, "%d", bright);
            res = ( ::write( fd, &buf[0], len ) != 0 );
            ::close( fd );
        }
    }

    return res;
}

OLedTimer::OLedTimer ( const QString &led, ODevice *odev, const char * name )
    : QTimer( odev, name )
{
    this->on = true; // we start on
    this->led = led;
    this->odev = odev;

    connect( this, SIGNAL(timeout()), this, SLOT(fire()) );
}

void OLedTimer::fire()
{
    this->on = ! this->on;
    odev->setLedInternal( this->led, this->on );
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
OHingeStatus ODevice::readHingeSensor()const
{
    return CASE_UNKNOWN;
}

/**
* @return if the device has wave audio speaker (we're mostly interested for alarms)
*/
bool ODevice::hasWaveSpeaker() const
{
    return true;
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
* @return a list of special button combos
*/
const QValueList <ODeviceButtonCombo> &ODevice::buttonCombos()
{
    initButtonCombos();

    return *d->m_buttonCombos;
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
        if ( (*it).keycode() == code )
            return &(*it);
    }
    return 0;
}

bool ODevice::comboKeyEvent( ushort keycode, bool press, bool locked )
{
    initButtonCombos();

    for( QValueListIterator<ODeviceButtonCombo> it = d->m_buttonCombos->begin(); it != d->m_buttonCombos->end(); ++it ) {
        if( (*it).keyCodeEvent( keycode, press, locked ) ) {
            if( (*it).hold() ) {
                // This combo needs to be held down to activate
                if( press ) {
                    d->m_heldCombo = &(*it);
                    d->m_heldComboTimer->start( d->m_holdtime, true );
                }
                else
                    d->m_heldComboTimer->stop();
            }
            return true;
        }
    }
    return false;
}

void ODevice::holdCheckCombo()
{
    d->m_heldCombo->checkActivate();
}

void ODevice::reloadButtonMapping()
{
    if(!d->m_buttons)
        initButtons();

    if(!d->m_initializedButtonQcop) {
        QCopChannel *chan = new QCopChannel("QPE/System", this, "ODevice button channel");
        connect(chan,SIGNAL(received(const QCString&,const QByteArray&)),
            this,SLOT(systemMessage(const QCString&,const QByteArray&)));
        d->m_initializedButtonQcop = true;
    }

    Config cfg ( "ButtonSettings" );

    for ( uint i = 0; i < d->m_buttons->count(); i++ ) {
        ODeviceButton &b = ( *d->m_buttons ) [i];
        QString group = "Button" + QString::number ( i );

        QCString pch, hch;
        QCString pm, hm;
        QByteArray pdata, hdata;

        if ( cfg. hasGroup ( group )) {
            cfg. setGroup ( group );
            pch = cfg.readEntry ( "PressedActionChannel" ). latin1();
            pm  = cfg.readEntry ( "PressedActionMessage" ). latin1();
            // pdata = decodeBase64 ( buttonFile. readEntry ( "PressedActionArgs" ));

            hch = cfg.readEntry ( "HeldActionChannel" ). latin1();
            hm  = cfg.readEntry ( "HeldActionMessage" ). latin1();
            // hdata = decodeBase64 ( buttonFile. readEntry ( "HeldActionArgs" ));
        }

        b.setPressedAction( OQCopMessage ( pch, pm, pdata ));
        b.setHeldAction( OQCopMessage ( hch, hm, hdata ));
    }
}

void ODevice::remapPressedAction ( int button, const OQCopMessage &action )
{
    initButtons();

    QString mb_chan;

    if ( button >= (int) d->m_buttons->count())
        return;

    ODeviceButton &b = ( *d->m_buttons ) [button];
    b.setPressedAction( action );

    mb_chan = b.pressedAction(). channel();

    Config buttonFile ( "ButtonSettings" );
    buttonFile.setGroup ( "Button" + QString::number ( button ));
    buttonFile.writeEntry ( "PressedActionChannel", (const char*) mb_chan);
    buttonFile.writeEntry ( "PressedActionMessage", (const char*) b.pressedAction(). message());

//  buttonFile.writeEntry( "PressedActionArgs", encodeBase64 ( b.pressedAction(). data()));

    QCopEnvelope( "QPE/System", "deviceButtonMappingChanged()" );
}

void ODevice::remapHeldAction ( int button, const OQCopMessage &action )
{
    initButtons();

    if ( button >= (int) d->m_buttons->count())
        return;

    ODeviceButton &b = ( *d->m_buttons ) [button];
    b.setHeldAction( action );

    Config buttonFile( "ButtonSettings" );
    buttonFile.setGroup( "Button" + QString::number ( button ));
    buttonFile.writeEntry( "HeldActionChannel", (const char *) b.heldAction(). channel());
    buttonFile.writeEntry( "HeldActionMessage", (const char *) b.heldAction(). message());

//  buttonFile.writeEntry( "HeldActionArgs", decodeBase64 ( b.heldAction(). data()));

    QCopEnvelope( "QPE/System", "deviceButtonMappingChanged()" );
}

/**
 * @internal
 */
void ODevice::virtual_hook(int, void* ){

}

/**
 * \brief Send a QCOP Message before suspending
 *
 * Sends a QCOP message to channel QPE/System
 * with the message "aboutToSuspend()" if this
 * is the windowing server.
 *
 * Call this in your custom \sa suspend() Method
 * before going to suspend.
 *
 */
void ODevice::sendSuspendmsg()
{
    if ( !isQWS() )
        return;

    QCopEnvelope ( "QPE/System", "aboutToSuspend()" );
}

/**
 * \brief Prepend the QWSServer::KeyboardFilter to the list of installed KeyFilters
 *
 * Prepend a QWSServer::KeyboardFilter to the List of Keyboard
 * Filters. This function is the only way to prepend a KeyFilter.
 *
 * @param aFilter The KeyFilter to be prepended to the list of filters
 *
 * @see Opie::Core::OKeyFilter
 * @see Opie::Core::OKeyFilter::inst()
 */
void ODevice::addPreHandler(QWSServer::KeyboardFilter*aFilter)
{
    Opie::Core::OKeyFilter::inst()->addPreHandler(aFilter);
}

/**
 * \brief Remove the QWSServer::KeyboardFilter in the param from the list
 *
 * Remove the QWSServer::KeyboardFilter \par aFilter from the List
 * of Keyfilters. Call this when you delete the KeyFilter!
 *
 * @param aFilter The filter to be removed from the Opie::Core::OKeyFilter
 * @see Opie::Core::ODevice::addPreHandler
 */
void ODevice::remPreHandler(QWSServer::KeyboardFilter*aFilter)
{
    Opie::Core::OKeyFilter::inst()->remPreHandler(aFilter);
}

/**
 * \brief Perform standard APM suspend
 *
 * Suspends the device by calling apm --suspend (asynchronously, while
 * continuing to process events, allowing suspend notification scripts to
 * work).
 *
 * @param timeout max time to wait after calling apm --suspend (for older
 *  async apm implementations) or 0 to disable waiting
 * @return true on success, false on failure
 */
bool ODevice::apmSuspend( int timeout )
{
    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    bool res = false;
    sendSuspendmsg();

    struct timeval tvs, tvn;
    ::gettimeofday ( &tvs, 0 );

    ::sync(); // flush fs caches

    OProcess *process = new OProcess();
    *process << "/usr/bin/apm" << "--suspend";
    if (process->start()) {
        while( process->isRunning() ) {
           qApp->processEvents();
        }
        if( process->normalExit() )
            res = (process->exitStatus() == 0);
    }
    else
        owarn << "apm suspend execution failed" << oendl;

    if ( res && timeout > 0 ) {
        // This is needed because some apm implementations are asynchronous and we
        // can not be sure when exactly the device is really suspended
        // This can be deleted as soon as a stable familiar with a synchronous apm implementation exists.

        do { // wait for timeout, after which either suspend didn't work or the device resumed
            ::usleep ( 200 * 1000 );
            ::gettimeofday ( &tvn, 0 );
        } while ((( tvn. tv_sec - tvs. tv_sec ) * 1000 + ( tvn. tv_usec - tvs. tv_usec ) / 1000 ) < timeout );
    }    

    QCopChannel::send( "QPE/System", "returnFromSuspend()" );

    return res;
}


}
}
