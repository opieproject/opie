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
#include <linux/soundcard.h>
#include <math.h>

#include <qapplication.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qpe/sound.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include "odevice.h"

#include <qwindowsystem_qws.h>


// _IO and friends are only defined in kernel headers ...

#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

using namespace Opie;

class ODeviceData {
public:
	bool m_qwsserver;

	QString m_vendorstr;
	OVendor m_vendor;
	
	QString m_modelstr;
	OModel m_model;

	QString m_systemstr;
	OSystem m_system;
	
	QString m_sysverstr;
};


class iPAQ : public QObject, public ODevice, public QWSServer::KeyboardFilter {
protected: 
	virtual void init ( );
	
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
	
	//virtual QValueList <int> keyList ( ) const;
	
protected:
	virtual bool filter ( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );
	virtual void timerEvent ( QTimerEvent *te );
	
	int m_power_timer;
	
	OLedState m_leds [2];
};

class Zaurus : public ODevice {
protected:
	virtual void init ( );

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
	
	//virtual QValueList <int> keyList ( ) const;
	
protected:
	virtual void buzzer ( int snd );
	
	OLedState m_leds [1];
};







ODevice *ODevice::inst ( )
{
	static ODevice *dev = 0;
	
	if ( !dev ) {
		if ( QFile::exists ( "/proc/hal/model" ))
			dev = new iPAQ ( );
		else if ( QFile::exists ( "/dev/sharp_buz" ) || QFile::exists ( "/dev/sharp_led" ))
			dev = new Zaurus ( );
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

	d-> m_qwsserver = qApp ? ( qApp-> type ( ) == QApplication::GuiServer ) : false;

	d-> m_modelstr = "Unknown";
	d-> m_model = Model_Unknown;
	d-> m_vendorstr = "Unkown";
	d-> m_vendor = Vendor_Unknown;
	d-> m_systemstr = "Unkown";
	d-> m_system = System_Unknown;
	d-> m_sysverstr = "0.0";
}

void ODevice::init ( )
{
}

ODevice::~ODevice ( )
{
	delete d;
}

bool ODevice::setSoftSuspend ( bool /*soft*/ )
{
	return false;
}

//#include <linux/apm_bios.h>

#define APM_IOC_SUSPEND          OD_IO( 'A', 2 )


bool ODevice::suspend ( )
{
	if ( !d-> m_qwsserver ) // only qwsserver is allowed to suspend
		return false;

	if ( d-> m_model == Model_Unknown ) // better don't suspend in qvfb / on unkown devices
		return false;

	int fd;
	bool res = false;
	
	if ((( fd = ::open ( "/dev/apm_bios", O_RDWR )) >= 0 ) ||
	    (( fd = ::open ( "/dev/misc/apm_bios",O_RDWR )) >= 0 )) {	
		struct timeval tvs, tvn;

//		::signal ( SIGTSTP, SIG_IGN );	// we don't want to be stopped
		::gettimeofday ( &tvs, 0 );
	
		::sync ( ); // flush fs caches
	
		res = ( ::ioctl ( fd, APM_IOC_SUSPEND, 0 ) == 0 ); // tell the kernel to "start" suspending
		::close ( fd );

		if ( res ) {	
//			::kill ( -::getpid ( ), SIGTSTP ); // stop everthing in our process group

			do { // wait at most 1.5 sec: either suspend didn't work or the device resumed
				::usleep ( 200 * 1000 );
				::gettimeofday ( &tvn, 0 );				
			} while ((( tvn. tv_sec - tvs. tv_sec ) * 1000 + ( tvn. tv_usec - tvs. tv_usec ) / 1000 ) < 1500 );
			
//			::kill ( -::getpid ( ), SIGCONT ); // continue everything in our process group
		}	
		
//		::signal ( SIGTSTP, SIG_DFL );
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


bool ODevice::setDisplayStatus ( bool on )
{
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

bool ODevice::setDisplayBrightness ( int )
{
	return false;
}

int ODevice::displayBrightnessResolution ( ) const
{
	return 16;
}

QString ODevice::vendorString ( ) const
{
	return d-> m_vendorstr;
}

OVendor ODevice::vendor ( ) const
{
	return d-> m_vendor;
}

QString ODevice::modelString ( ) const
{
	return d-> m_modelstr;
}

OModel ODevice::model ( ) const
{
	return d-> m_model;
}

QString ODevice::systemString ( ) const
{
	return d-> m_systemstr;
}

OSystem ODevice::system ( ) const
{
	return d-> m_system;
}

QString ODevice::systemVersionString ( ) const
{
	return d-> m_sysverstr;
}

void ODevice::alarmSound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}

void ODevice::keySound ( )
{
#ifndef QT_NO_SOUND
	static Sound snd ( "keysound" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}

void ODevice::touchSound ( )
{

#ifndef QT_NO_SOUND
	static Sound snd ( "touchsound" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
}


QValueList <OLed> ODevice::ledList ( ) const
{
	return QValueList <OLed> ( );
}

QValueList <OLedState> ODevice::ledStateList ( OLed /*which*/ ) const
{
	return QValueList <OLedState> ( );
}

OLedState ODevice::ledState ( OLed /*which*/ ) const
{
	return Led_Off;
}

bool ODevice::setLedState ( OLed /*which*/, OLedState /*st*/ )
{
	return false;
}

bool ODevice::hasLightSensor ( ) const
{
	return false;
}

int ODevice::readLightSensor ( )
{
	return -1;
}

int ODevice::lightSensorResolution ( ) const
{
	return 0;
}

//QValueList <int> ODevice::keyList ( ) const
//{
//	return QValueList <int> ( );
//}



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
		else
			d-> m_model = Model_Unknown;

		f. close ( );
	}

	f. setName ( "/etc/familiar-version" );
	if ( f. open ( IO_ReadOnly )) { 
		d-> m_systemstr = "Familiar";
		d-> m_system = System_Familiar;
		
		QTextStream ts ( &f );
		d-> m_sysverstr = ts. readLine ( ). mid ( 10 );
		
		f. close ( );
	}

	m_leds [0] = m_leds [1] = Led_Off;
	
	m_power_timer = 0;
	
	if ( d-> m_qwsserver )
		QWSServer::setKeyboardFilter ( this );	
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


//QValueList <int> iPAQ::keyList ( ) const
//{
//	QValueList <int> vl;
//	vl << HardKey_Datebook << HardKey_Contacts << ( model ( ) == Model_iPAQ_H38xx ? HardKey_Mail : HardKey_Menu ) << HardKey_Home << HardKey_Record << HardKey_Suspend << HardKey_Backlight;
//	return vl;
//}

bool iPAQ::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
	int newkeycode = keycode;

	
	// simple susbstitutions
	switch ( d-> m_model ) {
		case Model_iPAQ_H38xx:
			// H38xx has no "Q" key anymore - this is now the Mail key
			if ( keycode == HardKey_Menu )
				newkeycode = HardKey_Mail;
			//nobreak
						
		case Model_iPAQ_H31xx: 
			// Rotate cursor keys 180°
			switch ( keycode ) {
				case Key_Left : newkeycode = Key_Right; break;
				case Key_Right: newkeycode = Key_Left; break;
				case Key_Up   : newkeycode = Key_Down; break;
				case Key_Down : newkeycode = Key_Up; break;
			}
			//nobreak;

		case Model_iPAQ_H36xx: 
		case Model_iPAQ_H37xx: 
			// map Power Button short/long press to F34/F35
			if ( keycode == Key_SysReq ) {
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
			}
			//nobreak;
		
		default: 
			break;
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

			if (( volalarm & 0xff ) > ( vol & 0xff )) {
				if ( ::ioctl ( fd, MIXER_WRITE( 0 ), &volalarm ) >= 0 )
					vol_reset = true;
			}
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



void Zaurus::init ( )
{
	d-> m_modelstr = "Zaurus SL5000";
	d-> m_model = Model_Zaurus_SL5000;
	d-> m_vendorstr = "Sharp";
	d-> m_vendor = Vendor_Sharp;

	QFile f ( "/proc/filesystems" );

	if ( f. open ( IO_ReadOnly ) && ( QTextStream ( &f ). read ( ). find ( "\tjffs2\n" ) >= 0 )) {
		d-> m_systemstr = "OpenZaurus";
		d-> m_system = System_OpenZaurus;

		f. close ( );

		f. setName ( "/etc/oz_version" );
		if ( f. open ( IO_ReadOnly )) { 
			QTextStream ts ( &f );
			d-> m_sysverstr = ts. readLine ( );//. mid ( 10 );
			f. close ( );
		}
	}
	else {
		d-> m_systemstr = "Zaurus";
		d-> m_system = System_Zaurus;
	}


	m_leds [0] = Led_Off;
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
	static int fd = ::open ( "/dev/sharp_buz", O_RDWR|O_NONBLOCK );
  
	if ( fd >= 0 )
		::ioctl ( fd, SHARP_BUZZER_MAKESOUND, sound );
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
	bool res = false;
	int fd;
	
	if ( bright > 255 )
		bright = 255;
	if ( bright < 0 )
		bright = 0;
	
	if (( fd = ::open ( "/dev/fl", O_WRONLY )) >= 0 ) {
		int bl = ( bright * 4 + 127 ) / 255; // only 4 steps on zaurus
		if ( bright && !bl )
			bl = 1;
		res = ( ::ioctl ( fd, FL_IOCTL_STEP_CONTRAST, bl ) == 0 );
		::close ( fd );
	}
	return res;
}


int Zaurus::displayBrightnessResolution ( ) const 
{
	return 5;
}


