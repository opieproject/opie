/*  This file is part of the OPIE libraries
    Copyright (C) 2002 Robert Griebl (sandman@handhelds.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/time.h>
#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif
#include <math.h>


#include <qfile.h>
#include <qtextstream.h>
#include <qpe/sound.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>

#include "odevice.h"

#include <qwindowsystem_qws.h>

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

class ODeviceData {
public:
	QString m_vendorstr;
	OVendor m_vendor;

	QString m_modelstr;
	OModel m_model;

	QString m_systemstr;
	OSystem m_system;

	QString m_sysverstr;

	Transformation m_rotation;
	ODirection m_direction;

	QValueList <ODeviceButton> *m_buttons;
	uint                        m_holdtime;
	QStrList                   *m_cpu_frequencies;

};

class iPAQ : public ODevice, public QWSServer::KeyboardFilter {
protected:
	virtual void init ( );
	virtual void initButtons ( );

public:
	virtual bool setSoftSuspend ( bool soft );

	virtual bool setDisplayBrightness ( int b );
	virtual int displayBrightnessResolution ( ) const;

	virtual void alarmSound ( );

	virtual QValueList <OLed> ledList ( ) const;
	virtual QValueList <OLedState> ledStateList ( OLed led ) const;
	virtual OLedState ledState ( OLed led ) const;
	virtual bool setLedState ( OLed led, OLedState st );

	virtual bool hasLightSensor ( ) const;
	virtual int readLightSensor ( );
	virtual int lightSensorResolution ( ) const;

protected:
	virtual bool filter ( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
	virtual void timerEvent ( QTimerEvent *te );

	int m_power_timer;

	OLedState m_leds [2];
};

class Jornada : public ODevice {
protected:
	virtual void init ( );
	//virtual void initButtons ( );
public:
	virtual bool setSoftSuspend ( bool soft );
	virtual bool setDisplayBrightness ( int b );
	virtual int displayBrightnessResolution ( ) const;
	static bool isJornada();

};

class Zaurus : public ODevice {
protected:
	virtual void init ( );
	virtual void initButtons ( );

public:
	virtual bool setSoftSuspend ( bool soft );

	virtual bool setDisplayBrightness ( int b );
	virtual int displayBrightnessResolution ( ) const;

	virtual void alarmSound ( );
	virtual void keySound ( );
	virtual void touchSound ( );

	virtual QValueList <OLed> ledList ( ) const;
	virtual QValueList <OLedState> ledStateList ( OLed led ) const;
	virtual OLedState ledState ( OLed led ) const;
	virtual bool setLedState ( OLed led, OLedState st );

	bool hasHingeSensor() const;
	OHingeStatus readHingeSensor();

	static bool isZaurus();

	// Does this break BC?
	virtual bool suspend ( );
	Transformation rotation ( ) const;
	ODirection direction ( ) const;

protected:
	virtual void buzzer ( int snd );

	OLedState m_leds [1];
	bool m_embedix;
        void virtual_hook( int id, void *data );
};

class SIMpad : public ODevice, public QWSServer::KeyboardFilter {
protected:
	virtual void init ( );
	virtual void initButtons ( );

public:
	virtual bool setSoftSuspend ( bool soft );
	virtual bool suspend();

	virtual bool setDisplayStatus( bool on );
	virtual bool setDisplayBrightness ( int b );
	virtual int displayBrightnessResolution ( ) const;

	virtual void alarmSound ( );

	virtual QValueList <OLed> ledList ( ) const;
	virtual QValueList <OLedState> ledStateList ( OLed led ) const;
	virtual OLedState ledState ( OLed led ) const;
	virtual bool setLedState ( OLed led, OLedState st );

protected:
	virtual bool filter ( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
	virtual void timerEvent ( QTimerEvent *te );

	int m_power_timer;

	OLedState m_leds [1]; //FIXME check if really only one
};

class Ramses : public ODevice, public QWSServer::KeyboardFilter {
protected:
	virtual void init ( );

public:
	virtual bool setSoftSuspend ( bool soft );
	virtual bool suspend ( );

	virtual bool setDisplayStatus( bool on );
	virtual bool setDisplayBrightness ( int b );
	virtual int displayBrightnessResolution ( ) const;
	virtual bool setDisplayContrast ( int b );
	virtual int displayContrastResolution ( ) const;

protected:
	virtual bool filter ( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
	virtual void timerEvent ( QTimerEvent *te );

	int m_power_timer;
};

struct i_button {
	uint model;
	Qt::Key code;
	char *utext;
	char *pix;
	char *fpressedservice;
	char *fpressedaction;
	char *fheldservice;
	char *fheldaction;
} ipaq_buttons [] = {
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx,
    Qt::Key_F9, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
	"devicebuttons/ipaq_calendar",
	"datebook", "nextView()",
	"today", "raise()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx,
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
	"mail", "raise()",
	"mail", "newMail()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
	"devicebuttons/ipaq_home",
	"QPE/Launcher", "home()",
	"buttonsettings", "raise()" },
    { Model_iPAQ_H31xx | Model_iPAQ_H36xx | Model_iPAQ_H37xx | Model_iPAQ_H38xx | Model_iPAQ_H39xx | Model_iPAQ_H5xxx,
    Qt::Key_F24, QT_TRANSLATE_NOOP("Button", "Record Button"),
	"devicebuttons/ipaq_record",
	"QPE/VMemo", "toggleRecord()",
	"sound", "raise()" },
};

struct z_button {
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
} z_buttons [] = {
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

struct s_button {
    uint model;
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
} simpad_buttons [] = {
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
    "mail", "raise()",
    "mail", "newMail()" },

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

struct r_button {
	uint model;
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
} ramses_buttons [] = {
    { Model_Ramses_MNCI,
    Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "Menu Button"),
	"devicebuttons/z_menu",
 	"QPE/TaskBar", "toggleMenu()",
	"QPE/TaskBar", "toggleStartMenu()" },
    { Model_Ramses_MNCI,
    Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "Home Button"),
	"devicebuttons/ipaq_home",
	"QPE/Launcher", "home()",
	"buttonsettings", "raise()" },
};

class Yopy : public ODevice {
protected:
  virtual void init ( );
  virtual void initButtons ( );

public:
  virtual bool suspend ( );

  virtual bool setDisplayBrightness ( int b );
  virtual int displayBrightnessResolution ( ) const;

  static bool isYopy ( );
};

struct yopy_button {
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
} yopy_buttons [] = {
  { Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Action Button"),
    "devicebuttons/yopy_action",
    "datebook", "nextView()",
    "today", "raise()" },
  { Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "OK Button"),
    "devicebuttons/yopy_ok",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
  { Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "End Button"),
    "devicebuttons/yopy_end",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
};

static QCString makeChannel ( const char *str )
{
	if ( str && !::strchr ( str, '/' ))
		return QCString ( "QPE/Application/" ) + str;
	else
		return str;
}

static inline bool isQWS()
{
	return qApp ? ( qApp-> type ( ) == QApplication::GuiServer ) : false;
}

ODevice *ODevice::inst ( )
{
	static ODevice *dev = 0;

	if ( !dev ) {
		if ( QFile::exists ( "/proc/hal/model" ))
			dev = new iPAQ ( );
		else if ( Zaurus::isZaurus() )
			dev = new Zaurus ( );
		else if ( QFile::exists ( "/proc/ucb1x00" ) && QFile::exists ( "/proc/cs3" ))
			dev = new SIMpad ( );
		else if ( QFile::exists ( "/proc/sys/board/name" ))
			dev = new Ramses ( );
		else if ( Yopy::isYopy() )
		        dev = new Yopy ( );
		else if ( Jornada::isJornada() )
			dev = new Jornada ( );
		else
			dev = new ODevice ( );
		dev-> init ( );
	}
	return dev;
}


/**************************************************
 *
 * common
 *
 **************************************************/


ODevice::ODevice ( )
{
	d = new ODeviceData;

	d-> m_modelstr = "Unknown";
	d-> m_model = Model_Unknown;
	d-> m_vendorstr = "Unknown";
	d-> m_vendor = Vendor_Unknown;
	d-> m_systemstr = "Unknown";
	d-> m_system = System_Unknown;
	d-> m_sysverstr = "0.0";
	d-> m_rotation = Rot0;
	d-> m_direction = CW;

	d-> m_holdtime = 1000; // 1000ms
	d-> m_buttons = 0;
	d-> m_cpu_frequencies = new QStrList;
}

void ODevice::systemMessage ( const QCString &msg, const QByteArray & )
{
	if ( msg == "deviceButtonMappingChanged()" ) {
		reloadButtonMapping ( );
	}
}

void ODevice::init ( )
{
}

/**
 * This method initialises the  button mapping
 */
void ODevice::initButtons ( )
{
	if ( d-> m_buttons )
		return;

	// Simulation uses iPAQ 3660 device buttons

	qDebug ( "init Buttons" );
	d-> m_buttons = new QValueList <ODeviceButton>;

	for ( uint i = 0; i < ( sizeof( ipaq_buttons ) / sizeof( i_button )); i++ ) {
		i_button *ib = ipaq_buttons + i;
		ODeviceButton b;

		if (( ib-> model & Model_iPAQ_H36xx ) == Model_iPAQ_H36xx ) {
			b. setKeycode ( ib-> code );
			b. setUserText ( QObject::tr ( "Button", ib-> utext ));
			b. setPixmap ( Resource::loadPixmap ( ib-> pix ));
			b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib-> fpressedservice ), ib-> fpressedaction ));
			b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib-> fheldservice ), ib-> fheldaction ));
			d-> m_buttons-> append ( b );
		}
	}
	reloadButtonMapping ( );

	QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
	connect ( sysch, SIGNAL( received(const QCString&,const QByteArray&)), this, SLOT( systemMessage(const QCString&,const QByteArray&)));
}

ODevice::~ODevice ( )
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
bool ODevice::suspend ( )
{
	qDebug("ODevice::suspend");
	if ( !isQWS( ) ) // only qwsserver is allowed to suspend
		return false;

	if ( d-> m_model == Model_Unknown ) // better don't suspend in qvfb / on unkown devices
		return false;

	bool res = false;

	struct timeval tvs, tvn;
	::gettimeofday ( &tvs, 0 );

	::sync ( ); // flush fs caches
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

	if ( d-> m_model == Model_Unknown )
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
int ODevice::displayBrightnessResolution ( ) const
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
int ODevice::displayContrastResolution ( ) const
{
	return 0;
}

/**
 * This returns the vendor as string
 * @return Vendor as QString
 */
QString ODevice::vendorString ( ) const
{
	return d-> m_vendorstr;
}

/**
 * This returns the vendor as one of the values of OVendor
 * @return OVendor
 */
OVendor ODevice::vendor ( ) const
{
	return d-> m_vendor;
}

/**
 * This returns the model as a string
 * @return A string representing the model
 */
QString ODevice::modelString ( ) const
{
	return d-> m_modelstr;
}

/**
 * This does return the OModel used
 */
OModel ODevice::model ( ) const
{
	return d-> m_model;
}

/**
 * This does return the systen name
 */
QString ODevice::systemString ( ) const
{
	return d-> m_systemstr;
}

/**
 * Return System as OSystem value
 */
OSystem ODevice::system ( ) const
{
	return d-> m_system;
}

/**
 * @return the version string of the base system
 */
QString ODevice::systemVersionString ( ) const
{
	return d-> m_sysverstr;
}

/**
 *  @return the current Transformation
 */
Transformation ODevice::rotation ( ) const
{
    VirtRotation rot;
    ODevice* that =(ODevice* )this;
    that->virtual_hook( VIRTUAL_ROTATION, &rot );
    return rot.trans;
}

/**
 *  @return the current rotation direction
 */
ODirection ODevice::direction ( ) const
{
    VirtDirection dir;
    ODevice* that =(ODevice* )this;
    that->virtual_hook( VIRTUAL_DIRECTION, &dir );
    return dir.direct;
}

/**
 * This plays an alarmSound
 */
void ODevice::alarmSound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}

/**
 * This plays a key sound
 */
void ODevice::keySound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "keysound" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}

/**
 * This plays a touch sound
 */
void ODevice::touchSound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "touchsound" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}

/**
 * This method will return a list of leds
 * available on this device
 * @return a list of LEDs.
 */
QValueList <OLed> ODevice::ledList ( ) const
{
	return QValueList <OLed> ( );
}

/**
 * This does return the state of the LEDs
 */
QValueList <OLedState> ODevice::ledStateList ( OLed /*which*/ ) const
{
	return QValueList <OLedState> ( );
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
bool ODevice::hasLightSensor ( ) const
{
	return false;
}

/**
 * @return a value from the light sensor
 */
int ODevice::readLightSensor ( )
{
	return -1;
}

/**
 * @return the light sensor resolution
 */
int ODevice::lightSensorResolution ( ) const
{
	return 0;
}

/**
 * @return if the device has a hinge sensor
 */
bool ODevice::hasHingeSensor ( ) const
{
    VirtHasHinge hing;
    ODevice* that =(ODevice* )this;
    that->virtual_hook( VIRTUAL_HAS_HINGE, &hing );
    return hing.hasHinge;
}

/**
 * @return a value from the hinge sensor
 */
OHingeStatus ODevice::readHingeSensor ( )
{
    VirtHingeStatus hing;
    virtual_hook( VIRTUAL_HINGE, &hing );
    return hing.hingeStat;
}

/**
 * @return a list with CPU frequencies supported by the hardware
 */
const QStrList &ODevice::allowedCpuFrequencies ( ) const
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
const QValueList <ODeviceButton> &ODevice::buttons ( )
{
	initButtons ( );

	return *d-> m_buttons;
}

/**
 * @return The amount of time that would count as a hold
 */
uint ODevice::buttonHoldTime ( ) const
{
	return d-> m_holdtime;
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
	initButtons ( );

	for ( QValueListConstIterator<ODeviceButton> it = d-> m_buttons-> begin ( ); it != d-> m_buttons-> end ( ); ++it ) {
		if ( (*it). keycode ( ) == code )
			return &(*it);
	}
	return 0;
}

void ODevice::reloadButtonMapping ( )
{
	initButtons ( );

	Config cfg ( "ButtonSettings" );

	for ( uint i = 0; i < d-> m_buttons-> count ( ); i++ ) {
		ODeviceButton &b = ( *d-> m_buttons ) [i];
		QString group = "Button" + QString::number ( i );

		QCString pch, hch;
		QCString pm, hm;
		QByteArray pdata, hdata;

		if ( cfg. hasGroup ( group )) {
			cfg. setGroup ( group );
			pch = cfg. readEntry ( "PressedActionChannel" ). latin1 ( );
			pm  = cfg. readEntry ( "PressedActionMessage" ). latin1 ( );
			// pdata = decodeBase64 ( buttonFile. readEntry ( "PressedActionArgs" ));

			hch = cfg. readEntry ( "HeldActionChannel" ). latin1 ( );
			hm  = cfg. readEntry ( "HeldActionMessage" ). latin1 ( );
			// hdata = decodeBase64 ( buttonFile. readEntry ( "HeldActionArgs" ));
		}

		b. setPressedAction ( OQCopMessage ( pch, pm, pdata ));

		b. setHeldAction ( OQCopMessage ( hch, hm, hdata ));
	}
}

void ODevice::remapPressedAction ( int button, const OQCopMessage &action )
{
	initButtons ( );

	QString mb_chan;

	if ( button >= (int) d-> m_buttons-> count ( ))
		return;

	ODeviceButton &b = ( *d-> m_buttons ) [button];
        b. setPressedAction ( action );

	mb_chan=b. pressedAction ( ). channel ( );

	Config buttonFile ( "ButtonSettings" );
	buttonFile. setGroup ( "Button" + QString::number ( button ));
	buttonFile. writeEntry ( "PressedActionChannel", (const char*) mb_chan);
	buttonFile. writeEntry ( "PressedActionMessage", (const char*) b. pressedAction ( ). message ( ));

//	buttonFile. writeEntry ( "PressedActionArgs", encodeBase64 ( b. pressedAction ( ). data ( )));

	QCopEnvelope ( "QPE/System", "deviceButtonMappingChanged()" );
}

void ODevice::remapHeldAction ( int button, const OQCopMessage &action )
{
	initButtons ( );

	if ( button >= (int) d-> m_buttons-> count ( ))
		return;

	ODeviceButton &b = ( *d-> m_buttons ) [button];
    	b. setHeldAction ( action );

	Config buttonFile ( "ButtonSettings" );
	buttonFile. setGroup ( "Button" + QString::number ( button ));
	buttonFile. writeEntry ( "HeldActionChannel", (const char *) b. heldAction ( ). channel ( ));
	buttonFile. writeEntry ( "HeldActionMessage", (const char *) b. heldAction ( ). message ( ));

//	buttonFile. writeEntry ( "HeldActionArgs", decodeBase64 ( b. heldAction ( ). data ( )));

	QCopEnvelope ( "QPE/System", "deviceButtonMappingChanged()" );
}
void ODevice::virtual_hook(int id, void* data){
    switch( id ) {
    case VIRTUAL_ROTATION:{
        VirtRotation* rot = reinterpret_cast<VirtRotation*>( data );
        rot->trans = d->m_rotation;
        break;
    }
    case VIRTUAL_DIRECTION:{
        VirtDirection *dir = reinterpret_cast<VirtDirection*>( data );
        dir->direct = d->m_direction;
        break;
    }
    case VIRTUAL_HAS_HINGE:{
        VirtHasHinge *hin = reinterpret_cast<VirtHasHinge*>( data );
        hin->hasHinge = false;
        break;
    }
    case VIRTUAL_HINGE:{
        VirtHingeStatus *hin = reinterpret_cast<VirtHingeStatus*>( data );
        hin->hingeStat = CASE_UNKNOWN;
        break;
    }
    }
}

/**************************************************
 *
 * Yopy 3500/3700
 *
 **************************************************/

bool Yopy::isYopy ( )
{
  QFile f( "/proc/cpuinfo" );
  if ( f. open ( IO_ReadOnly ) ) {
    QTextStream ts ( &f );
    QString line;
    while( line = ts. readLine ( ) ) {
      if ( line. left ( 8 ) == "Hardware" ) {
	int loc = line. find ( ":" );
	if ( loc != -1 ) {
	  QString model =
	    line. mid ( loc + 2 ). simplifyWhiteSpace( );
	  return ( model == "Yopy" );
	}
      }
    }
  }
  return false;
}

void Yopy::init ( )
{
  d-> m_vendorstr = "G.Mate";
  d-> m_vendor = Vendor_GMate;
  d-> m_modelstr = "Yopy3700";
  d-> m_model = Model_Yopy_3700;
  d-> m_rotation = Rot0;

  d-> m_systemstr = "Linupy";
  d-> m_system = System_Linupy;

  QFile f ( "/etc/issue" );
  if ( f. open ( IO_ReadOnly )) {
    QTextStream ts ( &f );
    ts.readLine();
    d-> m_sysverstr = ts. readLine ( );
    f. close ( );
  }
}

void Yopy::initButtons ( )
{
  if ( d-> m_buttons )
    return;

  d-> m_buttons = new QValueList <ODeviceButton>;

  for (uint i = 0; i < ( sizeof( yopy_buttons ) / sizeof(yopy_button)); i++) {

    yopy_button *ib = yopy_buttons + i;

    ODeviceButton b;

    b. setKeycode ( ib-> code );
    b. setUserText ( QObject::tr ( "Button", ib-> utext ));
    b. setPixmap ( Resource::loadPixmap ( ib-> pix ));
    b. setFactoryPresetPressedAction
      (OQCopMessage(makeChannel(ib->fpressedservice), ib->fpressedaction));
    b. setFactoryPresetHeldAction
      (OQCopMessage(makeChannel(ib->fheldservice), ib->fheldaction));

    d-> m_buttons-> append ( b );
  }
  reloadButtonMapping ( );

  QCopChannel *sysch = new QCopChannel("QPE/System", this);
  connect(sysch, SIGNAL(received(const QCString&,const QByteArray&)),
	  this, SLOT(systemMessage(const QCString&,const QByteArray&)));
}

bool Yopy::suspend()
{
  /* Opie for Yopy does not implement its own power management at the
     moment.  The public version runs parallel to X, and relies on the
     existing power management features. */
  return false;
}

bool Yopy::setDisplayBrightness(int bright)
{
  /* The code here works, but is disabled as the current version runs
     parallel to X, and relies on the existing backlight demon. */
#if 0
  if ( QFile::exists("/proc/sys/pm/light") ) {
    int fd = ::open("/proc/sys/pm/light", O_WRONLY);
    if (fd >= 0 ) {
      if (bright)
	::write(fd, "1\n", 2);
      else
	::write(fd, "0\n", 2);
      ::close(fd);
      return true;
    }
  }
#endif
  return false;
}

int Yopy::displayBrightnessResolution() const
{
  return 2;
}

/**************************************************
 *
 * iPAQ
 *
 **************************************************/

void iPAQ::init ( )
{
	d-> m_vendorstr = "HP";
	d-> m_vendor = Vendor_HP;

	QFile f ( "/proc/hal/model" );

	if ( f. open ( IO_ReadOnly )) {
		QTextStream ts ( &f );

		d-> m_modelstr = "H" + ts. readLine ( );

		if ( d-> m_modelstr == "H3100" )
			d-> m_model = Model_iPAQ_H31xx;
		else if ( d-> m_modelstr == "H3600" )
			d-> m_model = Model_iPAQ_H36xx;
		else if ( d-> m_modelstr == "H3700" )
			d-> m_model = Model_iPAQ_H37xx;
		else if ( d-> m_modelstr == "H3800" )
			d-> m_model = Model_iPAQ_H38xx;
		else if ( d-> m_modelstr == "H3900" )
			d-> m_model = Model_iPAQ_H39xx;
		else if ( d-> m_modelstr == "H5400" )
			d-> m_model = Model_iPAQ_H5xxx;
		else
			d-> m_model = Model_Unknown;

		f. close ( );
	}

	switch ( d-> m_model ) {
		case Model_iPAQ_H31xx:
		case Model_iPAQ_H38xx:
			d-> m_rotation = Rot90;
			break;
		case Model_iPAQ_H36xx:
		case Model_iPAQ_H37xx:
		case Model_iPAQ_H39xx:

		default:
			d-> m_rotation = Rot270;
			break;
		case Model_iPAQ_H5xxx:
			d-> m_rotation = Rot0;
		}

	f. setName ( "/etc/familiar-version" );
	if ( f. open ( IO_ReadOnly )) {
		d-> m_systemstr = "Familiar";
		d-> m_system = System_Familiar;

		QTextStream ts ( &f );
		d-> m_sysverstr = ts. readLine ( ). mid ( 10 );

		f. close ( );
	} else {
	    f. setName ( "/etc/oz_version" );

                    if ( f. open ( IO_ReadOnly )) {
	    	d-> m_systemstr = "OpenEmbedded/iPaq";
    		d-> m_system = System_Familiar;

	    	QTextStream ts ( &f );
		    ts.setDevice ( &f );
	    	d-> m_sysverstr = ts. readLine ( );
		    f. close ( );
        }
    }





	m_leds [0] = m_leds [1] = Led_Off;

	m_power_timer = 0;

}

void iPAQ::initButtons ( )
{
	if ( d-> m_buttons )
		return;

	if ( isQWS( ) )
		QWSServer::setKeyboardFilter ( this );

	d-> m_buttons = new QValueList <ODeviceButton>;

	for ( uint i = 0; i < ( sizeof( ipaq_buttons ) / sizeof( i_button )); i++ ) {
		i_button *ib = ipaq_buttons + i;
		ODeviceButton b;

		if (( ib-> model & d-> m_model ) == d-> m_model ) {
			b. setKeycode ( ib-> code );
			b. setUserText ( QObject::tr ( "Button", ib-> utext ));
			b. setPixmap ( Resource::loadPixmap ( ib-> pix ));
			b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib-> fpressedservice ), ib-> fpressedaction ));
			b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib-> fheldservice ), ib-> fheldaction ));

			d-> m_buttons-> append ( b );
		}
	}
	reloadButtonMapping ( );

	QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
	connect ( sysch, SIGNAL( received(const QCString&,const QByteArray&)), this, SLOT( systemMessage(const QCString&,const QByteArray&)));
}


//#include <linux/h3600_ts.h>  // including kernel headers is evil ...

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


QValueList <OLed> iPAQ::ledList ( ) const
{
	QValueList <OLed> vl;
	vl << Led_Power;

	if ( d-> m_model == Model_iPAQ_H38xx )
		vl << Led_BlueTooth;
	return vl;
}

QValueList <OLedState> iPAQ::ledStateList ( OLed l ) const
{
	QValueList <OLedState> vl;

	if ( l == Led_Power )
		vl << Led_Off << Led_On << Led_BlinkSlow << Led_BlinkFast;
	else if ( l == Led_BlueTooth && d-> m_model == Model_iPAQ_H38xx )
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
			if (( d-> m_model == Model_iPAQ_H38xx ) ||
			    ( d-> m_model == Model_iPAQ_H39xx ) ||
			    ( d-> m_model == Model_iPAQ_H5xxx)) {
				newkeycode = HardKey_Mail;
			}
			break;
		}

		// Rotate cursor keys 180° or 270°
		case Key_Left :
		case Key_Right:
		case Key_Up   :
		case Key_Down : {

			if (( d-> m_model == Model_iPAQ_H31xx ) ||
			    ( d-> m_model == Model_iPAQ_H38xx )) {
				newkeycode = Key_Left + ( keycode - Key_Left + 2 ) % 4;
			}
                        // Rotate the cursor keys by 270°
                        // keycode - Key_Left = position of the button starting from left clockwise
                        // add the rotation to it and modolo. No we've the original offset
                        // add the offset to the Key_Left key
                        if ( d-> m_model == Model_iPAQ_H5xxx )
                                newkeycode = Key_Left + ( keycode - Key_Left + 3 ) % 4;
			break;
		}

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


void iPAQ::alarmSound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );
	int fd;
	int vol;
	bool vol_reset = false;

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

	snd. play ( );
	while ( !snd. isFinished ( ))
		qApp-> processEvents ( );

	if ( fd >= 0 ) {
		if ( vol_reset )
			::ioctl ( fd, MIXER_WRITE( 0 ), &vol );
		::close ( fd );
	}
#endif
}


bool iPAQ::setSoftSuspend ( bool soft )
{
	bool res = false;
	int fd;

	if (( fd = ::open ( "/proc/sys/ts/suspend_button_mode", O_WRONLY )) >= 0 ) {
		if ( ::write ( fd, soft ? "1" : "0", 1 ) == 1 )
			res = true;
		else
			::perror ( "write to /proc/sys/ts/suspend_button_mode" );

		::close ( fd );
	}
	else
		::perror ( "/proc/sys/ts/suspend_button_mode" );

	return res;
}


bool iPAQ::setDisplayBrightness ( int bright )
{
	bool res = false;
	int fd;

	if ( bright > 255 )
		bright = 255;
	if ( bright < 0 )
		bright = 0;

	if (( fd = ::open ( "/dev/touchscreen/0", O_WRONLY )) >= 0 ) {
		FLITE_IN bl;
		bl. mode = 1;
		bl. pwr = bright ? 1 : 0;
		bl. brightness = ( bright * ( displayBrightnessResolution ( ) - 1 ) + 127 ) / 255;
		res = ( ::ioctl ( fd, FLITE_ON, &bl ) == 0 );
		::close ( fd );
	}
	return res;
}

int iPAQ::displayBrightnessResolution ( ) const
{
	switch ( model ( )) {
		case Model_iPAQ_H31xx:
		case Model_iPAQ_H36xx:
		case Model_iPAQ_H37xx:
			return 128;  	// really 256, but >128 could damage the LCD

		case Model_iPAQ_H38xx:
		case Model_iPAQ_H39xx:
			return 64;
		case Model_iPAQ_H5xxx:
			return 255;

		default:
			return 2;
	}
}


bool iPAQ::hasLightSensor ( ) const
{
	return true;
}

int iPAQ::readLightSensor ( )
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

int iPAQ::lightSensorResolution ( ) const
{
	return 256;
}

/**************************************************
 *
 * Zaurus
 *
 **************************************************/

// Check whether this device is the sharp zaurus..
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

bool Zaurus::isZaurus()
{

	// If the special devices by embedix exist, it is quite simple: it is a Zaurus !
	if ( QFile::exists ( "/dev/sharp_buz" ) || QFile::exists ( "/dev/sharp_led" ) ){
		return true;
	}

	// On non-embedix kernels, we have to look closer.
	bool is_zaurus = false;
	QFile f ( "/proc/cpuinfo" );
	if ( f. open ( IO_ReadOnly ) ) {
		QString model;
		QFile f ( "/proc/cpuinfo" );

  		QTextStream ts ( &f );
  		QString line;
  		while( line = ts. readLine ( ) ) {
  			if ( line. left ( 8 ) == "Hardware" )
  				break;
  		}
  		int loc = line. find ( ":" );
  		if ( loc != -1 )
  			model = line. mid ( loc + 2 ). simplifyWhiteSpace( );

		if ( model == "Sharp-Collie"
		     || model == "Collie"
		     || model == "SHARP Corgi"
		     || model == "SHARP Shepherd"
		     || model == "SHARP Poodle"
                     || model == "SHARP Husky"
		   )
			is_zaurus = true;

  	}
	return is_zaurus;
}


void Zaurus::init ( )
{
	d-> m_vendorstr = "Sharp";
	d-> m_vendor = Vendor_Sharp;
	m_embedix = true;  // Not openzaurus means: It has an embedix kernel !

	// QFile f ( "/proc/filesystems" );
	QString model;

	// It isn't a good idea to check the system configuration to
	// detect the distribution !
	// Otherwise it may happen that any other distribution is detected as openzaurus, just
	// because it uses a jffs2 filesystem..
	// (eilers)
	// if ( f. open ( IO_ReadOnly ) && ( QTextStream ( &f ). read ( ). find ( "\tjffs2\n" ) >= 0 )) {
	QFile f ("/etc/oz_version");
	if ( f.exists() ){
		d-> m_vendorstr = "OpenZaurus Team";
		d-> m_systemstr = "OpenZaurus";
		d-> m_system = System_OpenZaurus;

		if ( f. open ( IO_ReadOnly )) {
			QTextStream ts ( &f );
			d-> m_sysverstr = ts. readLine ( );//. mid ( 10 );
			f. close ( );
		}

		// Openzaurus sometimes uses the embedix kernel!
		// => Check whether this is an embedix kernel
		FILE *uname = popen("uname -r", "r");
		QString line;
		if ( f.open(IO_ReadOnly, uname) ) {
			QTextStream ts ( &f );
			line = ts. readLine ( );
			int loc = line. find ( "embedix" );
			if ( loc != -1 )
				m_embedix = true;
			else
				m_embedix = false;
			f. close ( );
		}
		pclose(uname);
	}
	else {
		d-> m_systemstr = "Zaurus";
		d-> m_system = System_Zaurus;
	}

	f. setName ( "/proc/cpuinfo" );
	if ( f. open ( IO_ReadOnly ) ) {
		QTextStream ts ( &f );
		QString line;
		while( line = ts. readLine ( ) ) {
			if ( line. left ( 8 ) == "Hardware" )
				break;
		}
		int loc = line. find ( ":" );
		if ( loc != -1 )
			model = line. mid ( loc + 2 ). simplifyWhiteSpace( );
	}

	if ( model == "SHARP Corgi" ) {
		d-> m_model = Model_Zaurus_SLC7x0;
		d-> m_modelstr = "Zaurus SL-C700";
	} else if ( model == "SHARP Shepherd" ) {
		d-> m_model = Model_Zaurus_SLC7x0;
		d-> m_modelstr = "Zaurus SL-C750";
	} else if ( model == "SHARP Husky" ) {
		d-> m_model = Model_Zaurus_SLC7x0;
		d-> m_modelstr = "Zaurus SL-C760";
	} else if ( model == "SHARP Poodle" ) {
		d-> m_model = Model_Zaurus_SLB600;
		d-> m_modelstr = "Zaurus SL-B500 or SL-5600";
	} else if ( model == "Sharp-Collie" || model == "Collie" ) {
		d-> m_model = Model_Zaurus_SL5500;
		d-> m_modelstr = "Zaurus SL-5500 or SL-5000d";
	} else {
		d-> m_model = Model_Zaurus_SL5500;
		d-> m_modelstr = "Zaurus (Model unknown)";
	}

	bool flipstate = false;
	switch ( d-> m_model ) {
		case Model_Zaurus_SLA300:
			d-> m_rotation = Rot0;
			break;
		case Model_Zaurus_SLC7x0:
			d-> m_rotation = rotation();
			d-> m_direction = direction();
			break;
		case Model_Zaurus_SLB600:
		case Model_Zaurus_SL5500:
		case Model_Zaurus_SL5000:
		default:
			d-> m_rotation = Rot270;
			break;
	}
	m_leds [0] = Led_Off;
}

void Zaurus::initButtons ( )
{
	if ( d-> m_buttons )
		return;

	d-> m_buttons = new QValueList <ODeviceButton>;

	struct z_button * pz_buttons;
	int buttoncount;
	switch ( d-> m_model ) {
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

		b. setKeycode ( zb-> code );
		b. setUserText ( QObject::tr ( "Button", zb-> utext ));
		b. setPixmap ( Resource::loadPixmap ( zb-> pix ));
		b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( zb-> fpressedservice ),
								  zb-> fpressedaction ));
		b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( zb-> fheldservice ),
							       zb-> fheldaction ));

		d-> m_buttons-> append ( b );
	}

	reloadButtonMapping ( );

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

		snd. play ( );
		while ( !snd. isFinished ( ))
			qApp-> processEvents ( );

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


void Zaurus::alarmSound ( )
{
	buzzer ( SHARP_BUZ_SCHEDULE_ALARM );
}

void Zaurus::touchSound ( )
{
	buzzer ( SHARP_BUZ_TOUCHSOUND );
}

void Zaurus::keySound ( )
{
	buzzer ( SHARP_BUZ_KEYSOUND );
}


QValueList <OLed> Zaurus::ledList ( ) const
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

bool Zaurus::suspend ( )
{
	qDebug("ODevice::suspend");
	if ( !isQWS( ) ) // only qwsserver is allowed to suspend
		return false;

	if ( d-> m_model == Model_Unknown ) // better don't suspend in qvfb / on unkown devices
		return false;

	bool res = false;

	struct timeval tvs, tvn;
	::gettimeofday ( &tvs, 0 );

	::sync ( ); // flush fs caches
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


Transformation Zaurus::rotation ( ) const
{
	Transformation rot;
	int handle = 0;
	int retval = 0;

	switch ( d-> m_model ) {
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
			rot = d-> m_rotation;
			break;
	}

	return rot;
}
ODirection Zaurus::direction ( ) const
{
	ODirection dir;
	int handle = 0;
	int retval = 0;
	switch ( d-> m_model ) {
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
			dir = d-> m_direction;
			break;
	}
	return dir;

}

int Zaurus::displayBrightnessResolution ( ) const
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


void Zaurus::virtual_hook( int id, void *data ) {
    switch( id ) {
    case VIRTUAL_ROTATION:{
        VirtRotation* rot = reinterpret_cast<VirtRotation*>( data );
        rot->trans = rotation();
        break;
    }
    case VIRTUAL_DIRECTION:{
        VirtDirection *dir = reinterpret_cast<VirtDirection*>( data );
        dir->direct = direction();
        break;
    }
    case VIRTUAL_HAS_HINGE:{
        VirtHasHinge *hin = reinterpret_cast<VirtHasHinge*>( data );
        hin->hasHinge = hasHingeSensor();
        break;
    }
    case VIRTUAL_HINGE:{
        VirtHingeStatus *hin = reinterpret_cast<VirtHingeStatus*>( data );
        hin->hingeStat = readHingeSensor();
        break;
    }
    default:
        ODevice::virtual_hook( id, data );
        break;
    }
}

/**************************************************
 *
 * SIMpad
 *
 **************************************************/

void SIMpad::init ( )
{
	d-> m_vendorstr = "SIEMENS";
	d-> m_vendor = Vendor_SIEMENS;

	QFile f ( "/proc/hal/model" );

    //TODO Implement model checking
    //FIXME For now we assume an SL4

    d-> m_modelstr = "SL4";
    d-> m_model = Model_SIMpad_SL4;

	switch ( d-> m_model ) {
		default:
			d-> m_rotation = Rot0;
			d-> m_direction = CCW;
			d-> m_holdtime = 1000; // 1000ms

			break;
	}

	f. setName ( "/etc/familiar-version" );
	if ( f. open ( IO_ReadOnly )) {
		d-> m_systemstr = "Familiar";
		d-> m_system = System_Familiar;

		QTextStream ts ( &f );
		d-> m_sysverstr = ts. readLine ( ). mid ( 10 );

		f. close ( );
	} else {
	    f. setName ( "/etc/oz_version" );

                    if ( f. open ( IO_ReadOnly )) {
	    	d-> m_systemstr = "OpenEmbedded/SIMpad";
    		d-> m_system = System_OpenZaurus;

	    	QTextStream ts ( &f );
		    ts.setDevice ( &f );
	    	d-> m_sysverstr = ts. readLine ( );
		    f. close ( );
        }
    }

	m_leds [0] = m_leds [1] = Led_Off;

	m_power_timer = 0;

}

void SIMpad::initButtons ( )
{
	if ( d-> m_buttons )
		return;

	if ( isQWS( ) )
		QWSServer::setKeyboardFilter ( this );

	d-> m_buttons = new QValueList <ODeviceButton>;

	for ( uint i = 0; i < ( sizeof( simpad_buttons ) / sizeof( s_button )); i++ ) {
		s_button *sb = simpad_buttons + i;
		ODeviceButton b;

		if (( sb-> model & d-> m_model ) == d-> m_model ) {
			b. setKeycode ( sb-> code );
			b. setUserText ( QObject::tr ( "Button", sb-> utext ));
			b. setPixmap ( Resource::loadPixmap ( sb-> pix ));
			b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( sb-> fpressedservice ), sb-> fpressedaction ));
			b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( sb-> fheldservice ), sb-> fheldaction ));

			d-> m_buttons-> append ( b );
		}
	}
	reloadButtonMapping ( );

	QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
	connect ( sysch, SIGNAL( received(const QCString&,const QByteArray&)), this, SLOT( systemMessage(const QCString&,const QByteArray&)));
}

// SIMpad boardcontrol register CS3
#define SIMPAD_BOARDCONTROL "/proc/cs3"
#define SIMPAD_VCC_5V_EN                   0x0001 // For 5V PCMCIA
#define SIMPAD_VCC_3V_EN                   0x0002 // FOR 3.3V PCMCIA
#define SIMPAD_EN1                         0x0004 // This is only for EPROM's
#define SIMPAD_EN0                         0x0008 // Both should be enable for 3.3V or 5V
#define SIMPAD_DISPLAY_ON                  0x0010
#define SIMPAD_PCMCIA_BUFF_DIS             0x0020
#define SIMPAD_MQ_RESET                    0x0040
#define SIMPAD_PCMCIA_RESET                0x0080
#define SIMPAD_DECT_POWER_ON               0x0100
#define SIMPAD_IRDA_SD                     0x0200 // Shutdown for powersave
#define SIMPAD_RS232_ON                    0x0400
#define SIMPAD_SD_MEDIAQ                   0x0800 // Shutdown for powersave
#define SIMPAD_LED2_ON                     0x1000
#define SIMPAD_IRDA_MODE                   0x2000 // Fast/Slow IrDA mode
#define SIMPAD_ENABLE_5V                   0x4000 // Enable 5V circuit
#define SIMPAD_RESET_SIMCARD               0x8000

//SIMpad touchscreen backlight strength control
#define SIMPAD_BACKLIGHT_CONTROL "/proc/driver/mq200/registers/PWM_CONTROL"
#define SIMPAD_BACKLIGHT_MASK    0x00a10044

QValueList <OLed> SIMpad::ledList ( ) const
{
	QValueList <OLed> vl;
	vl << Led_Power; //FIXME which LED is LED2 ? The green one or the amber one?
	//vl << Led_Mail; //TODO find out if LED1 is accessible anyway
	return vl;
}

QValueList <OLedState> SIMpad::ledStateList ( OLed l ) const
{
	QValueList <OLedState> vl;

	if ( l == Led_Power )  //FIXME which LED is LED2 ? The green one or the amber one?
		vl << Led_Off << Led_On;
	//else if ( l == Led_Mail ) //TODO find out if LED1 is accessible anyway
		//vl << Led_Off;
	return vl;
}

OLedState SIMpad::ledState ( OLed l ) const
{
	switch ( l ) {
		case Led_Power:
			return m_leds [0];
		//case Led_Mail:
		//	return m_leds [1];
		default:
			return Led_Off;
	}
}

bool SIMpad::setLedState ( OLed l, OLedState st )
{
	static int fd = ::open ( SIMPAD_BOARDCONTROL, O_RDWR | O_NONBLOCK );

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

			{
            /*TODO Implement this like that:
               read from cs3
               && with SIMPAD_LED2_ON
               write to cs3 */
				m_leds [0] = st;
				return true;
			}
		}
	}
	return false;
}


bool SIMpad::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
	//TODO
	return false;
}

void SIMpad::timerEvent ( QTimerEvent * )
{
	killTimer ( m_power_timer );
	m_power_timer = 0;
	QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, true, false );
	QWSServer::sendKeyEvent ( -1, HardKey_Backlight, 0, false, false );
}


void SIMpad::alarmSound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );
	int fd;
	int vol;
	bool vol_reset = false;

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

	snd. play ( );
	while ( !snd. isFinished ( ))
		qApp-> processEvents ( );

	if ( fd >= 0 ) {
		if ( vol_reset )
			::ioctl ( fd, MIXER_WRITE( 0 ), &vol );
		::close ( fd );
	}
#endif
}


bool SIMpad::suspend ( ) // Must override because SIMpad does NOT have apm
{
	qDebug( "ODevice for SIMpad: suspend()" );
	if ( !isQWS( ) ) // only qwsserver is allowed to suspend
		return false;

	bool res = false;

	struct timeval tvs, tvn;
	::gettimeofday ( &tvs, 0 );

	::sync ( ); // flush fs caches
	res = ( ::system ( "cat /dev/fb/0 >/tmp/.buffer; echo > /proc/sys/pm/suspend; cat /tmp/.buffer >/dev/fb/0" ) == 0 ); //TODO make better :)

	return res;
}


bool SIMpad::setSoftSuspend ( bool soft )
{
	qDebug( "ODevice for SIMpad: UNHANDLED setSoftSuspend(%s)", soft? "on" : "off" );
	return false;
}


bool SIMpad::setDisplayStatus ( bool on )
{
	qDebug( "ODevice for SIMpad: setDisplayStatus(%s)", on? "on" : "off" );

	bool res = false;
	int fd;

	QString cmdline = QString().sprintf( "echo %s > /proc/cs3", on ? "0xd41a" : "0xd40a" ); //TODO make better :)

	res = ( ::system( (const char*) cmdline ) == 0 );

	return res;
}


bool SIMpad::setDisplayBrightness ( int bright )
{
	qDebug( "ODevice for SIMpad: setDisplayBrightness( %d )", bright );
	bool res = false;
	int fd;

	if ( bright > 255 )
		bright = 255;
	if ( bright < 1 )
		bright = 0;

	if (( fd = ::open ( SIMPAD_BACKLIGHT_CONTROL, O_WRONLY )) >= 0 ) {
		int value = 255 - bright;
		const int mask = SIMPAD_BACKLIGHT_MASK;
		value = value << 8;
		value += mask;
		char writeCommand[100];
		const int count = sprintf( writeCommand, "0x%x\n", value );
		res = ( ::write ( fd, writeCommand, count ) != -1 );
		::close ( fd );
	}
	return res;
}


int SIMpad::displayBrightnessResolution ( ) const
{
	return 255; // All SIMpad models share the same display
}

/**************************************************
 *
 * Ramses
 *
 **************************************************/

void Ramses::init()
{
	d->m_vendorstr = "M und N";
	d->m_vendor = Vendor_MundN;

	QFile f("/proc/sys/board/ramses");

	d->m_modelstr = "Ramses";
	d->m_model = Model_Ramses_MNCI;

	d->m_rotation = Rot0;
	d->m_holdtime = 1000;

	f.setName("/etc/oz_version");

	if (f.open(IO_ReadOnly)) {
	   	d->m_systemstr = "OpenEmbedded/Ramses";
	    	d->m_system = System_OpenZaurus;

		QTextStream ts(&f);
		ts.setDevice(&f);
		d->m_sysverstr = ts.readLine();
		f.close();
	}

	m_power_timer = 0;

#ifdef QT_QWS_ALLOW_OVERCLOCK
#warning *** Overclocking enabled - this may fry your hardware - you have been warned ***
#define OC(x...)        x
#else
#define OC(x...)
#endif


	// This table is true for a Intel XScale PXA 255

	d->m_cpu_frequencies->append("99000");      // mem= 99, run= 99, turbo= 99, PXbus= 50
OC(	d->m_cpu_frequencies->append("118000"); )   // mem=118, run=118, turbo=118, PXbus= 59 OC'd mem
	d->m_cpu_frequencies->append("199100");     // mem= 99, run=199, turbo=199, PXbus= 99
OC(	d->m_cpu_frequencies->append("236000"); )   // mem=118, run=236, turbo=236, PXbus=118 OC'd mem
	d->m_cpu_frequencies->append("298600");     // mem= 99, run=199, turbo=298, PXbus= 99
OC(	d->m_cpu_frequencies->append("354000"); )   // mem=118, run=236, turbo=354, PXbus=118 OC'd mem
	d->m_cpu_frequencies->append("398099");     // mem= 99, run=199, turbo=398, PXbus= 99
	d->m_cpu_frequencies->append("398100");     // mem= 99, run=398, turbo=398, PXbus=196
OC(	d->m_cpu_frequencies->append("471000"); )   // mem=118, run=471, turbo=471, PXbus=236 OC'd mem/core/bus

}

bool Ramses::filter(int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
        Q_UNUSED( keycode );
        Q_UNUSED( modifiers );
        Q_UNUSED( isPress );
        Q_UNUSED( autoRepeat );
	return false;
}

void Ramses::timerEvent(QTimerEvent *)
{
	killTimer(m_power_timer);
	m_power_timer = 0;
	QWSServer::sendKeyEvent(-1, HardKey_Backlight, 0, true, false);
	QWSServer::sendKeyEvent(-1, HardKey_Backlight, 0, false, false);
}


bool Ramses::setSoftSuspend(bool soft)
{
	qDebug("Ramses::setSoftSuspend(%d)", soft);
#if 0
	bool res = false;
	int fd;

	if (((fd = ::open("/dev/apm_bios", O_RDWR)) >= 0) ||
        ((fd = ::open("/dev/misc/apm_bios",O_RDWR)) >= 0)) {

		int sources = ::ioctl(fd, APM_IOCGEVTSRC, 0); // get current event sources

		if (sources >= 0) {
			if (soft)
				sources &= ~APM_EVT_POWER_BUTTON;
			else
				sources |= APM_EVT_POWER_BUTTON;

			if (::ioctl(fd, APM_IOCSEVTSRC, sources) >= 0) // set new event sources
				res = true;
			else
				perror("APM_IOCGEVTSRC");
		}
		else
			perror("APM_IOCGEVTSRC");

		::close(fd);
	}
	else
		perror("/dev/apm_bios or /dev/misc/apm_bios");

    return res;
#else
    return true;
#endif
}

bool Ramses::suspend ( )
{
	qDebug("Ramses::suspend");
	return false;
}

/**
 * This sets the display on or off
 */
bool Ramses::setDisplayStatus(bool on)
{
	qDebug("Ramses::setDisplayStatus(%d)", on);
#if 0
	bool res = false;
	int fd;

	if ((fd = ::open ("/dev/fb/0", O_RDWR)) >= 0) {
		res = (::ioctl(fd, FBIOBLANK, on ? VESA_NO_BLANKING : VESA_POWERDOWN) == 0);
		::close(fd);
	}
	return res;
#else
	return true;
#endif
}


/*
 * We get something between 0..255 into us
*/
bool Ramses::setDisplayBrightness(int bright)
{
	qDebug("Ramses::setDisplayBrightness(%d)", bright);
	bool res = false;
	int fd;

	// pwm1 brighness: 20 steps 500..0 (dunkel->hell)

	if (bright > 255 )
		bright = 255;
	if (bright < 0)
		bright = 0;

	// Turn backlight completely off
	if ((fd = ::open("/proc/sys/board/lcd_backlight", O_WRONLY)) >= 0) {
		char writeCommand[10];
		const int count = sprintf(writeCommand, "%d\n", bright ? 1 : 0);
		res = (::write(fd, writeCommand, count) != -1);
		::close(fd);
	}

	// scale backlight brightness to hardware
	bright = 500-(bright * 500 / 255);
	if ((fd = ::open("/proc/sys/board/pwm1", O_WRONLY)) >= 0) {
		qDebug(" %d -> pwm1", bright);
		char writeCommand[100];
		const int count = sprintf(writeCommand, "%d\n", bright);
		res = (::write(fd, writeCommand, count) != -1);
		::close(fd);
	}
	return res;
}


int Ramses::displayBrightnessResolution() const
{
	return 32;
}

bool Ramses::setDisplayContrast(int contr)
{
	qDebug("Ramses::setDisplayContrast(%d)", contr);
	bool res = false;
	int fd;

	// pwm0 contrast: 20 steps 79..90 (dunkel->hell)

	if (contr > 255 )
		contr = 255;
	if (contr < 0)
		contr = 0;
	contr = 90 - (contr * 20 / 255);

	if ((fd = ::open("/proc/sys/board/pwm0", O_WRONLY)) >= 0) {
		qDebug(" %d -> pwm0", contr);
		char writeCommand[100];
		const int count = sprintf(writeCommand, "%d\n", contr);
		res = (::write(fd, writeCommand, count) != -1);
		res = true;
		::close(fd);
	}
	return res;
}


int Ramses::displayContrastResolution() const
{
	return 20;
}


/**************************************************
 *                                                *
 * Jornada                                        *
 *                                                *
 **************************************************/


bool Jornada::isJornada ( )
{
  QFile f( "/proc/cpuinfo" );
  if ( f. open ( IO_ReadOnly ) ) {
    QTextStream ts ( &f );
    QString line;
    while( line = ts. readLine ( ) ) {
      if ( line. left ( 8 ) == "Hardware" ) {
	int loc = line. find ( ":" );
	if ( loc != -1 ) {
	  QString model =
	    line. mid ( loc + 2 ). simplifyWhiteSpace( );
	  return ( model == "HP Jornada 56x" );
	}
      }
    }
  }
  return false;
}

void Jornada::init ( )
{
	d-> m_vendorstr = "HP";
	d-> m_vendor = Vendor_HP;
	d-> m_modelstr = "Jornada 56x";
	d-> m_model = Model_Jornada_56x;
	d-> m_systemstr = "Familiar";
	d-> m_system = System_Familiar;
	d-> m_rotation = Rot0;

	QFile f ( "/etc/familiar-version" );
	f. setName ( "/etc/familiar-version" );
	if ( f. open ( IO_ReadOnly )) {

		QTextStream ts ( &f );
		d-> m_sysverstr = ts. readLine ( ). mid ( 10 );

		f. close ( );
	}
}

#if 0
void Jornada::initButtons ( )
{
	if ( d-> m_buttons )
		return;

	// Simulation uses iPAQ 3660 device buttons

	qDebug ( "init Buttons" );
	d-> m_buttons = new QValueList <ODeviceButton>;

	for ( uint i = 0; i < ( sizeof( ipaq_buttons ) / sizeof( i_button )); i++ ) {
		i_button *ib = ipaq_buttons + i;
		ODeviceButton b;

		if (( ib-> model & Model_iPAQ_H36xx ) == Model_iPAQ_H36xx ) {
			b. setKeycode ( ib-> code );
			b. setUserText ( QObject::tr ( "Button", ib-> utext ));
			b. setPixmap ( Resource::loadPixmap ( ib-> pix ));
			b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib-> fpressedservice ), ib-> fpressedaction ));
			b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib-> fheldservice ), ib-> fheldaction ));
			d-> m_buttons-> append ( b );
		}
	}
	reloadButtonMapping ( );

	QCopChannel *sysch = new QCopChannel ( "QPE/System", this );
	connect ( sysch, SIGNAL( received(const QCString&,const QByteArray&)), this, SLOT( systemMessage(const QCString&,const QByteArray&)));
}
#endif
int Jornada::displayBrightnessResolution ( ) const
{
}

bool Jornada::setDisplayBrightness ( int bright )
{
	bool res = false;
	int fd;

	if ( bright > 255 )
		bright = 255;
	if ( bright < 0 )
		bright = 0;

	if (( fd = ::open ( "/dev/touchscreen/0", O_WRONLY )) >= 0 ) {
		FLITE_IN bl;
		bl. mode = 1;
		bl. pwr = bright ? 1 : 0;
		bl. brightness = ( bright * ( displayBrightnessResolution ( ) - 1 ) + 127 ) / 255;
		res = ( ::ioctl ( fd, FLITE_ON, &bl ) == 0 );
		::close ( fd );
	}
	return res;
}

bool Jornada::setSoftSuspend ( bool soft )
{
	bool res = false;
	int fd;

	if (( fd = ::open ( "/proc/sys/ts/suspend_button_mode", O_WRONLY )) >= 0 ) {
		if ( ::write ( fd, soft ? "1" : "0", 1 ) == 1 )
			res = true;
		else
			::perror ( "write to /proc/sys/ts/suspend_button_mode" );

		::close ( fd );
	}
	else
		::perror ( "/proc/sys/ts/suspend_button_mode" );

	return res;
}
