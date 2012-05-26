/*
                             This file is part of the Opie Project

                             Copyright (C)2002, 2003, 2004 The Opie Team <opie-devel@lists.sourceforge.net>
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

#include "odevice_genuineintel.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie2/okeyfilter.h>
#include <opie2/oresource.h>

#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif

using namespace Opie::Core;
using namespace Opie::Core::Internal;

void GenuineIntel::init(const QString& model)
{
    d->m_vendorstr = "Intel";
    d->m_vendor = Vendor_GenuineIntel;

    QStringList SL = QStringList::split( " ", model );

    d->m_model = Model_GenuineIntel;
    d->m_rotation = Rot0;
}

void GenuineIntel::initButtons()
{
    if ( d->m_buttons )
        return;

    if ( isQWS( ) ) {
        addPreHandler(this);
    }

    ODevice::initButtons();
}

bool GenuineIntel::filter ( int /*unicode*/, int , int , bool , bool )
{
    return false;
}

bool GenuineIntel::setDisplayBrightness ( int )
{
    return false;
}

int GenuineIntel::displayBrightnessResolution() const
{
    return 1; // perhaps to avoid division by zero
}


bool GenuineIntel::hasLightSensor() const
{
    return false;
}

int GenuineIntel::readLightSensor()
{
    return 0;
}

int GenuineIntel::lightSensorResolution() const
{
    return 1; // see above
}
