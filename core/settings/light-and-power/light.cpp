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
#include <qlistbox.h>
#include <qdir.h>
#if QT_VERSION >= 300
#include <qstylefactory.h>
#endif

#include <opie/odevice.h>

using namespace Opie;

LightSettings::LightSettings( QWidget* parent,  const char* name, WFlags fl )
    : LightSettingsBase( parent, name, TRUE, fl )
{

    if ( ODevice::inst()->model() ==  Model_iPAQ_H31xx ||
         ODevice::inst()->model() ==  Model_iPAQ_H36xx ||
         ODevice::inst()->model() ==  Model_iPAQ_H37xx ||
         ODevice::inst()->model() ==  Model_iPAQ_H38xx ) {
        // Not supported yet - hide until implemented
        IpaqGroupBox->setEnabled( false );
        IpaqGroupBoxAC->setEnabled( false );
        LightSensorGroupBox->setEnabled( false );
    } else {
        // if ipaq no need to show the sensor box
        IpaqGroupBox->hide();
        IpaqGroupBoxAC->hide();
        LightSensorGroupBox->hide();
    }

    Config config( "qpe" );

    config.setGroup( "Screensaver" );

    int interval;

    // battery spinboxes
    interval = config.readNumEntry( "Interval_Dim", 20 );
    interval_dim->setValue( interval );
    interval = config.readNumEntry( "Interval_LightOff", 30 );
    interval_lightoff->setValue( interval );
    interval = config.readNumEntry( "Interval", 60 );
    if ( interval > 3600 ) interval /= 1000; // compatibility (was millisecs)
    interval_suspend->setValue( interval );

    // ac spinboxes
    interval = config.readNumEntry( "Interval_DimAC", 20 );
    interval_dim_ac_3->setValue( interval );
    interval = config.readNumEntry( "Interval_LightOffAC", 30 );
    interval_lightoff_ac_3->setValue( interval );
    interval = config.readNumEntry( "IntervalAC", 60 );
    if ( interval > 3600 ) {
        interval /= 1000; // compatibility (was millisecs)
    }
    interval_suspend_ac_3->setValue( interval );


    // battery check and slider
    screensaver_dim->setChecked( config.readNumEntry("Dim",1) != 0 );
    screensaver_lightoff->setChecked( config.readNumEntry("LightOff",1) != 0 );
    LcdOffOnly->setChecked( config.readNumEntry("LcdOffOnly",0) != 0 );
    int maxbright = ODevice::inst ( )-> displayBrightnessResolution ( );
    initbright = config.readNumEntry("Brightness",255);
    brightness->setMaxValue( maxbright );
    brightness->setTickInterval( QMAX(1,maxbright/16) );
    brightness->setLineStep( QMAX(1,maxbright/16) );
    brightness->setPageStep( QMAX(1,maxbright/16) );
    brightness->setValue( (maxbright*255 - initbright*maxbright)/255 );

    // ac check and slider
    screensaver_dim_ac_3->setChecked( config.readNumEntry("DimAC",1) != 0 );
    screensaver_lightoff_ac_3->setChecked( config.readNumEntry("LightOffAC",1) != 0 );
    LcdOffOnly_2_3->setChecked( config.readNumEntry("LcdOffOnlyAC",0) != 0 );
    int maxbright_ac = ODevice::inst ( )-> displayBrightnessResolution ( );
    initbright_ac = config.readNumEntry("BrightnessAC",255);
    brightness_ac_3->setMaxValue( maxbright_ac );
    brightness_ac_3->setTickInterval( QMAX(1,maxbright_ac/16) );
    brightness_ac_3->setLineStep( QMAX(1,maxbright_ac/16) );
    brightness_ac_3->setPageStep( QMAX(1,maxbright_ac/16) );
    brightness_ac_3->setValue( (maxbright_ac*255 - initbright_ac*maxbright_ac)/255 );

    // advanced settings
    config.setGroup( "APM" );
    warnintervalBox->setValue( config.readNumEntry("check_interval", 10000)/1000 );
    lowSpinBox->setValue( config.readNumEntry("power_verylow", 10 ) );
    criticalSpinBox->setValue( config.readNumEntry("power_critical", 5 ) );

    // ipaq sensor
    config.setGroup( "Ipaq_light_sensor" );
    auto_brightness->setChecked( config.readNumEntry("LightSensor",1) != 0 );
    auto_brightness_ac_3->setChecked( config.readNumEntry("LightSensorAC",1) != 0 );
    LightStepSpin->setValue( config.readNumEntry("Steps", 10 ) );
    LightMinValueSlider->setValue( config.readNumEntry("MinValue", 70 ) );
    connect( LightStepSpin, SIGNAL( valueChanged( int ) ), this, SLOT( slotSliderTicks( int ) ) ) ;
    LightShiftSpin->setValue( config.readNumEntry("Shift", 0 ) );

    connect(brightness, SIGNAL(valueChanged(int)), this, SLOT(applyBrightness()));
    connect(brightness_ac_3, SIGNAL(valueChanged(int)), this, SLOT(applyBrightness()));
}

LightSettings::~LightSettings()
{
}

void LightSettings::slotSliderTicks( int steps ) {
    LightMinValueSlider->setTickInterval( steps );
}

static void set_fl(int bright)
{
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
    if ( qApp->focusWidget() )
        qApp->focusWidget()->clearFocus();

    applyBrightness();

    // bat
    int i_dim =      (screensaver_dim->isChecked() ? interval_dim->value() : 0);
    int i_lightoff = (screensaver_lightoff->isChecked() ? interval_lightoff->value() : 0);
    int i_suspend =  interval_suspend->value();
    QCopEnvelope e("QPE/System", "setScreenSaverIntervals(int,int,int)" );
    e << i_dim << i_lightoff << i_suspend;

    // ac
    int i_dim_ac =      (screensaver_dim_ac_3->isChecked() ? interval_dim_ac_3->value() : 0);
    int i_lightoff_ac = (screensaver_lightoff_ac_3->isChecked() ? interval_lightoff_ac_3->value() : 0);
    int i_suspend_ac =  interval_suspend_ac_3->value();
    QCopEnvelope e_ac("QPE/System", "setScreenSaverIntervalsAC(int,int,int)" );
    e << i_dim_ac << i_lightoff_ac << i_suspend_ac;

    Config config( "qpe" );
    config.setGroup( "Screensaver" );

    // bat
    config.writeEntry( "Dim", (int)screensaver_dim->isChecked() );
    config.writeEntry( "LightOff", (int)screensaver_lightoff->isChecked() );
    config.writeEntry( "LcdOffOnly", (int)LcdOffOnly->isChecked() );
    config.writeEntry( "Interval_Dim", interval_dim->value() );
    config.writeEntry( "Interval_LightOff", interval_lightoff->value() );
    config.writeEntry( "Interval", interval_suspend->value() );
    config.writeEntry( "Brightness",
    (brightness->maxValue()-brightness->value())*255/brightness->maxValue() );

    // ac
    config.writeEntry( "DimAC", (int)screensaver_dim_ac_3->isChecked() );
    config.writeEntry( "LightOffAC", (int)screensaver_lightoff_ac_3->isChecked() );
    config.writeEntry( "LcdOffOnlyAC", (int)LcdOffOnly_2_3->isChecked() );
    config.writeEntry( "Interval_DimAC", interval_dim_ac_3->value() );
    config.writeEntry( "Interval_LightOffAC", interval_lightoff_ac_3->value() );
    config.writeEntry( "IntervalAC", interval_suspend_ac_3->value() );
    config.writeEntry( "BrightnessAC",
    (brightness_ac_3->maxValue() - brightness_ac_3->value())*255/brightness_ac_3->maxValue() );

    // advanced
    config.setGroup( "APM" );
    config.writeEntry( "check_interval", warnintervalBox->value()*1000 );
    config.writeEntry( "power_verylow",  lowSpinBox->value() );
    config.writeEntry( "power_critical", criticalSpinBox->value() );
    QCopEnvelope e_warn("QPE/System", "reloadPowerWarnSettings()");


    // only make ipaq light sensor entries in config file if on an ipaq
    if ( ODevice::inst()->model() ==  Model_iPAQ_H31xx ||
         ODevice::inst()->model() ==  Model_iPAQ_H36xx ||
         ODevice::inst()->model() ==  Model_iPAQ_H37xx ||
         ODevice::inst()->model() ==  Model_iPAQ_H38xx ) {

        // ipaq sensor
        config.setGroup( "Ipaq_light_sensor" );

        config.writeEntry( "LightSensor", (int)auto_brightness->isChecked() );
        config.writeEntry( "LightSensorAC", (int)auto_brightness_ac_3->isChecked() );
        config.writeEntry( "Steps", LightStepSpin->value() );
        config.writeEntry( "MinValue", LightMinValueSlider->value() );
        config.writeEntry( "Shift", LightShiftSpin->value() );
    }

    config.write();

    QDialog::accept();
}

void LightSettings::applyBrightness()
{
    // if ac is attached, set directly that sliders setting, else the "on battery" sliders setting
    if ( PowerStatus::Online ) {
        int bright = (brightness_ac_3->maxValue() - brightness_ac_3->value())*255 / brightness_ac_3->maxValue();
        set_fl(bright);
    } else {
        int bright = (brightness->maxValue()-brightness->value())*255 / brightness->maxValue();
        set_fl(bright);
    }
}



void LightSettings::done(int r)
{
  QDialog::done(r);
  close ( );
}
