/*
                             This file is part of the Opie Project

                             (C) 2003 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include <opie2/onetutils.h>
#include <opie2/onetwork.h>
#include <opie2/omanufacturerdb.h>

#include <net/if.h>
#include <assert.h>
#include <stdio.h>

namespace Opie {
namespace Net  {

/*======================================================================================
 * OMacAddress
 *======================================================================================*/

// static initializer for broadcast and unknown MAC Adresses
const unsigned char __broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const OMacAddress& OMacAddress::broadcast = OMacAddress( __broadcast );
const unsigned char __unknown[6] = { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 };
const OMacAddress& OMacAddress::unknown = OMacAddress( __unknown );

//TODO: Incorporate Ethernet Manufacturer database here! (inline or so)

OMacAddress::OMacAddress()
{
    memcpy( _bytes, __unknown, 6 );
}


OMacAddress::OMacAddress( unsigned char* p )
{
    memcpy( _bytes, p, 6 );
}


OMacAddress::OMacAddress( const unsigned char* p )
{
    memcpy( _bytes, p, 6 );
}


OMacAddress::OMacAddress( struct ifreq& ifr )
{
    memcpy( _bytes, ifr.ifr_hwaddr.sa_data, 6 );
}


OMacAddress::~OMacAddress()
{
}


//#ifdef QT_NO_DEBUG
//inline
//#endif
const unsigned char* OMacAddress::native() const
{
    return (const unsigned char*) &_bytes;
}


OMacAddress OMacAddress::fromString( const QString& str )
{
    QString addr( str );
    unsigned char buf[6];
    bool ok = true;
    int index = 14;
    for ( int i = 5; i >= 0; --i )
    {
        buf[i] = addr.right( 2 ).toUShort( &ok, 16 );
        if ( !ok ) return OMacAddress::unknown;
        addr.truncate( index );
        index -= 3;
    }
    return (const unsigned char*) &buf;
}


QString OMacAddress::toString( bool substitute ) const
{
    QString manu;
    manu.sprintf( "%.2X:%.2X:%.2X", _bytes[0]&0xff, _bytes[1]&0xff, _bytes[2]&0xff );
    QString serial;
    serial.sprintf( ":%.2X:%.2X:%.2X", _bytes[3]&0xff, _bytes[4]&0xff, _bytes[5]&0xff );
    if ( !substitute ) return manu+serial;
    // fallback - if no vendor is found, just use the number
    QString textmanu = OManufacturerDB::instance()->lookup( manu );
    return textmanu.isNull() ? manu+serial : textmanu+serial;
}


QString OMacAddress::manufacturer() const
{
    return OManufacturerDB::instance()->lookupExt( toString() );
}


bool operator==( const OMacAddress &m1, const OMacAddress &m2 )
{
    return memcmp( &m1._bytes, &m2._bytes, 6 ) == 0;
}


/*======================================================================================
 * OHostAddress
 *======================================================================================*/


/*======================================================================================
 * OPrivateIOCTL
 *======================================================================================*/

OPrivateIOCTL::OPrivateIOCTL( QObject* parent, const char* name, int cmd, int getargs, int setargs )
              :QObject( parent, name ), _ioctl( cmd ), _getargs( getargs ), _setargs( setargs )
{
}


OPrivateIOCTL::~OPrivateIOCTL()
{
}


int OPrivateIOCTL::numberGetArgs() const
{
    return _getargs & IW_PRIV_SIZE_MASK;
}


int OPrivateIOCTL::typeGetArgs() const
{
    return _getargs & IW_PRIV_TYPE_MASK >> 12;
}


int OPrivateIOCTL::numberSetArgs() const
{
    return _setargs & IW_PRIV_SIZE_MASK;
}


int OPrivateIOCTL::typeSetArgs() const
{
    return _setargs & IW_PRIV_TYPE_MASK >> 12;
}


void OPrivateIOCTL::invoke() const
{
    ( (OWirelessNetworkInterface*) parent() )->wioctl( _ioctl );
}


void OPrivateIOCTL::setParameter( int num, u_int32_t value )
{
    u_int32_t* arglist = (u_int32_t*) &( (OWirelessNetworkInterface*) parent() )->_iwr.u.name;
    arglist[num] = value;
}



namespace Private {
/*======================================================================================
 * assorted functions
 *======================================================================================*/

void dumpBytes( const unsigned char* data, int num )
{
    printf( "Dumping %d bytes @ %0x", num, data );
    printf( "-------------------------------------------\n" );

    for ( int i = 0; i < num; ++i )
    {
        printf( "%02x ", data[i] );
        if ( !((i+1) % 32) ) printf( "\n" );
    }
    printf( "\n\n" );
}


int stringToMode( const QString& mode )
{
    if ( mode == "auto" )            return IW_MODE_AUTO;
    else if ( mode == "adhoc" )      return IW_MODE_ADHOC;
    else if ( mode == "managed" )    return IW_MODE_INFRA;
    else if ( mode == "master" )     return IW_MODE_MASTER;
    else if ( mode == "repeater" )   return IW_MODE_REPEAT;
    else if ( mode == "secondary" )  return IW_MODE_SECOND;
    else if ( mode == "monitor" )    return IW_MODE_MONITOR;
    else assert( 0 );
}


QString modeToString( int mode )
{
    switch ( mode )
    {
        case IW_MODE_AUTO:          return "auto";
        case IW_MODE_ADHOC:         return "adhoc";
        case IW_MODE_INFRA:         return "managed";
        case IW_MODE_MASTER:        return "master";
        case IW_MODE_REPEAT:        return "repeater";
        case IW_MODE_SECOND:        return "second";
        case IW_MODE_MONITOR:       return "monitor";
        default: assert( 0 );
    }
}
}
}
}
