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
#include "settings.h"

#include <qpe/global.h>
#include <qpe/fontmanager.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
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
#include <qspinbox.h>
#include <qlistbox.h>
#include <qdir.h>
#if QT_VERSION >= 300
#include <qstylefactory.h>
#endif

#include <opie/odevice.h>


LightSettings::LightSettings( QWidget* parent,  const char* name, WFlags fl )
    : LightSettingsBase( parent, name, TRUE, fl )
{
    // Not supported
    auto_brightness->hide();

    Config config( "qpe" );

    config.setGroup( "Screensaver" );

    int interval;
    interval = config.readNumEntry( "Interval_Dim", 20 );
    interval_dim->setValue( interval );
    interval = config.readNumEntry( "Interval_LightOff", 30 );
    interval_lightoff->setValue( interval );
    interval = config.readNumEntry( "Interval", 60 );
    if ( interval > 3600 ) interval /= 1000; // compatibility (was millisecs)
    interval_suspend->setValue( interval );

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

    connect(brightness, SIGNAL(valueChanged(int)), this, SLOT(applyBrightness()));
}

LightSettings::~LightSettings()
{
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

    int i_dim =      (screensaver_dim->isChecked() ? interval_dim->value() : 0);
    int i_lightoff = (screensaver_lightoff->isChecked() ? interval_lightoff->value() : 0);
    int i_suspend =  interval_suspend->value();
    QCopEnvelope e("QPE/System", "setScreenSaverIntervals(int,int,int)" );
    e << i_dim << i_lightoff << i_suspend;

    Config config( "qpe" );
    config.setGroup( "Screensaver" );
    config.writeEntry( "Dim", (int)screensaver_dim->isChecked() );
    config.writeEntry( "LightOff", (int)screensaver_lightoff->isChecked() );
    config.writeEntry( "LcdOffOnly", (int)LcdOffOnly->isChecked() );
    config.writeEntry( "Interval_Dim", interval_dim->value() );
    config.writeEntry( "Interval_LightOff", interval_lightoff->value() );
    config.writeEntry( "Interval", interval_suspend->value() );
    config.writeEntry( "Brightness",
    (brightness->maxValue()-brightness->value())*255/brightness->maxValue() );
    config.write();

    QDialog::accept();
}

void LightSettings::applyBrightness()
{
    int bright = (brightness->maxValue()-brightness->value())*255
      / brightness->maxValue();
    set_fl(bright);
}


void LightSettings::done(int r)
{
  QDialog::done(r);
  close ( );
}
