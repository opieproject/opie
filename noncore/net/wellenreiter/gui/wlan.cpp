/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "wlan.h"
#include "cardconfig.h"

// Qt
#include <qstring.h>

// Qtopia
#ifdef QWS
#include <opie/odevice.h>
using namespace Opie;
#endif

WLAN::WLAN( const QString& interface )
{
    _configuration = new CardConfig( interface );
}

WLAN::WLAN( const CardConfig* configuration )
{
    _configuration = configuration;

}

WLAN::~WLAN()
{
    delete _configuration;

}
  
void WLAN::setMonitorMode( bool enabled )
{

    /*
    
    if ( _configuration->system() == System_OpenZaurus && _configuration->type() == CardConfig::Prism )
    {
    }
    
    */

}

