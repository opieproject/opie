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

#ifndef _LIBOPIE_ODEVICE_H_
#define _LIBOPIE_ODEVICE_H_

#include <qstring.h>
#include <qnamespace.h>

class ODeviceData;

namespace Opie {

enum OModel {
	Model_Unknown,

	Model_iPAQ_H31xx,
	Model_iPAQ_H36xx,
	Model_iPAQ_H37xx,
	Model_iPAQ_H38xx,

	Model_Zaurus_SL5000
};

enum OVendor {	
	Vendor_Unknown,

	Vendor_HP,
	Vendor_Sharp
};	

enum OSystem {
	System_Unknown,

	System_Familiar,
	System_Zaurus,
	System_OpenZaurus
};

enum OLedState {
	Led_Off,
	Led_On,
	Led_BlinkSlow,
	Led_BlinkFast
};

enum OLed {
	Led_Mail,
	Led_Power,
	Led_BlueTooth
};

enum OHardKey {
	HardKey_Datebook  = Qt::Key_F9,
	HardKey_Contacts  = Qt::Key_F10,
	HardKey_Menu      = Qt::Key_F11,
	HardKey_Home      = Qt::Key_F12,
	HardKey_Mail      = Qt::Key_F14,
	HardKey_Record    = Qt::Key_F24,
	HardKey_Suspend   = Qt::Key_F34,
	HardKey_Backlight = Qt::Key_F35,
};


class ODevice
{
private:
	ODevice ( const ODevice & );

protected:
	ODevice ( );
	virtual void init ( );
	
	ODeviceData *d;

public:
	virtual ~ODevice ( );

	static ODevice *inst ( );



// information

	QString modelString ( ) const; 
	OModel model ( ) const;
	
	QString vendorString ( ) const;
	OVendor vendor ( ) const;

	QString systemString ( ) const;
	OSystem system ( ) const;

	QString systemVersionString ( ) const;

// system	

	virtual bool setSoftSuspend ( bool on );
	virtual bool suspend ( );

	virtual bool setDisplayStatus ( bool on );
	virtual bool setDisplayBrightness ( int brightness );
	virtual int displayBrightnessResolution ( ) const;
	
// input / output

	virtual void alarmSound ( );
	virtual void keySound ( );
	virtual void touchSound ( );

	virtual QValueList <OLed> ledList ( ) const;
	virtual QValueList <OLedState> ledStateList ( OLed led ) const;
	virtual OLedState ledState ( OLed led ) const;
	virtual bool setLedState ( OLed led, OLedState st );

	//virtual QValueList <int> keyList ( ) const;
};

}

#endif

