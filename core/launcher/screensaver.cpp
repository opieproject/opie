
#include "screensaver.h"

#include <qpe/config.h>
#include <qpe/power.h>
#include <qpe/network.h>

#include <opie/odevice.h>


using namespace Opie;


OpieScreenSaver::OpieScreenSaver ( )
		: QObject ( 0, "screensaver" ), QWSScreenSaver ( )
{
	m_disable_suspend = 100;
	m_enable_dim = false;
	m_enable_lightoff = false;
	m_enable_suspend = false;
	m_onlylcdoff = false;

	m_enable_dim_ac = false;
	m_enable_lightoff_ac = false;
	m_enable_suspend_ac = false;
	m_onlylcdoff_ac = false;

	m_use_light_sensor = false;
	m_backlight_sensor = -1;

	m_lcd_status = true;

	m_backlight_normal = -1;
	m_backlight_current = -1;
	m_backlight_forcedoff = false;

	m_on_ac = false;

	m_level = -1;

	// Make sure the LCD is in fact on, (if opie was killed while the LCD is off it would still be off)
	ODevice::inst ( )-> setDisplayStatus ( true );
	setBacklight ( -1 );
}


void OpieScreenSaver::restore()
{
	m_level = -1;

	if ( !m_lcd_status ) {    // We must have turned it off
		ODevice::inst ( ) -> setDisplayStatus ( true );
		m_lcd_status = true;
	}

	setBacklightInternal ( -1 );
}


bool OpieScreenSaver::save( int level )
{
	m_level = level;

	switch ( level ) {
		case 0:
			if (( m_on_ac && m_enable_dim_ac ) ||
			        ( !m_on_ac && m_enable_dim )) {
				if (( m_disable_suspend > 0 ) && ( m_backlight_current > 1 ) && !m_use_light_sensor )
					setBacklightInternal ( 1 ); // lowest non-off
			}
			return true;
			break;

		case 1:
			if (( m_on_ac && m_enable_lightoff_ac ) ||
			        ( !m_on_ac && m_enable_lightoff )) {
				if ( m_disable_suspend > 1 )
					setBacklightInternal ( 0 ); // off
			}
			return true;
			break;

		case 2:
			if (( m_on_ac && !m_enable_suspend_ac ) ||
			        ( !m_on_ac && !m_enable_suspend )) {
				return true;
			}

			if (( m_on_ac && m_onlylcdoff_ac ) ||
			        ( !m_on_ac && m_onlylcdoff )) {
				ODevice::inst ( ) -> setDisplayStatus ( false );
				m_lcd_status = false;
				return true;
			}

			// We're going to suspend the whole machine

			if (( m_disable_suspend > 2 ) && !Network::networkOnline ( )) {
				QWSServer::sendKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );
				return true;
			}

			break;
	}
	return false;
}


void OpieScreenSaver::setIntervals ( int i1, int i2, int i3 )
{
	Config config ( "apm" );
	config. setGroup ( m_on_ac ? "AC" : "Battery" );

	int v[ 4 ];
	if ( i1 < 0 )
		i1 = config. readNumEntry ( "Dim", 30 );
	if ( i2 < 0 )
		i2 = config. readNumEntry ( "LightOff", 20 );
	if ( i3 < 0 )
		i3 = config. readNumEntry ( "Suspend", 60 );

	if ( m_on_ac ) {
		m_enable_dim_ac = ( i1 > 0 );
		m_enable_lightoff_ac = ( i2 > 0 );
		m_enable_suspend_ac = ( i3 > 0 );
		m_onlylcdoff_ac = config.readNumEntry ( "LcdOffOnly", 0 );
	}
	else {
		m_enable_dim = ( i1 > 0 );
		m_enable_lightoff = ( i2 > 0 );
		m_enable_suspend = ( i3 > 0 );
		m_onlylcdoff = config.readNumEntry ( "LcdOffOnly", 0 );
	}
	
	qDebug("screen saver intervals: %d %d %d", i1, i2, i3);

	v [ 0 ] = QMAX( 1000 * i1, 100 );
	v [ 1 ] = QMAX( 1000 * i2, 100 );
	v [ 2 ] = QMAX( 1000 * i3, 100 );
	v [ 3 ] = 0;

	if ( !i1 && !i2 && !i3 )
		QWSServer::setScreenSaverInterval( 0 );
	else
		QWSServer::setScreenSaverIntervals( v );
}


void OpieScreenSaver::setInterval ( int interval )
{
	setIntervals ( -1, -1, interval );
}


void OpieScreenSaver::setMode ( int mode )
{
	if ( mode > m_disable_suspend )
		setInterval ( -1 );
	m_disable_suspend = mode;
}


void OpieScreenSaver::setBacklight ( int bright )
{
	// Read from config
	Config config ( "apm" );
	config. setGroup ( m_on_ac ? "AC" : "Battery" );
	m_backlight_normal = config. readNumEntry ( "Brightness", 255 );

	m_use_light_sensor = config. readBoolEntry ( "LightSensor", false );

	qDebug ( "setBacklight: %d (ls: %d)", m_backlight_normal, m_use_light_sensor ? 1 : 0 );

	killTimers ( );
	if ( m_use_light_sensor ) {
		timerEvent ( 0 );
		startTimer ( 2000 );
	}

	setBacklightInternal ( bright );
}


void OpieScreenSaver::setBacklightInternal ( int bright )
{
	if ( bright == -3 ) {
		// Forced on
		m_backlight_forcedoff = false;
		bright = -1;
	}
	if ( m_backlight_forcedoff && bright != -2 )
		return ;
	if ( bright == -2 ) {
		// Toggle between off and on
		bright = m_backlight_current ? 0 : -1;
		m_backlight_forcedoff = !bright;
	}
	if ( bright == -1 )
		bright = m_use_light_sensor ? m_backlight_sensor : m_backlight_normal;

	if ( bright != m_backlight_current ) {
		ODevice::inst ( )-> setDisplayBrightness ( bright );
		m_backlight_current = bright;
	}
}


void OpieScreenSaver::timerEvent ( QTimerEvent * )
{
	m_backlight_sensor = (( 255 - ODevice::inst ( )-> readLightSensor ( )) * m_backlight_normal ) / 255;

	if ( m_level <= 0 )
		setBacklightInternal ( -1 );
}


void OpieScreenSaver::setDisplayState ( bool on )
{
	if ( m_lcd_status != on ) {
		ODevice::inst ( ) -> setDisplayStatus ( on );
		m_lcd_status = on;
	}
}


void OpieScreenSaver::powerStatusChanged ( PowerStatus ps )
{
	bool newonac = ( ps. acStatus ( ) == PowerStatus::Online );
	
	if ( newonac != m_on_ac ) {
		m_on_ac = newonac;
		setInterval ( -1 );
		setBacklight ( -1 );
		restore ( );		
	}
}

