/*
                             This file is part of the OPIE Project
               =.            Copyright (c)  2002 Maximilian Reiss <harlekin@handhelds.org>
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "light.h"

#include <qpe/config.h>
#include <qpe/power.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif
#include <qpe/qpeapplication.h>

#include <qlabel.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>

#include <opie/odevice.h>

#include "sensor.h"

using namespace Opie;

LightSettings::LightSettings( QWidget* parent,  const char* name, WFlags )
	: LightSettingsBase( parent, name, false, WStyle_ContextHelp )
{
	m_bres = ODevice::inst ( )-> displayBrightnessResolution ( );
	m_cres = ODevice::inst ( )-> displayContrastResolution ( );

    // check whether to show the light sensor stuff

	if ( !ODevice::inst ( )-> hasLightSensor ( )) {
		auto_brightness-> hide ( );
		CalibrateLightSensor-> hide ( );
		auto_brightness_ac-> hide ( );
		CalibrateLightSensor_ac-> hide ( );
	}

    // check whether to show the contrast stuff

	if (m_cres) {
		GroupLight->setTitle(tr("Backlight && Contrast"));
		GroupLight_ac->setTitle(GroupLight->title());
	} else {
		contrast->hide();
		contrast_ac->hide();
	}

    // check whether to show the cpu frequency stuff

	QStrList freq = ODevice::inst()->allowedCpuFrequencies();
	if ( freq.count() ) {
		frequency->insertStrList( freq );
		frequency_ac->insertStrList( freq );
	} else {
		frequencyLabel->hide();
		frequency->hide();
		frequencyLabel_ac->hide();
		frequency_ac->hide();
	}

    // check whether to show the hinge action stuff

    if ( !ODevice::inst()->hasHingeSensor() ) {
        closeHingeLabel->hide();
        closeHingeAction->hide();
        closeHingeLabel_ac->hide();
        closeHingeAction_ac->hide();
    }

	Config config ( "apm" );
	config. setGroup ( "Battery" );

	// battery spinboxes
	interval_dim->      setValue ( config. readNumEntry ( "Dim", 30 ));
	interval_lightoff-> setValue ( config. readNumEntry ( "LightOff", 20 ));
	interval_suspend->  setValue ( config. readNumEntry ( "Suspend", 60 ));

	// battery check and slider
	LcdOffOnly->setChecked ( config. readBoolEntry ( "LcdOffOnly", false ));

	// CPU frequency
	frequency->setCurrentItem( config.readNumEntry("Freq", 0) );

	// hinge action
	closeHingeAction->setCurrentItem( config.readNumEntry("CloseHingeAction", 0) );

    int bright = config. readNumEntry ( "Brightness", 127 );
	int contr  = m_oldcontrast = config. readNumEntry ( "Contrast", 127 );
	brightness-> setTickInterval ( QMAX( 16, 256 / m_bres ));
	brightness-> setLineStep ( QMAX( 1, 256 / m_bres ));
	brightness-> setPageStep ( QMAX( 1, 256 / m_bres ));
	brightness-> setValue ( bright );

	if (m_cres) {
		contrast-> setTickInterval ( QMAX( 16, 256 / m_cres ));
		contrast-> setLineStep ( QMAX( 1, 256 / m_cres ));
		contrast-> setPageStep ( QMAX( 1, 256 / m_cres ));
		contrast-> setValue ( contr );
	}

	// light sensor
	auto_brightness-> setChecked ( config. readBoolEntry ( "LightSensor", false ));
	m_sensordata = config. readListEntry ( "LightSensorData", ';' );

	config. setGroup ( "AC" );

	// ac spinboxes
	interval_dim_ac->      setValue ( config. readNumEntry ( "Dim", 60 ));
	interval_lightoff_ac-> setValue ( config. readNumEntry ( "LightOff", 120 ));
	interval_suspend_ac->  setValue ( config. readNumEntry ( "Suspend", 0 ));

	// ac check and slider
	LcdOffOnly_ac-> setChecked ( config. readBoolEntry ( "LcdOffOnly", false ));

	// CPU frequency
	frequency_ac->setCurrentItem( config.readNumEntry("Freq", 0) );

	// hinge action
	closeHingeAction_ac->setCurrentItem( config.readNumEntry("CloseHingeAction", 0) );

	bright = config. readNumEntry ( "Brightness", 255 );
	brightness_ac-> setTickInterval ( QMAX( 16, 256 / m_bres ));
	brightness_ac-> setLineStep ( QMAX( 1, 256 / m_bres ));
	brightness_ac-> setPageStep ( QMAX( 1, 256 / m_bres ));
	brightness_ac-> setValue ( bright );

	if (m_cres) {
		contr = config. readNumEntry ( "Contrast", 127);
		contrast_ac-> setTickInterval ( QMAX( 16, 256 / m_cres ));
		contrast_ac-> setLineStep ( QMAX( 1, 256 / m_cres ));
		contrast_ac-> setPageStep ( QMAX( 1, 256 / m_cres ));
		contrast_ac-> setValue ( contr );
	}

	// light sensor
	auto_brightness_ac-> setChecked ( config. readBoolEntry ( "LightSensor", false ));
	m_sensordata_ac = config. readListEntry ( "LightSensorData", ';' );

	// warnings
	config. setGroup ( "Warnings" );
	warnintervalBox-> setValue ( config. readNumEntry ( "checkinterval", 10000 ) / 1000 );
	lowSpinBox->      setValue ( config. readNumEntry ( "powerverylow", 10 ) );
	criticalSpinBox-> setValue ( config. readNumEntry ( "powercritical", 5 ) );

	m_resettimer = new QTimer ( this );
	connect ( m_resettimer, SIGNAL( timeout ( )), this, SLOT( resetBacklight ( )));

	if ( PowerStatusManager::readStatus ( ). acStatus ( ) != PowerStatus::Online ) {
		tabs-> setCurrentPage ( 0 );
	}
	else {
		tabs-> setCurrentPage ( 1 );
	}

	connect ( brightness, SIGNAL( valueChanged ( int )), this, SLOT( setBacklight ( int )));
	connect ( brightness_ac, SIGNAL( valueChanged ( int )), this, SLOT( setBacklight ( int )));
	if (m_cres) {
		connect ( contrast,    SIGNAL( valueChanged ( int )), this, SLOT( setContrast ( int )));
		connect ( contrast_ac, SIGNAL( valueChanged ( int )), this, SLOT( setContrast ( int )));
	}
	connect( frequency, SIGNAL( activated(int) ), this, SLOT( setFrequency(int) ) );
	connect( frequency_ac, SIGNAL( activated(int) ), this, SLOT( setFrequency(int) ) );
	connect( closeHingeAction, SIGNAL( activated(int) ), this, SLOT( setCloseHingeAction(int) ) );
	connect( closeHingeAction_ac, SIGNAL( activated(int) ), this, SLOT( setCloseHingeAction(int) ) );
}

LightSettings::~LightSettings ( )
{
}

void LightSettings::calibrateSensor ( )
{
	Sensor *s = new Sensor ( m_sensordata, this );
	connect ( s, SIGNAL( viewBacklight ( int )), this, SLOT( setBacklight ( int )));
	QPEApplication::execDialog( s );
	delete s;
}

void LightSettings::calibrateSensorAC ( )
{
	Sensor *s = new Sensor ( m_sensordata_ac, this );
	connect ( s, SIGNAL( viewBacklight ( int )), this, SLOT( setBacklight ( int )));
	s-> showMaximized ( );
	s-> exec ( );
	delete s;
}

void LightSettings::setBacklight ( int bright )
{
	QCopEnvelope e ( "QPE/System", "setBacklight(int)" );
	e << bright;

	if ( bright != -1 ) {
		m_resettimer-> stop ( );
		m_resettimer-> start ( 4000, true );
	}
}

void LightSettings::setContrast ( int contr )
{
	if (contr == -1) contr = m_oldcontrast;
	ODevice::inst ( )-> setDisplayContrast(contr);
}

void LightSettings::setFrequency ( int index )
{
	qWarning("LightSettings::setFrequency(%d)", index);
	ODevice::inst ( )-> setCurrentCpuFrequency(index);
}

void LightSettings::resetBacklight ( )
{
	setBacklight ( -1 );
	setContrast ( -1 );
}

void LightSettings::setCloseHingeAction ( int index )
{
    qWarning("LightSettings::setCloseHingeStatus(%d)", index);
}

void LightSettings::accept ( )
{
	Config config ( "apm" );

	// bat
	config. setGroup ( "Battery" );
	config. writeEntry ( "LcdOffOnly", LcdOffOnly-> isChecked ( ));
	config. writeEntry ( "Dim",        interval_dim-> value ( ));
	config. writeEntry ( "LightOff",   interval_lightoff-> value ( ));
	config. writeEntry ( "Suspend",    interval_suspend-> value ( ));
	config. writeEntry ( "Brightness", brightness-> value () );
	if (m_cres)
	config. writeEntry ( "Contrast",   contrast-> value () );
	config. writeEntry ( "Freq",       frequency->currentItem() );
	config. writeEntry ( "CloseHingeAction", closeHingeAction->currentItem() );

	// ac
	config. setGroup ( "AC" );
	config. writeEntry ( "LcdOffOnly", LcdOffOnly_ac-> isChecked ( ));
	config. writeEntry ( "Dim",        interval_dim_ac-> value ( ));
	config. writeEntry ( "LightOff",   interval_lightoff_ac-> value ( ));
	config. writeEntry ( "Suspend",    interval_suspend_ac-> value ( ));
	config. writeEntry ( "Brightness", brightness_ac-> value () );
	if (m_cres)
	config. writeEntry ( "Contrast",   contrast_ac-> value () );
	config. writeEntry ( "Freq",       frequency_ac->currentItem() );
	config. writeEntry ( "CloseHingeAction", closeHingeAction_ac->currentItem() );

	// only make light sensor stuff appear if the unit has a sensor
	if ( ODevice::inst ( )-> hasLightSensor ( )) {
		config. setGroup ( "Battery" );
		config. writeEntry ( "LightSensor", auto_brightness->isChecked() );
		config. writeEntry ( "LightSensorData", m_sensordata, ';' );
		config. setGroup ( "AC" );
		config. writeEntry ( "LightSensor", auto_brightness_ac->isChecked() );
		config. writeEntry ( "LightSensorData", m_sensordata_ac, ';' );
	}

	// advanced
	config. setGroup ( "Warnings" );
	config. writeEntry ( "check_interval", warnintervalBox-> value ( ) * 1000 );
	config. writeEntry ( "power_verylow",  lowSpinBox-> value ( ));
	config. writeEntry ( "power_critical", criticalSpinBox-> value ( ));
	config. write ( );

	// notify the launcher
	{
		QCopEnvelope e ( "QPE/System", "reloadPowerWarnSettings()" );
	}
	{
		QCopEnvelope e ( "QPE/System", "setScreenSaverInterval(int)" );
		e << -1;
	}
	LightSettingsBase::accept ( );
}

void LightSettings::done ( int r )
{
	m_resettimer-> stop ( );
	resetBacklight ( );

	LightSettingsBase::done ( r );
	close ( );
}
