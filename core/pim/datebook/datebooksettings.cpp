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

#include "datebooksettings.h"

#include <qpe/qpeapplication.h>

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>

DateBookSettings::DateBookSettings( bool whichClock, QWidget *parent,
                                    const char *name, bool modal, WFlags fl )
    : DateBookSettingsBase( parent, name, modal, fl ),
      ampm( whichClock )
{
    init();
    QObject::connect( qApp, SIGNAL( clockChanged( bool ) ),
                      this, SLOT( slotChangeClock( bool ) ) );
}

DateBookSettings::~DateBookSettings()
{
}

void DateBookSettings::setStartTime( int newStartViewTime )
{
    if ( ampm ) {
	if ( newStartViewTime >= 12 ) {
	    newStartViewTime %= 12;
	    if ( newStartViewTime == 0 )
		newStartViewTime = 12;
	    spinStart->setSuffix( tr(":00 PM") );
	}
	else if ( newStartViewTime == 0 ) {
	    newStartViewTime = 12;
	    spinStart->setSuffix( tr(":00 AM") );
	}
	oldtime = newStartViewTime;
    }
    spinStart->setValue( newStartViewTime );
}

int DateBookSettings::startTime() const
{
    int returnMe = spinStart->value();
    if ( ampm ) {
	if ( returnMe != 12 && spinStart->suffix().contains(tr("PM"), FALSE) )
	    returnMe += 12;
	else if (returnMe == 12 && spinStart->suffix().contains(tr("AM"), TRUE))
	    returnMe = 0;
    }
    return returnMe;
}


void DateBookSettings::setAlarmPreset( bool bAlarm, int presetTime )
{
    chkAlarmPreset->setChecked( bAlarm );
    if ( presetTime >=5 )
	spinPreset->setValue( presetTime );
}

bool DateBookSettings::alarmPreset() const
{
    return chkAlarmPreset->isChecked();
}

int DateBookSettings::presetTime() const
{
    return spinPreset->value();
}


void DateBookSettings::slot12Hour( int i )
{
    if ( ampm ) {
	if ( spinStart->suffix().contains( tr("AM"), FALSE ) ) {
	    if ( oldtime == 12 && i == 11 || oldtime == 11 && i == 12 )
		spinStart->setSuffix( tr(":00 PM") );
	} else {
	    if ( oldtime == 12 && i == 11 || oldtime == 11 && i == 12 )
		spinStart->setSuffix( tr(":00 AM") );
	}
	oldtime = i;
    }
}

void DateBookSettings::init()
{
    if ( ampm ) {
	spinStart->setMinValue( 1 );
	spinStart->setMaxValue( 12 );
	spinStart->setValue( 12 );
	spinStart->setSuffix( tr(":00 AM") );
	oldtime = 12;
    } else {
	spinStart->setMinValue( 0 );
	spinStart->setMaxValue( 23 );
	spinStart->setSuffix( tr(":00") );
    }
}

void DateBookSettings::slotChangeClock( bool whichClock )
{
    int saveMe;
    saveMe = spinStart->value();
    if ( ampm && spinStart->suffix().contains( tr("AM"), FALSE ) ) {
	if ( saveMe == 12 )
	    saveMe = 0;
    } else if ( ampm && spinStart->suffix().contains( tr("PM"), FALSE )  ) {
	if ( saveMe != 12 )
	    saveMe += 12;
    }
    ampm = whichClock;
    init();
    setStartTime( saveMe );
}
