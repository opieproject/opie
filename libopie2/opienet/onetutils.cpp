/*
                             This file is part of the Opie Project

                             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
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

#include <net/if.h>

#include <cstdio>
using namespace std;

/*======================================================================================
 * OMacAddress
 *======================================================================================*/

// static initializer for broadcast and unknown MAC Adresses
const unsigned char __broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const OMacAddress& OMacAddress::broadcast = OMacAddress( __broadcast );
const unsigned char __unknown[6] = { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 };
const OMacAddress& OMacAddress::unknown = OMacAddress( __unknown );


//TODO: Incorporate Ethernet Manufacturer database here!

OMacAddress::OMacAddress( unsigned char* p )
{
    memcpy( _bytes, p, 6 ); // D'OH! memcpy in my sources... eeek...
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


QString OMacAddress::toString() const
{
    QString s;
    s.sprintf( "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
      _bytes[0]&0xff, _bytes[1]&0xff, _bytes[2]&0xff,
      _bytes[3]&0xff, _bytes[4]&0xff, _bytes[5]&0xff );
    return s;
}


bool operator==( const OMacAddress &m1, const OMacAddress &m2 )
{
    return memcmp( &m1._bytes, &m2._bytes, 6 ) == 0;
}

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
