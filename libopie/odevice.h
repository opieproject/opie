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


class ODeviceData;

enum OModel {
	OMODEL_Unknown,

	OMODEL_iPAQ_H31xx,
	OMODEL_iPAQ_H36xx,
	OMODEL_iPAQ_H37xx,
	OMODEL_iPAQ_H38xx,

	OMODEL_Zaurus_SL5000
};

enum OVendor {	
	OVENDOR_Unknown,

	OVENDOR_HP,
	OVENDOR_Sharp,
};	

enum OSystem {
	OSYSTEM_Unknown,

	OSYSTEM_Familiar,
	OSYSTEM_Zaurus,
	OSYSTEM_OpenZaurus
};

enum OLedState {
	OLED_Off,
	OLED_On,
	OLED_BlinkSlow,
	OLED_BlinkFast
};


class ODevice
{
public:

public:
	static ODevice *inst ( );
	

// information

	QString modelString ( ); 
	OModel model ( );
	
	QString vendorString ( );
	OVendor vendor ( );

	QString systemString ( );
	OSystem system ( );


// input / output

	virtual void alarmSound ( );
	virtual void keySound ( );
	virtual void touchSound ( );

	virtual uint hasLeds ( ) const;
	virtual OLedState led ( uint which ) const;
	virtual bool setLed ( uint which, OLedState st );

	virtual ~ODevice ( );

protected:
	ODevice ( );
	virtual void init ( );

	ODeviceData *d;

private:
	ODevice ( const ODevice & );

};

#endif


