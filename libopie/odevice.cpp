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

#include <qapplication.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qpe/sound.h>
#include <qpe/resource.h>
#include <qpe/config.h>



#include "odevice.h"


class ODeviceData {
public:
	QString m_vendorstr;
	OVendor m_vendor;
	
	QString m_modelstr;
	OModel m_model;

	QString m_systemstr;
	OSystem m_system;
	
	QString m_sysverstr;
	
	OLedState m_leds [4]; // just for convenience ...
};

class ODeviceIPAQ : public ODevice {
protected: 
	virtual void init ( );
	
public:
	virtual void alarmSound ( );

	virtual uint hasLeds ( ) const;
	virtual OLedState led ( uint which ) const;
	virtual bool setLed ( uint which, OLedState st );	
};

class ODeviceZaurus : public ODevice {
protected:
	virtual void init ( );

public:	
	virtual void alarmSound ( );
	virtual void keySound ( );
	virtual void touchSound ( );	

	virtual uint hasLeds ( ) const;
	virtual OLedState led ( uint which ) const;
	virtual bool setLed ( uint which, OLedState st );	
	
protected:
	virtual void buzzer ( int snd );
};




ODevice *ODevice::inst ( )
{
	static ODevice *dev = 0;
	
	if ( !dev ) {
		if ( QFile::exists ( "/proc/hal/model" ))
			dev = new ODeviceIPAQ ( );
		else if ( QFile::exists ( "/dev/sharp_buz" ) || QFile::exists ( "/dev/sharp_led" ))
			dev = new ODeviceZaurus ( );
		else
			dev = new ODevice ( );
			
		dev-> init ( );
	}
	return dev;
}

ODevice::ODevice ( )
{
	d = new ODeviceData;

	d-> m_modelstr = "Unknown";
	d-> m_model = OMODEL_Unknown;
	d-> m_vendorstr = "Unkown";
	d-> m_vendor = OVENDOR_Unknown;
	d-> m_systemstr = "Unkown";
	d-> m_system = OSYSTEM_Unknown;
	d-> m_sysverstr = "0.0";
}

void ODevice::init ( )
{
}

ODevice::~ODevice ( )
{
	delete d;
}

//#include <linux/apm_bios.h>

//#define APM_IOC_SUSPEND        _IO('A',2)

#define APM_IOC_SUSPEND          (( 0<<30 ) | ( 'A'<<8 ) | ( 2 ) | ( 0<<16 ))

bool ODevice::suspend ( )
{
	if ( d-> m_model == OMODEL_Unknown ) // better don't suspend in qvfb / on unkown devices
		return false;

	int fd;
	bool res = false;
	
	if ((( fd = ::open ( "/dev/apm_bios", O_RDWR )) >= 0 ) ||
	    (( fd = ::open ( "/dev/misc/apm_bios",O_RDWR )) >= 0 )) {	
		struct timeval tvs, tvn;

		::signal ( SIGTSTP, SIG_IGN );	// we don't want to be stopped
		::gettimeofday ( &tvs, 0 );
	
		res = ( ::ioctl ( fd, APM_IOC_SUSPEND ) == 0 ); // tell the kernel to "start" suspending
		::close ( fd );

		if ( res ) {	
			::kill ( -::getpid ( ), SIGTSTP ); // stop everthing in out process group

			do { // wait at most 1.5 sec: either suspend didn't work or the device resumed
				::usleep ( 200 * 1000 );
				::gettimeofday ( &tvn, 0 );				
			} while ((( tvn. tv_sec - tvs. tv_sec ) * 1000 + ( tvn. tv_usec - tvs. tv_usec ) / 1000 ) < 1500 );
			
			::kill ( -::getpid ( ), SIGCONT ); // continue everything in our process group
		}	
		
		::signal ( SIGTSTP, SIG_DFL );
	}
	
	return res;
}


QString ODevice::vendorString ( )
{
	return d-> m_vendorstr;
}

OVendor ODevice::vendor ( )
{
	return d-> m_vendor;
}

QString ODevice::modelString ( )
{
	return d-> m_modelstr;
}

OModel ODevice::model ( )
{
	return d-> m_model;
}

QString ODevice::systemString ( )
{
	return d-> m_systemstr;
}

OSystem ODevice::system ( )
{
	return d-> m_system;
}

QString ODevice::systemVersionString ( )
{
	return d-> m_sysverstr;
}

void ODevice::alarmSound ( )
{
#ifndef QT_QWS_EBX
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
#endif
}

void ODevice::keySound ( )
{
#ifndef QT_QWS_EBX
#ifndef QT_NO_SOUND
	static Sound snd ( "keysound" );

	if ( snd. isFinished ( ))
		snd. play ( );
#endif
#endif
}

void ODevice::touchSound ( )
{

#ifndef QT_QWS_EBX
#ifndef QT_NO_SOUND
	static Sound snd ( "touchsound" );
//qDebug("touchSound");
	if ( snd. isFinished ( )) {
		snd. play ( );
//		qDebug("sound should play");
		}
#endif
#endif
}

uint ODevice::hasLeds ( ) const
{
	return 0;
}

OLedState ODevice::led ( uint /*which*/ ) const
{
	return OLED_Off;
}

bool ODevice::setLed ( uint /*which*/, OLedState /*st*/ )
{
	return false;
}




//#if defined( QT_QWS_IPAQ ) // IPAQ


void ODeviceIPAQ::init ( )
{
	d-> m_vendorstr = "HP";
	d-> m_vendor = OVENDOR_HP;

	QFile f ( "/proc/hal/model" );

	if ( f. open ( IO_ReadOnly )) {
		QTextStream ts ( &f );

		d-> m_modelstr = "H" + ts. readLine ( );

		if ( d-> m_modelstr == "H3100" )
			d-> m_model = OMODEL_iPAQ_H31xx;
		else if ( d-> m_modelstr == "H3600" )
			d-> m_model = OMODEL_iPAQ_H36xx;
		else if ( d-> m_modelstr == "H3700" )
			d-> m_model = OMODEL_iPAQ_H37xx;
		else if ( d-> m_modelstr == "H3800" )
			d-> m_model = OMODEL_iPAQ_H38xx;
		else
			d-> m_model = OMODEL_Unknown;

		f. close ( );
	}

	f. setName ( "/etc/familiar-version" );
	if ( f. open ( IO_ReadOnly )) { 
		d-> m_systemstr = "Familiar";
		d-> m_system = OSYSTEM_Familiar;
		
		QTextStream ts ( &f );
		d-> m_sysverstr = ts. readLine ( ). mid ( 10 );
		
		f. close ( );
	}

	d-> m_leds [0] = OLED_Off;
}

//#include <linux/h3600_ts.h>  // including kernel headers is evil ...

typedef struct h3600_ts_led {
  unsigned char OffOnBlink;       /* 0=off 1=on 2=Blink */
  unsigned char TotalTime;        /* Units of 5 seconds */
  unsigned char OnTime;           /* units of 100m/s */
  unsigned char OffTime;          /* units of 100m/s */
} LED_IN;


// #define IOC_H3600_TS_MAGIC  'f'
// #define LED_ON                  _IOW(IOC_H3600_TS_MAGIC,  5, struct h3600_ts_led)
#define LED_ON    (( 1<<30 ) | ( 'f'<<8 ) | ( 5 ) | ( sizeof(struct h3600_ts_led)<<16 )) // _IOW only defined in kernel headers :(


void ODeviceIPAQ::alarmSound ( )
{
#if defined( QT_QWS_IPAQ ) // IPAQ
#ifndef QT_NO_SOUND
	static Sound snd ( "alarm" );
	int fd;
	int vol;
	bool vol_reset = false;

	if ((( fd = ::open ( "/dev/sound/mixer", O_RDWR )) >= 0 ) ||
	    (( fd = ::open ( "/dev/mixer", O_RDWR )) >= 0 )) {

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
#endif
}

uint ODeviceIPAQ::hasLeds ( ) const
{
	return 1;
}

OLedState ODeviceIPAQ::led ( uint which ) const
{
	if ( which == 0 )
		return d-> m_leds [0];
	else
		return OLED_Off;
}

bool ODeviceIPAQ::setLed ( uint which, OLedState st )
{
	static int fd = ::open ( "/dev/touchscreen/0", O_RDWR|O_NONBLOCK );

	if ( which == 0 ) {
		if ( fd >= 0 ) {
			struct h3600_ts_led leds;
			::memset ( &leds, 0, sizeof( leds ));
			leds. TotalTime  = 0;
			leds. OnTime     = 0;
			leds. OffTime    = 1;
			leds. OffOnBlink = 2;

			switch ( st ) {
				case OLED_Off      : leds. OffOnBlink = 0; break;
				case OLED_On       : leds. OffOnBlink = 1; break;
				case OLED_BlinkSlow: leds. OnTime = 10; leds. OffTime = 10; break;
				case OLED_BlinkFast: leds. OnTime =  5; leds. OffTime =  5; break;
			}

			if ( ::ioctl ( fd, LED_ON, &leds ) >= 0 ) {
				d-> m_leds [0] = st;
				return true;
			}
		}
	}
	return false;
}


//#endif





//#if defined( QT_QWS_EBX ) // Zaurus

void ODeviceZaurus::init ( )
{
	d-> m_modelstr = "Zaurus SL5000";
	d-> m_model = OMODEL_Zaurus_SL5000;
	d-> m_vendorstr = "Sharp";
	d-> m_vendor = OVENDOR_Sharp;

	QFile f ( "/proc/filesystems" );

	if ( f. open ( IO_ReadOnly ) && ( QTextStream ( &f ). read ( ). find ( "\tjffs2\n" ) >= 0 )) {
		d-> m_systemstr = "OpenZaurus";
		d-> m_system = OSYSTEM_OpenZaurus;

		f. close ( );
	}
	else {
		d-> m_systemstr = "Zaurus";
		d-> m_system = OSYSTEM_Zaurus;
	}

	d-> m_leds [0] = OLED_Off;
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



void ODeviceZaurus::buzzer ( int sound )
{
	static int fd = ::open ( "/dev/sharp_buz", O_RDWR|O_NONBLOCK );
  
	if ( fd >= 0 )
		::ioctl ( fd, SHARP_BUZZER_MAKESOUND, sound );
}


void ODeviceZaurus::alarmSound ( ) 
{
	buzzer ( SHARP_BUZ_SCHEDULE_ALARM );
}

void ODeviceZaurus::touchSound ( ) 
{
	buzzer ( SHARP_BUZ_TOUCHSOUND );
}

void ODeviceZaurus::keySound ( ) 
{
	buzzer ( SHARP_BUZ_KEYSOUND );
}


uint ODeviceZaurus::hasLeds ( ) const
{
	return 1;
}

OLedState ODeviceZaurus::led ( uint which ) const
{
	if ( which == 0 )
		return d-> m_leds [0];
	else	
		return OLED_Off;
}

bool ODeviceZaurus::setLed ( uint which, OLedState st )
{
	static int fd = ::open ( "/dev/sharp_led", O_RDWR|O_NONBLOCK ); 

	if ( which == 0 ) {
		if ( fd >= 0 ) {
			struct sharp_led_status leds;
			::memset ( &leds, 0, sizeof( leds ));
			leds. which = SHARP_LED_MAIL_EXISTS;
			
			switch ( st ) {
				case OLED_Off      : leds. status = LED_MAIL_NO_UNREAD_MAIL; break;
				case OLED_On       : leds. status = LED_MAIL_NEWMAIL_EXISTS; break;
				case OLED_BlinkSlow: 
				case OLED_BlinkFast: leds. status = LED_MAIL_UNREAD_MAIL_EX; break;
			}
		
			if ( ::ioctl ( fd, SHARP_LED_SETSTATUS, &leds ) >= 0 ) {
				d-> m_leds [0] = st;
				return true;
			}
		}		
	}
	return false;
}

//#endif
