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

#ifndef POWER_H
#define POWER_H

#include <qobject.h>

class PowerStatus
{
public:
    PowerStatus() {
	ac = Offline;
	bs = NotPresent;
	bbs = NotPresent;
	percentRemain = -1;
	secsRemain = -1;
    }

    enum ACStatus { Offline, Online, Backup };
    ACStatus acStatus() const { return ac; }

    enum BatteryStatus { High=0x01, Low=0x02, VeryLow=0x04, Critical=0x08,
			 Charging=0x10, NotPresent=0x20 };
    BatteryStatus batteryStatus() const { return bs; }
    BatteryStatus backupBatteryStatus() const { return bbs; }

    bool batteryPercentAccurate() const { return percentAccurate; }
    int batteryPercentRemaining() const { return percentRemain; }
    int batteryTimeRemaining() const { return secsRemain; }

    bool operator!=( const PowerStatus &ps ) {
	return  (ps.ac != ac) || (ps.bs != bs) || (ps.bbs != bbs) ||
		(ps.percentRemain != percentRemain) ||
		(ps.secsRemain != secsRemain );
    }

private:
    ACStatus ac;
    BatteryStatus bs;
    BatteryStatus bbs;
    int percentRemain;
    int secsRemain;
    bool percentAccurate;

    friend class PowerStatusManager;
};


class PowerStatusManager
{
public:
    PowerStatusManager();

    static const PowerStatus &readStatus();

protected:
    bool getProcApmStatus( int &ac, int &bs, int &bf, int &pc, int &sec );
    void getStatus();

private:
    static PowerStatus *ps;
    static PowerStatusManager *powerManager;
};


#endif

