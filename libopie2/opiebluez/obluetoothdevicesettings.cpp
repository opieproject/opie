/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
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

#include <obluetoothdevicesettings.h>

#include <qstringlist.h>
#include <qpe/config.h>

using namespace Opie::Bluez;

DeviceSettings::DeviceSettings()
{
}

DeviceSettings::~DeviceSettings()
{
}

DeviceRecord::ValueList DeviceSettings::load()
{
    DeviceRecord::ValueList list;

    Config conf("bluetooth-devices");
    QStringList grps = conf.allGroups();

    if (!grps.isEmpty() ) {
        QStringList::Iterator it;
        for (it = grps.begin(); it != grps.end(); ++it ) {
            conf.setGroup(*it);
            QString name = conf.readEntry("name", "Unknown");
            DeviceRecord currentDevice( (*it), name  );
            list.append( currentDevice );
        }
    }
    return list;
}

void DeviceSettings::save( const DeviceRecord::ValueList& list)
{
    Config conf("bluetooth-devices");

    // Get the current list of groups
    QStringList grps = conf.allGroups();
    // Remove all current devices from list
    DeviceRecord::ValueList::ConstIterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        grps.remove( (*it).mac() );
    }
    // If there are any remaining, they've been deleted, so delete them from the file
    if (!grps.isEmpty() ) {
        QStringList::Iterator it2;
        for (it2 = grps.begin(); it2 != grps.end(); ++it2 ) {
            conf.removeGroup(*it2);
        }
    }
    // Write out the new values
    for ( it = list.begin(); it != list.end(); ++it ) {
        conf.setGroup( (*it).mac() );
        conf.writeEntry( "name", (*it).name() );
    }
}
