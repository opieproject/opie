/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

// redone by Maximilian Reiss <harlekin@handhelds.org>

#include "settings.h"

#include <qpe/global.h>
#include <qpe/fontmanager.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/power.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qlabel.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qslider.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qdir.h>
#if QT_VERSION >= 300
#include <qstylefactory.h>
#endif

#include <opie/odevice.h>

using namespace Opie;

LightSettings::LightSettings( QWidget* parent,  const char* name, WFlags )
    : LightSettingsBase( parent, name, TRUE, WStyle_ContextHelp )
{

    if ( ODevice::inst()->hasLightSensor() ) {
        // Not supported yet - hide until implemented
        CalibrateLightSensor->setEnabled( false );
        CalibrateLightSensorAC->setEnabled( false );
    } else {
        // if ipaq no need to show the sensor box
        auto_brightness->hide();
        CalibrateLightSensor->hide();
        auto_brightness_ac_3->hide();
        CalibrateLightSensorAC->hide();
    }

    Config config( "apm" );
    config.setGroup( "Battery" );

    // battery spinboxes
    interval_dim->setValue( config.readNumEntry( "Dim", 20 ));
    interval_lightoff->setValue( config.readNumEntry( "LightOff", 30 ));
    interval_suspend->setValue( config.readNumEntry( "Suspend", 60 ));

    // battery check and slider
    LcdOffOnly->setChecked( config.readBoolEntry("LcdOffOnly",false));
    int maxbright = ODevice::inst ( )-> displayBrightnessResolution ( );
    initbright = config.readNumEntry("Brightness",255);
    brightness->setMaxValue( maxbright );
    brightness->setTickInterval( QMAX(1,maxbright/16) );
    brightness->setLineStep( QMAX(1,maxbright/16) );
    brightness->setPageStep( QMAX(1,maxbright/16) );
    brightness->setValue( (maxbright*255 - initbright*maxbright)/255 );

    // light sensor
    auto_brightness->setChecked( config.readNumEntry("LightSensor",0) != 0 );



    config.setGroup( "AC" );
    // ac spinboxes
    interval_dim_ac_3->setValue( config.readNumEntry( "Dim", 20 ));
    interval_lightoff_ac_3->setValue( config.readNumEntry( "LightOff", 30 ));
    interval_suspend_ac_3->setValue( config.readNumEntry( "Suspend", 60 ));

    // ac check and slider
    LcdOffOnly_2_3->setChecked( config.readBoolEntry("LcdOffOnly",false));
    int maxbright_ac = ODevice::inst ( )-> displayBrightnessResolution ( );
    initbright_ac = config.readNumEntry("Brightness",255);
    brightness_ac_3->setMaxValue( maxbright_ac );
    brightness_ac_3->setTickInterval( QMAX(1,maxbright_ac/16) );
    brightness_ac_3->setLineStep( QMAX(1,maxbright_ac/16) );
    brightness_ac_3->setPageStep( QMAX(1,maxbright_ac/16) );
    brightness_ac_3->setValue( (maxbright_ac*255 - initbright_ac*maxbright_ac)/255 );

    // light sensor
    auto_brightness_ac_3->setChecked( config.readNumEntry("LightSensor",0) != 0 );


    //LightStepSpin->setValue( config.readNumEntry("Steps", 10 ) );
    //LightMinValueSlider->setValue( config.readNumEntry("MinValue", 70 ) );
    //connect( LightStepSpin, SIGNAL( valueChanged( int ) ), this, SLOT( slotSliderTicks( int ) ) ) ;
    //LightShiftSpin->setValue( config.readNumEntry("Shift", 0 ) );

    // advanced settings
    config.setGroup( "Warnings" );
    warnintervalBox->setValue( config.readNumEntry("checkinterval", 10000)/1000 );
    lowSpinBox->setValue( config.readNumEntry("powerverylow", 10 ) );
    criticalSpinBox->setValue( config.readNumEntry("powercritical", 5 ) );

    connect( brightness, SIGNAL( valueChanged(int) ), this, SLOT( applyBrightness() ) );
    connect( brightness_ac_3, SIGNAL( valueChanged(int) ), this, SLOT( applyBrightnessAC() ) );
}

LightSettings::~LightSettings() {
}

void LightSettings::slotSliderTicks( int steps ) {
//    LightMinValueSlider->setTickInterval( steps );
}

static void set_fl(int bright)
{
    qDebug ( QString( "Brightness" ).arg( bright ) );
    QCopEnvelope e("QPE/System", "setBacklight(int)" );
    e << bright;
}

void LightSettings::reject()
{
    set_fl(initbright);
    QDialog::reject();
}

void LightSettings::accept()
{
    if ( qApp->focusWidget() ) {
        qApp->focusWidget()->clearFocus();
    }

    applyBrightness();

    // bat
    int i_dim =      interval_dim->value();
    int i_lightoff = interval_lightoff->value();
    int i_suspend =  interval_suspend->value();

    // ac
    int i_dim_ac =      interval_dim_ac_3->value();
    int i_lightoff_ac = interval_lightoff_ac_3->value();
    int i_suspend_ac =  interval_suspend_ac_3->value();

    Config config( "apm" );

    config.setGroup( "Battery" );

    // bat
    config.writeEntry( "LcdOffOnly", LcdOffOnly->isChecked() );
    config.writeEntry( "Dim", i_dim );
    config.writeEntry( "LightOff", i_lightoff );
    config.writeEntry( "Suspend", i_suspend );
    config.writeEntry( "Brightness",
    ( brightness->value() ) * 255 / brightness->maxValue() );

    // ac
    config.setGroup( "AC" );
    config.writeEntry( "LcdOffOnly", LcdOffOnly_2_3->isChecked() );
    config.writeEntry( "Dim", i_dim_ac );
    config.writeEntry( "LightOff", i_lightoff_ac );
    config.writeEntry( "Suspend", i_suspend_ac );
    config.writeEntry( "Brightness",
    ( brightness_ac_3->value()) * 255 / brightness_ac_3->maxValue() );


    // only make light sensor stuff appear if the unit has a sensor
    if ( ODevice::inst()->hasLightSensor() ) {
        config.setGroup( "Battery" );
        config.writeEntry( "LightSensor", (int)auto_brightness->isChecked() );
        config.setGroup( "AC" );
        config.writeEntry( "LightSensor", (int)auto_brightness_ac_3->isChecked() );
        //config.writeEntry( "Steps", LightStepSpin->value() );
        //onfig.writeEntry( "MinValue", LightMinValueSlider->value() );
        //config.writeEntry( "Shift", LightShiftSpin->value() );
    }


    // advanced
    config.setGroup( "Warnings" );
    config.writeEntry( "check_interval", warnintervalBox->value()*1000 );
    config.writeEntry( "power_verylow",  lowSpinBox->value() );
    config.writeEntry( "power_critical", criticalSpinBox->value() );
    config.write();

	{
		QCopEnvelope e ( "QPE/System", "reloadPowerWarnSettings()" );
	}
	{
		QCopEnvelope e ( "QPE/System", "setScreenSaverInterval(int)" );
		e << -1;
	}
	{
		QCopEnvelope e ( "QPE/System", "setBacklight(int)" );
		e << -1;
	}
    
    QDialog::accept();
}

void LightSettings::applyBrightness()
{
    if ( PowerStatusManager::readStatus().acStatus() != PowerStatus::Online ) {
        int bright = ( brightness->value() ) * 255 / brightness->maxValue();
        set_fl(bright);
    }
}

void LightSettings::applyBrightnessAC()
{
    // if ac is attached, set directly that sliders setting, else the "on battery" sliders setting
    if ( PowerStatusManager::readStatus().acStatus() ==  PowerStatus::Online ) {
        int bright = ( brightness_ac_3->value() ) * 255  / brightness_ac_3->maxValue();
        set_fl(bright);
    }
}

void LightSettings::done(int r)
{
  QDialog::done(r);
  close();
}
