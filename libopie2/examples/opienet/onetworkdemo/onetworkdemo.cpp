/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Team <opie@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

/* OPIE */
#include <opie2/onetwork.h>
#include <opie2/ostation.h>
#include <opie2/omanufacturerdb.h>
#include <opie2/odebug.h>

/* STD */
#include <unistd.h>

using namespace Opie::Net;

int main( int argc, char** argv )
{
    odebug << "OPIE Network Demo" << oendl;

    ONetwork* net = ONetwork::instance();

    ONetwork::InterfaceIterator it = net->iterator();

    while ( it.current() )
    {
        odebug << "DEMO: ONetwork contains Interface '" <<  it.current()->name() << "'" << oendl;
        odebug << "DEMO: Datalink code is '" << it.current()->dataLinkType() << "'" << oendl;
        odebug << "DEMO: MAC Address is '" <<  it.current()->macAddress().toString() << "'" << oendl;
        odebug << "DEMO: MAC Address is '" <<  it.current()->macAddress().toString(true) << "'" << oendl;
        odebug << "DEMO: MAC Manufacturer seems to be '" <<  it.current()->macAddress().manufacturer() << "'" << oendl;
        odebug << "DEMO: Manufacturertest1 = '" <<  OManufacturerDB::instance()->lookupExt( "08:00:87" ) << "'" << oendl;
        odebug << "DEMO: Manufacturertest2 = '" <<  OManufacturerDB::instance()->lookupExt( "E2:0C:0F" ) << "'" << oendl;
        odebug << "Demo: IPv4 Address is '" <<  it.current()->ipV4Address() << "'" << oendl;
        if ( it.current()->isWireless() )
        {
            OWirelessNetworkInterface* iface = static_cast<OWirelessNetworkInterface*>( it.current() );
            odebug << "DEMO: '" <<  iface->name() << "' seems to feature the wireless extensions." << oendl;
            odebug << "DEMO: Current SSID is '" <<  iface->SSID() << "'" << oendl;
            odebug << "DEMO: Antenna is tuned to '" << iface->frequency() << "', that is channel " << iface->channel() << "" << oendl;

            //if ( iface->mode() == OWirelessNetworkInterface::adhoc )
            //{
                //odebug << "DEMO: Associated AP has MAC Address '" <<  iface->associatedAP().toString() << "'" << oendl;
            //}

            /*

            // nickname
            odebug << "DEMO: Current NickName is '" <<  iface->nickName() << "'" << oendl;
            iface->setNickName( "MyNickName" );
            if ( iface->nickName() != "MyNickName" )
                odebug << "DEMO: Warning! Can't change nickname" << oendl;
            else
                odebug << "DEMO: Nickname change successful." << oendl;

            /*

            // operation mode
            odebug << "DEMO: Current OperationMode is '" <<  iface->mode() << "'" << oendl;
            iface->setMode( "adhoc" );
            if ( iface->mode() != "adhoc" )
                odebug << "DEMO: Warning! Can't change operation mode" << oendl;
            else
                odebug << "DEMO: Operation Mode change successful." << oendl;

            // RF channel
            odebug << "DEMO: Current Channel is '" << iface->channel() << "'" << oendl;
            iface->setChannel( 1 );
            if ( iface->channel() != 1 )
                odebug << "DEMO: Warning! Can't change RF channel" << oendl;
            else
                odebug << "DEMO: RF channel change successful." << oendl;

            iface->setMode( "managed" );

            */

            /*

            // network scan

            OStationList* stations = iface->scanNetwork();
            if ( stations )
            {
                odebug << "DEMO: # of stations around = " << stations->count() << "" << oendl;
                OStation* station;
                for ( station = stations->first(); station != 0; station = stations->next() )
                {
                    odebug << "DEMO: station dump following..." << oendl;
                    station->dump();
                }
            }

            else
            {
                odebug << "DEMO: Warning! Scan didn't work!" << oendl;
            }

            /*

            // first some wrong calls to check if this is working
            iface->setPrivate( "seppel", 10 );
            iface->setPrivate( "monitor", 0 );

            // now the real deal
            iface->setPrivate( "monitor", 2, 2, 3 );

            // trying to set hw address to 12:34:56:AB:CD:EF

            /*

            OMacAddress addr = OMacAddress::fromString( "12:34:56:AB:CD:EF" );
            iface->setUp( false );
            iface->setMacAddress( addr );
            iface->setUp( true );
            odebug << "DEMO: MAC Address now is '" <<  iface->macAddress().toString() << "'" << oendl;

            */

            // monitor test



            odebug << "DEMO: current interface mode is '" <<  iface->mode() << "'" << oendl;
            iface->setMode( "monitor" );
            odebug << "DEMO: current interface mode is '" <<  iface->mode() << "'" << oendl;

            sleep( 1 );

            iface->setChannel( 1 );
            iface->setMode( "managed" );

            //sleep( 1 );
            odebug << "DEMO: current interface mode is '" <<  iface->mode() << "'" << oendl;

            /*iface->setMode( "adhoc" );
            sleep( 1 );
            odebug << "DEMO: current interface mode is '" <<  iface->mode() << "'" << oendl;
            iface->setMode( "managed" );
            sleep( 1 );
            odebug << "DEMO: current interface mode is '" <<  iface->mode() << "'" << oendl;
            iface->setMode( "master" );
            sleep( 1 );
            odebug << "DEMO: current interface mode is '" <<  iface->mode() << "'" << oendl; */

        }
        ++it;
    }

    return 0;

}
