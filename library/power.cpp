/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#include "power.h"

#ifdef QT_QWS_SL5XXX
#include "custom.h"
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef QT_QWS_IPAQ_NO_APM
#include <linux/h3600_ts.h>
#endif

PowerStatusManager *PowerStatusManager::powerManager = 0;
PowerStatus *PowerStatusManager::ps = 0;

static bool haveProcApm = false;

PowerStatusManager::PowerStatusManager()
{
    powerManager = this;
    ps = new PowerStatus;
    FILE *f = fopen("/proc/apm", "r");
    if ( f ) {
	fclose(f);
	haveProcApm = TRUE;
    }
}

const PowerStatus &PowerStatusManager::readStatus()
{
    if ( !powerManager )
	(void)new PowerStatusManager;

    powerManager->getStatus();

    return *ps;
}

// Standard /proc/apm reader
bool PowerStatusManager::getProcApmStatus( int &ac, int &bs, int &bf, int &pc, int &sec )
{
    bool ok = false;

    ac = 0xff;
    bs = 0xff;
    bf = 0xff;
    pc = -1;
    sec = -1;

    FILE *f = fopen("/proc/apm", "r");
    if ( f ) {
	//I 1.13 1.2 0x02 0x00 0xff 0xff 49% 147 sec
	char u;
	fscanf(f, "%*[^ ] %*d.%*d 0x%*x 0x%x 0x%x 0x%x %d%% %i %c",
		&ac, &bs, &bf, &pc, &sec, &u);
	fclose(f);
	switch ( u ) {
	    case 'm': sec *= 60;
	    case 's': break; // ok
	    default: sec = -1; // unknown
	}

	// extract data
	switch ( bs ) {
	    case 0x00:
		ps->bs = PowerStatus::High;
		break;
	    case 0x01:
		ps->bs = PowerStatus::Low;
		break;
	    case 0x7f:
		ps->bs = PowerStatus::VeryLow;
		break;
	    case 0x02:
		ps->bs = PowerStatus::Critical;
		break;
	    case 0x03:
		ps->bs = PowerStatus::Charging;
		break;
	    case 0x04:
	    case 0xff: // 0xff is Unknown but we map to NotPresent
	    default:
		ps->bs = PowerStatus::NotPresent;
		break;
	}

	switch ( ac ) {
	    case 0x00:
		ps->ac = PowerStatus::Offline;
		break;
	    case 0x01:
		ps->ac = PowerStatus::Online;
		break;
	    case 0x02:
		ps->ac = PowerStatus::Backup;
		break;
	}

	if ( pc > 100 ) pc = 100;
	if ( pc < 0 ) pc = 0;

	ps->percentRemain = pc;
	ps->secsRemain = sec;

	ok = true;
    }

    return ok;
}

void PowerStatusManager::getStatus()
{
    bool usedApm = FALSE;

    ps->percentAccurate = TRUE;

    // Some iPAQ kernel builds don't have APM. If this is not the case we
    // save ourselves an ioctl by testing if /proc/apm exists in the
    // constructor and we use /proc/apm instead
    int ac, bs, bf, pc, sec;
    if ( haveProcApm )
	usedApm = getProcApmStatus( ac, bs, bf, pc, sec );

    if ( !usedApm ) {
#ifdef QT_QWS_IPAQ_NO_APM
	int fd;
	int err;
	struct bat_dev batt_info;

	memset(&batt_info, 0, sizeof(batt_info));

	fd = ::open("/dev/ts",O_RDONLY);
	if( fd < 0 )
	    return;

	ioctl(fd, GET_BATTERY_STATUS, &batt_info);
	ac_status = batt_info.ac_status;
	ps->percentRemain = ( 425 * batt_info.batt1_voltage ) / 1000 - 298; // from h3600_ts.c
	ps->secsRemain = -1; // seconds is bogus on iPAQ
	::close (fd);
#else
	ps->percentRemain = 100;
	ps->secsRemain = -1;
	ps->percentAccurate = FALSE;
#endif
    }
}
