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

#include "power.h"

#ifdef QT_QWS_CUSTOM
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

	if ( pc > 100 )
	    pc = -1;

	ps->percentRemain = pc;
	ps->secsRemain = sec;

	ok = true;
    }

    return ok;
}

#ifdef QT_QWS_CUSTOM

void PowerStatusManager::getStatus()
{
    int ac, bs, bf, pc, sec;
    ps->percentAccurate = TRUE; // not for long...

    if ( haveProcApm && getProcApmStatus( ac, bs, bf, pc, sec ) ) {
	// special case
	if ( bs == 0x7f )
	    ps->bs = PowerStatus::VeryLow;
	pc = -1; // fake percentage
	if ( pc < 0 ) {
	    switch ( bs ) {
		case 0x00: ps->percentRemain = 100; break; // High
		case 0x01: ps->percentRemain = 30; break; // Low
		case 0x7f: ps->percentRemain = 10; break; // Very Low
		case 0x02: ps->percentRemain = 5; break; // Critical
		case 0x03: ps->percentRemain = -1; break; // Charging
	    }
	    ps->percentAccurate = FALSE;
	}
    }

    char *device = "/dev/apm_bios";
    int fd = ::open (device, O_WRONLY);
    if ( fd >= 0 ) {
	int bbat_status = ioctl( fd, APM_IOC_BATTERY_BACK_CHK, 0 );
	switch ( bbat_status ) {
	    case 0x00:
		ps->bbs = PowerStatus::High;
		break;
	    case 0x01:
		ps->bbs = PowerStatus::Low;
		break;
	    case 0x7f:
		ps->bbs = PowerStatus::VeryLow;
		break;
	    case 0x02:
		ps->bbs = PowerStatus::Critical;
		break;
	    case 0x03:
		ps->bbs = PowerStatus::Charging;
		break;
	    case 0x04:
		ps->bbs = PowerStatus::NotPresent;
		break;
	}
	::close(fd);
    }
}

#else

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

#endif

