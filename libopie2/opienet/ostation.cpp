/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 by Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <opie2/ostation.h>
#include <opie2/odebug.h>


using namespace Opie::Core;

namespace Opie {
namespace Net {
/*======================================================================================
 * OStation
 *======================================================================================*/

OStation::OStation()
{
    odebug << "OStation::OStation()" << oendl;

    type = "<unknown>";
    macAddress = OMacAddress::unknown;
    ssid = "<unknown>";
    channel = 0;
    apAddress = OMacAddress::unknown;

}


OStation::~OStation()
{
    odebug << "OStation::~OStation()" << oendl;
}


void OStation::dump()
{
    odebug << "------- OStation::dump() ------------" << oendl;
    qDebug( "type: %s", (const char*) type );
    qDebug( "mac:  %s", (const char*) macAddress.toString() );
    qDebug( "ap:   %s", (const char*) apAddress.toString() );
    qDebug( "ip:   %s", (const char*) ipAddress.toString() );
}

}
}