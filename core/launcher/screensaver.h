#ifndef __LAUNCHER_SCREENSAVER_H__
#define __LAUNCHER_SCREENSAVER_H__

#include <qwindowsystem_qws.h>
#include <qobject.h>

#include <qpe/power.h>

class OpieScreenSaver : public QObject, public QWSScreenSaver
{
public:
	OpieScreenSaver ( );

	void restore ( );
	bool save ( int level );

	void setIntervals( int i1, int i2, int i3 );
	void setIntervalsAC( int i1, int i2, int i3 );

	void setInterval ( int interval );

	void setMode ( int mode );

	void setBacklight ( int bright );
	void setDisplayState ( bool on );

	void powerStatusChanged ( PowerStatus ps );

private:
	void setBacklightInternal ( int bright );

protected:
	virtual void timerEvent ( QTimerEvent * );

private:
	int m_disable_suspend;
	bool m_enable_dim;
	bool m_enable_lightoff;
	bool m_enable_suspend;
	bool m_onlylcdoff;

	bool m_enable_dim_ac;
	bool m_enable_lightoff_ac;
	bool m_enable_suspend_ac;
	bool m_onlylcdoff_ac;

	bool m_use_light_sensor;
	int m_backlight_sensor;

	enum {
		LS_Interval = 0,
		LS_Steps,
		LS_SensorMin,
		LS_SensorMax,
		LS_LightMin,
		LS_LightMax,
	
		LS_Count
	};

	int m_sensordata [LS_Count];

	bool m_lcd_status;

	int m_backlight_normal;
	int m_backlight_current;
	bool m_backlight_forcedoff;

	bool m_on_ac;
	
	int m_level;
};

#endif
