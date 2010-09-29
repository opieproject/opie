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

#if defined(QT_QWS_SL5XXX) || defined(QT_QWS_RAMSES)
#include "custom.h"
#endif

#include <qmap.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cmath>

#ifdef QT_QWS_IPAQ_NO_APM
#include <linux/h3600_ts.h>
#endif

#include <sysfs/libsysfs.h>


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

// Standard sysfs reader
bool PowerStatusManager::getSysFsStatus()
{
    struct sysfs_class *cls = sysfs_open_class( "power_supply" );
    if( !cls )
        return false;

    ps->bs = PowerStatus::NotPresent;
    ps->secsRemain = -1;

    QStringList props;
    props += "type";
    props += "charge_full";
    props += "charge_full_design";
    props += "charge_now";
    props += "energy_full";
    props += "energy_now";
    props += "current_now";
    props += "power_now";
    props += "status";
    props += "online";

    bool ok = false;
    struct dlist *cdevs = sysfs_get_class_devices( cls );
    if (cdevs != NULL) {
        struct sysfs_class_device *cdev = NULL;
        dlist_for_each_data(cdevs, cdev, struct sysfs_class_device) {

            struct dlist *attrlist = sysfs_get_classdev_attributes( cdev );
            if (attrlist != NULL) {
                QMap<QString,QString> propmap;
                struct sysfs_attribute *attr = NULL;
                dlist_for_each_data(attrlist, attr, struct sysfs_attribute) {
                    if( props.findIndex( attr->name ) > -1 ) {
                        if( sysfs_read_attribute( attr ) == 0 )
                            propmap.insert( attr->name, QString( attr->value ).stripWhiteSpace() );
                    }
                }

                ok = true;
                if( propmap["type"] == "Battery" ) {
                    int charge_full = propmap["charge_full"].toInt();
                    int charge_now = 0, current_now = 0;

                    if( charge_full == 0 ) {
                        charge_full = propmap["charge_full_design"].toInt();
                    }

                    if( charge_full == 0 ) {
                        charge_full = propmap["energy_full"].toInt();
                        charge_now = propmap["energy_now"].toInt();
                        current_now = propmap["power_now"].toInt();
                    }
                    else {
                        charge_now = propmap["charge_now"].toInt();
                        current_now = propmap["current_now"].toInt();
                    }

                    int pc;

                    if( current_now > 0 )
                        ps->secsRemain = round( ( (double)charge_now / current_now ) * 3600 );

                    if( charge_now == charge_full ) {
                        ps->bs = PowerStatus::High;
                        pc = 100;
                    }
                    else {
                        pc = round( ( (double)charge_now / charge_full ) * 100 );
                        if ( pc > 100 ) pc = 100;
                        if ( pc < 0 ) pc = 0;

                        QString status = propmap["status"];
                        if( status == "Charging" ) {
                            ps->bs = PowerStatus::Charging;
                        }
                        else {
                            if( pc > 50 )
                                ps->bs = PowerStatus::High;
                            else if( pc > 35 )
                                ps->bs = PowerStatus::Low;
                            else if( pc > 20 )
                                ps->bs = PowerStatus::VeryLow;
                            else
                                ps->bs = PowerStatus::Critical;
                        }
                    }

                    ps->percentRemain = pc;
                }
                else if( propmap["type"] == "Mains" ) {
                    int ac = propmap["online"].toInt();
                    if( ac )
                        ps->ac = PowerStatus::Online;
                    else
                        ps->ac = PowerStatus::Offline;
                }
            }
        }
    }

    sysfs_close_class( cls );

    return ok;
}

// Standard /proc/apm reader
bool PowerStatusManager::getProcApmStatus()
{
    bool ok = false;

    int ac, bs, bf, pc, sec;
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
    bool gotStatus = FALSE;

    ps->percentAccurate = TRUE;

    gotStatus = getSysFsStatus();

    // Some iPAQ kernel builds don't have APM. If this is not the case we
    // save ourselves an ioctl by testing if /proc/apm exists in the
    // constructor and we use /proc/apm instead
    if ( (!gotStatus) && haveProcApm )
        gotStatus = getProcApmStatus();

    if ( !gotStatus ) {
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
