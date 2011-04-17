/*
                             This file is part of the Opie Project

              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

#include "obluetoothservices.h"

using namespace Opie::Bluez;


OBluetoothServices::ProfileDescriptor::ProfileDescriptor()
{
    m_idInt = 0;
    m_version = 0;
}

OBluetoothServices::ProfileDescriptor::ProfileDescriptor(const QString &id, int idInt, int version )
{
    m_id = id;
    m_idInt = idInt;
    m_version = version;
}

OBluetoothServices::ProfileDescriptor::ProfileDescriptor( const ProfileDescriptor& rem)
{
    (*this) = rem;
}

QString OBluetoothServices::ProfileDescriptor::id() const {
    return m_id;
}

void OBluetoothServices::ProfileDescriptor::setId( const QString& id ){
    m_id = id;
}

void OBluetoothServices::ProfileDescriptor::setId(int id ){
    m_idInt = id;
}

int OBluetoothServices::ProfileDescriptor::idInt() const
{
    return m_idInt;
}

int OBluetoothServices::ProfileDescriptor::version() const
{
    return m_version;
}

void OBluetoothServices::ProfileDescriptor::setVersion(int version)
{
    m_version = version;
}

OBluetoothServices::ProfileDescriptor& OBluetoothServices::ProfileDescriptor::operator=( const OBluetoothServices::ProfileDescriptor& prof)
{
    m_id = prof.m_id;
    m_idInt = prof.m_idInt;
    m_version = prof.m_version;
    return *this;
}

bool operator==(const OBluetoothServices::ProfileDescriptor& first,
                const OBluetoothServices::ProfileDescriptor& second )
{
    if( (first.id() == second.id() ) &&
        (first.version() == second.version() ) &&
        (first.idInt() == second.idInt() ) )
            return true;
    return false;
}

OBluetoothServices::ProtocolDescriptor::ProtocolDescriptor()
{
    m_number = 0;
    m_channel = 0;
}

OBluetoothServices::ProtocolDescriptor::ProtocolDescriptor(const QString& name,
                                                 int number,
                                                 int channel)
{
    m_name = name;
    m_number = number;
    m_channel = channel;
}

OBluetoothServices::ProtocolDescriptor::ProtocolDescriptor( const ProtocolDescriptor& ole )
{
    (*this) = ole;
}

OBluetoothServices::ProtocolDescriptor::~ProtocolDescriptor()
{
}

QString OBluetoothServices::ProtocolDescriptor::name() const
{
    return m_name;
}

void OBluetoothServices::ProtocolDescriptor::setName(const QString& name )
{
    m_name = name;
}

int OBluetoothServices::ProtocolDescriptor::id() const {
    return m_number;
}

void OBluetoothServices::ProtocolDescriptor::setId( int id )
{
    m_number = id;
}

int OBluetoothServices::ProtocolDescriptor::port() const
{
    return m_channel;
}

void OBluetoothServices::ProtocolDescriptor::setPort( int port )
{
    m_channel = port;
}

OBluetoothServices::ProtocolDescriptor &OBluetoothServices::ProtocolDescriptor::operator=( const OBluetoothServices::ProtocolDescriptor& desc )
{
    m_name = desc.m_name;
    m_channel = desc.m_channel;
    m_number = desc.m_number;
    return *this;
}

bool operator==( const OBluetoothServices::ProtocolDescriptor &first,
                 const OBluetoothServices::ProtocolDescriptor &second )
{
    if( ( first.name() == second.name() ) &&
        ( first.id() == second.id() ) &&
        ( first.port() == second.port() ) )
        return true;

    return false;
}

OBluetoothServices::OBluetoothServices()
{
    m_recHandle = 0;
}

OBluetoothServices::OBluetoothServices(const OBluetoothServices& service )
{
    (*this) = service;
}

OBluetoothServices::~OBluetoothServices()
{
}

OBluetoothServices &OBluetoothServices::operator=( const OBluetoothServices& ser)
{
    m_name =  ser.m_name;
    m_recHandle = ser.m_recHandle;
    m_classIds = ser.m_classIds;
    m_protocols = ser.m_protocols;
    m_profiles = ser.m_profiles;
    return *this;
}

bool operator==( const OBluetoothServices& one, const OBluetoothServices& two)
{
    if ( ( one.recHandle() == two.recHandle() ) &&
         ( one.serviceName() == two.serviceName() ) &&
         ( one.protocolDescriptorList() == two.protocolDescriptorList() ) &&
         ( one.profileDescriptor() == two.profileDescriptor() )
         /* ( one.classIdList() == two.classIdList() ) */ )
        return true;
    return false;
}

QString OBluetoothServices::serviceName() const
{
    return m_name;
}

void OBluetoothServices::setServiceName( const QString& service )
{
    m_name = service;
}

int OBluetoothServices::recHandle() const
{
    return m_recHandle;
}

void OBluetoothServices::setRecHandle( int handle)
{
    m_recHandle = handle;
}

QMap<int, QString> OBluetoothServices::classIdList() const
{
    return m_classIds;
}

void OBluetoothServices::insertClassId( int id, const QString& str )
{
    m_classIds.insert( id, str );
}

void OBluetoothServices::removeClassId(int id)
{
    m_classIds.remove( id );
}

void OBluetoothServices::clearClassId()
{
    m_classIds.clear();
}

void OBluetoothServices::insertProtocolDescriptor( const ProtocolDescriptor& prot)
{
    m_protocols.append( prot );
}

void OBluetoothServices::clearProtocolDescriptorList()
{
    m_protocols.clear();
}

void OBluetoothServices::removeProtocolDescriptor( const ProtocolDescriptor& prot)
{
    m_protocols.remove( prot );
}

OBluetoothServices::ProtocolDescriptor::ValueList OBluetoothServices::protocolDescriptorList() const
{
    return m_protocols;
}

void OBluetoothServices::insertProfileDescriptor( const ProfileDescriptor& prof)
{
    m_profiles.append( prof );
}

void OBluetoothServices::clearProfileDescriptorList()
{
    m_profiles.clear();
}

void OBluetoothServices::removeProfileDescriptor( const ProfileDescriptor& prof)
{
    m_profiles.remove(prof );
}

OBluetoothServices::ProfileDescriptor::ValueList OBluetoothServices::profileDescriptor() const
{
    return m_profiles;
}
