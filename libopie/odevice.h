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

#include <qobject.h>
#include <qstring.h>
#include <qnamespace.h>
#include <qstrlist.h>

#include <opie/odevicebutton.h>

#include <qpe/qpeapplication.h> /* for Transformation enum.. */

class ODeviceData;

namespace Opie {

/**
 * The available devices
 */
enum OModel {
        Model_Unknown, // = 0

	Model_Series_Mask   = 0xff000000,

	Model_iPAQ          = ( 1 << 24 ),

	Model_iPAQ_All      = ( Model_iPAQ | 0xffffff ),
	Model_iPAQ_H31xx    = ( Model_iPAQ | 0x000001 ),
	Model_iPAQ_H36xx    = ( Model_iPAQ | 0x000002 ),
	Model_iPAQ_H37xx    = ( Model_iPAQ | 0x000004 ),
	Model_iPAQ_H38xx    = ( Model_iPAQ | 0x000008 ),
	Model_iPAQ_H39xx    = ( Model_iPAQ | 0x000010 ),
	Model_iPAQ_H5xxx    = ( Model_iPAQ | 0x000011 ),

	Model_Zaurus        = ( 2 << 24 ),

	Model_Zaurus_SL5000 = ( Model_Zaurus | 0x000001 ),
	Model_Zaurus_SL5500 = ( Model_Zaurus | 0x000002 ),
	Model_Zaurus_SLA300 = ( Model_Zaurus | 0x000003 ),
	Model_Zaurus_SLB600 = ( Model_Zaurus | 0x000004 ),
	Model_Zaurus_SLC7x0 = ( Model_Zaurus | 0x000005 ),


	Model_SIMpad        = ( 3 << 24 ),

	Model_SIMpad_All    = ( Model_SIMpad | 0xffffff ),
	Model_SIMpad_CL4    = ( Model_SIMpad | 0x000001 ),
	Model_SIMpad_SL4    = ( Model_SIMpad | 0x000002 ),
	Model_SIMpad_SLC    = ( Model_SIMpad | 0x000004 ),
	Model_SIMpad_TSinus = ( Model_SIMpad | 0x000008 ),

	Model_Ramses        = ( 4 << 24 ),

	Model_Ramses_All    = ( Model_Ramses | 0xffffff ),
	Model_Ramses_MNCI   = ( Model_Ramses | 0x000001 ),
};

/**
 * The vendor of the device
 */
enum OVendor {
	Vendor_Unknown,

	Vendor_HP,
	Vendor_Sharp,
	Vendor_SIEMENS,
	Vendor_MundN,
};

/**
 * The System used
 */
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
	HardKey_Mail      = Qt::Key_F13,
	HardKey_Record    = Qt::Key_F24,
	HardKey_Suspend   = Qt::Key_F34,
	HardKey_Backlight = Qt::Key_F35,
};

enum ODirection {
	CW   = 0,
	CCW  = 1,
	Flip = 2,
};

/**
 * A singleton which gives informations about device specefic option
 * like the Hardware used, LEDs, the Base Distribution and
 * hardware key mappings.
 *
 * @short A small class for device specefic options
 * @see QObject
 * @author Robert Griebl
 * @version 1.0
 */
class ODevice : public QObject {
	Q_OBJECT

private:
        /* disable copy */
	ODevice ( const ODevice & );

protected:
	ODevice ( );
	virtual void init ( );
	virtual void initButtons ( );

	ODeviceData *d;

public:
	// sandman do we want to allow destructions? -zecke?
	virtual ~ODevice ( );

	static ODevice *inst ( );

	// information

	QString modelString ( ) const;
	OModel model ( ) const;
	inline OModel series ( ) const { return (OModel) ( model ( ) & Model_Series_Mask ); }

	QString vendorString ( ) const;
	OVendor vendor ( ) const;

	QString systemString ( ) const;
	OSystem system ( ) const;

	QString systemVersionString ( ) const;

	Transformation rotation ( ) const;
	ODirection direction ( ) const;

// system

	virtual bool setSoftSuspend ( bool on );
	virtual bool suspend ( );

	virtual bool setDisplayStatus ( bool on );
	virtual bool setDisplayBrightness ( int brightness );
	virtual int displayBrightnessResolution ( ) const;
	virtual bool setDisplayContrast ( int contrast );
	virtual int displayContrastResolution ( ) const;

// input / output
        //FIXME playAlarmSound and al might be better -zecke
	virtual void alarmSound ( );
	virtual void keySound ( );
	virtual void touchSound ( );

	virtual QValueList <OLed> ledList ( ) const;
	virtual QValueList <OLedState> ledStateList ( OLed led ) const;
	virtual OLedState ledState ( OLed led ) const;
	virtual bool setLedState ( OLed led, OLedState st );

	virtual bool hasLightSensor ( ) const;
	virtual int readLightSensor ( );
	virtual int lightSensorResolution ( ) const;

	const QStrList &allowedCpuFrequencies() const;
	bool setCurrentCpuFrequency(uint index);

	/**
	 * Returns the available buttons on this device.  The number and location
	 * of buttons will vary depending on the device.  Button numbers will be assigned
	 * by the device manufacturer and will be from most preferred button to least preffered
	 * button.  Note that this list only contains "user mappable" buttons.
	 */
	const QValueList<ODeviceButton> &buttons ( );

	/**
	 * Returns the DeviceButton for the \a keyCode.  If \a keyCode is not found, it
	 * returns 0L
	 */
	const ODeviceButton *buttonForKeycode ( ushort keyCode );

	/**
	 * Reassigns the pressed action for \a button.  To return to the factory
	 * default pass an empty string as \a qcopMessage.
	 */
	void remapPressedAction ( int button, const OQCopMessage &qcopMessage );

	/**
	 * Reassigns the held action for \a button.  To return to the factory
	 * default pass an empty string as \a qcopMessage.
	 */
	void remapHeldAction ( int button, const OQCopMessage &qcopMessage );

	/**
	 * How long (in ms) you have to press a button for a "hold" action
	 */
	uint buttonHoldTime ( ) const;

signals:
	void buttonMappingChanged ( );

private slots:
	void systemMessage ( const QCString &, const QByteArray & );

protected:
	void reloadButtonMapping ( );
	/* ugly virtual hook */
	virtual void virtual_hook( int id, void* data );
};

}

#endif

