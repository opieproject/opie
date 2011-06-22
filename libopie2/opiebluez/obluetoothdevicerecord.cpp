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

#include <opie2/obluetoothdevicerecord.h>

using namespace Opie::Bluez;

DeviceRecord::DeviceRecord()
{
}

DeviceRecord::DeviceRecord( const QString &mac, const QString &name )
    : m_mac(mac), m_name(name)
{
}

void DeviceRecord::setMac( const QString &mac )
{
    m_mac = mac;
}

void DeviceRecord::setName( const QString &name )
{
    m_name = name;
}

QString DeviceRecord::mac() const
{
    return m_mac;
}

QString DeviceRecord::name() const
{
    return m_name;
}

DeviceRecord& DeviceRecord::operator=( const DeviceRecord& other )
{
    m_mac = other.m_mac;
    m_name = other.m_name;
    return *this;
}

bool DeviceRecord::operator==( const DeviceRecord &other ) const
{
    return ( m_mac == other.mac() && m_name == other.name() );
}

