
#include "screensaver.h"

#include <qpe/config.h>
#include <qpe/power.h>
#include <qpe/network.h>

#include <opie2/odevice.h>


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
	::memset ( m_sensordata, 0xff, LS_Count * sizeof( m_sensordata [0] ));

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


/**
 * Stops the screen saver
 */
void OpieScreenSaver::restore()
{
	m_level = -1;

	if ( !m_lcd_status ) {    // We must have turned it off
		ODevice::inst ( ) -> setDisplayStatus ( true );
		m_lcd_status = true;
	}

	setBacklightInternal ( -1 );
}


/**
 * Starts the screen saver
 *
 * @param level what level of screen saving should happen (0=lowest non-off, 1=off,
 * 2=suspend whole machine)
 * @returns true on success
 */
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
				// TODO: why is this key F34 hard coded?  -- schurig
				// Does this now only work an devices with a ODevice::filter?
				QWSServer::sendKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );
				return true;
			}

			break;
	}
	return false;
}


/**
 * Set intervals in seconds for automatic dimming, light off and suspend
 *
 * This function also sets the member variables m_m_enable_dim[_ac],
 * m_enable_lightoff[_ac], m_enable_suspend[_ac], m_onlylcdoff[_ac]
 *
 * @param dim      time in seconds to dim, -1 to read value from config file,
 *                 0 to disable
 * @param lightoff time in seconds to turn LCD backlight off, -1 to
 *                 read value from config file, 0 to disable
 * @param suspend  time in seconds to do an APM suspend, -1 to
 *                 read value from config file, 0 to disable
 */
void OpieScreenSaver::setIntervals ( int dim, int lightoff, int suspend )
{
	Config config ( "apm" );
	config. setGroup ( m_on_ac ? "AC" : "Battery" );

	int v[ 4 ];
	if ( dim < 0 )
		dim = config. readNumEntry ( "Dim", m_on_ac ? 60 : 30 );
	if ( lightoff < 0 )
		lightoff = config. readNumEntry ( "LightOff", m_on_ac ? 120 : 20 );
	if ( suspend < 0 )
		suspend = config. readNumEntry ( "Suspend", m_on_ac ? 0 : 60 );

	if ( m_on_ac ) {
		m_enable_dim_ac = ( dim > 0 );
		m_enable_lightoff_ac = ( lightoff > 0 );
		m_enable_suspend_ac = ( suspend > 0 );
		m_onlylcdoff_ac = config.readBoolEntry ( "LcdOffOnly", false );
	}
	else {
		m_enable_dim = ( dim > 0 );
		m_enable_lightoff = ( lightoff > 0 );
		m_enable_suspend = ( suspend > 0 );
		m_onlylcdoff = config.readBoolEntry ( "LcdOffOnly", false );
	}
	
	//qDebug("screen saver intervals: %d %d %d", dim, lightoff, suspend);

	v [ 0 ] = QMAX( 1000 * dim, 100 );
	v [ 1 ] = QMAX( 1000 * lightoff, 100 );
	v [ 2 ] = QMAX( 1000 * suspend, 100 );
	v [ 3 ] = 0;

	if ( !dim && !lightoff && !suspend )
		QWSServer::setScreenSaverInterval( 0 );
	else
		QWSServer::setScreenSaverIntervals( v );
}


/**
 * Set suspend time. Will read the dim and lcd-off times from the config file.
 *
 * @param suspend  time in seconds to go into APM suspend, -1 to
 *                 read value from config file, 0 to disable
 */
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


/**
 * Set display brightness
 *
 * Get's default values for backlight, contrast and light sensor from config file.
 *
 * @param bright desired brighness (-1 to use automatic sensor data or value
 *               from config file, -2 to toggle backlight on and off, -3 to
 *               force backlight off)
 */
void OpieScreenSaver::setBacklight ( int bright )
{
	// Read from config
	Config config ( "apm" );
	config. setGroup ( m_on_ac ? "AC" : "Battery" );
	m_backlight_normal = config. readNumEntry ( "Brightness", m_on_ac ? 255 : 127 );
	int contrast = config. readNumEntry ( "Contrast", 127);
	m_use_light_sensor = config. readBoolEntry ( "LightSensor", false );

	//qDebug ( "setBacklight: %d (norm: %d) (ls: %d)", bright, m_backlight_normal, m_use_light_sensor ? 1 : 0 );

	killTimers ( );
	if (( bright < 0 ) && m_use_light_sensor ) {
		QStringList sl = config. readListEntry ( "LightSensorData", ';' );
		
		m_sensordata [LS_SensorMin] = 40;
		m_sensordata [LS_SensorMax] = 215;
		m_sensordata [LS_LightMin] = 1;
		m_sensordata [LS_LightMax] = 255;
		m_sensordata [LS_Steps] = 12;
		m_sensordata [LS_Interval] = 2000;		
		
		for ( uint i = 0; i < LS_Count; i++ ) {
			if ( i < sl. count ( ))
				m_sensordata [i] = sl [i]. toInt ( );
		}
		if ( m_sensordata [LS_Steps] < 2 ) // sanity check to avoid SIGFPE
			m_sensordata [LS_Steps] = 2;

		timerEvent ( 0 );
		startTimer ( m_sensordata [LS_Interval] );
	}

	setBacklightInternal ( bright );
	ODevice::inst ( )-> setDisplayContrast(contrast);
}


/**
 * Internal brightness setting method
 *
 * Get's default values for backlight and light sensor from config file.
 *
 * @param bright desired brighness (-1 to use automatic sensor data or value
 *               from config file, -2 to toggle backlight on and off, -3 to
 *               force backlight off)
 */
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


/**
 * Timer event used for automatic setting the backlight according to a light sensor
 * and to set the default brightness
 */
void OpieScreenSaver::timerEvent ( QTimerEvent * )
{
	int s = ODevice::inst ( )-> readLightSensor ( ) * 256 / ODevice::inst ( )-> lightSensorResolution ( );
	
	if ( s < m_sensordata [LS_SensorMin] )
		m_backlight_sensor = m_sensordata [LS_LightMax];
	else if ( s >= m_sensordata [LS_SensorMax] )
		m_backlight_sensor = m_sensordata [LS_LightMin];
	else {
		int dx = m_sensordata [LS_SensorMax] - m_sensordata [LS_SensorMin];
		int dy = m_sensordata [LS_LightMax] - m_sensordata [LS_LightMin];
		
		int stepno = ( s - m_sensordata [LS_SensorMin] ) * m_sensordata [LS_Steps] / dx; // dx is never 0
		
		m_backlight_sensor = m_sensordata [LS_LightMax] - dy * stepno / ( m_sensordata [LS_Steps] - 1 );
	}

	//qDebug ( "f(%d) = %d [%d - %d] -> [%d - %d] / %d", s, m_backlight_sensor, m_sensordata [LS_SensorMin], m_sensordata [LS_SensorMax], m_sensordata [LS_LightMin], m_sensordata [LS_LightMax], m_sensordata [LS_Steps] );

	if ( m_level <= 0 )
		setBacklightInternal ( -1 );
}


/**
 * Like ODevice::setDisplayStatus(), but keep current state in m_lcd_status.
 */
void OpieScreenSaver::setDisplayState ( bool on )
{
	if ( m_lcd_status != on ) {
		ODevice::inst ( ) -> setDisplayStatus ( on );
		m_lcd_status = on;
	}
}


/**
 * Set display to default ac/battery settings when power status changed.
 */
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
