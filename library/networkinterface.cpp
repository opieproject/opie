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

#include "networkinterface.h"
#include "network.h"
#include "config.h"
#include <stdio.h>

QString NetworkInterface::device( Config& cfg ) const
{
    return cfg.readEntry("Device");
}

bool NetworkInterface::isActive( Config& cfg ) const
{    
    QString dev = device(cfg);
    if ( dev.isEmpty() )
	return FALSE;
    QString dev0 = dev+'0';

    FILE* f;
    f = fopen("/proc/net/dev", "r");
    if ( f ) {
	char line[1024];
	char devname[80];
	while ( fgets( line, 1024, f ) ) {
	    if ( sscanf(line," %[^:]:", devname)==1 )
	    {
		if ( devname == dev || devname == dev0 ) {
		    fclose(f);
		    Network::writeProxySettings( cfg );
		    return TRUE;
		}
	    }
	}
	fclose(f);
    }
    return FALSE;
}

QString NetworkInterface::cardType( Config& cfg ) const
{
    return cfg.readEntry("CardType");
}

bool NetworkInterface::isAvailable( Config& cfg ) const
{
    QString ct = cardType(cfg);
    if ( ct.isEmpty() )
	return FALSE;

    FILE* f = fopen("/var/run/stab", "r");
    if (!f) f = fopen("/var/state/pcmcia/stab", "r");
    if (!f) f = fopen("/var/lib/pcmcia/stab", "r");

    if ( f ) {
	char line[1024];
	char devtype[80];
	while ( fgets( line, 1024, f ) ) {
	    if ( sscanf(line,"%*d %s %*s", devtype )==1 )
	    {
		if ( ct == devtype ) {
		    fclose(f);
		    return TRUE;
		}
	    }
	}
	fclose(f);
    }

    return FALSE;
}

bool NetworkInterface::start( Config& cfg, const QString& /*password*/ )
{
    return start(cfg);
}

bool NetworkInterface::needPassword( Config& ) const
{
    return FALSE;
}

QWidget* NetworkInterface::addStateWidget( QWidget*, Config& ) const
{
    return 0;
}
