#include <opie2/onetwork.h>

int main( int argc, char** argv )
{
    qDebug( "OPIE Network Demo" );

    ONetwork* net = ONetwork::instance();

    ONetwork::InterfaceIterator it = net->iterator();

    while ( it.current() )
    {
        qDebug( "DEMO: ONetwork contains Interface '%s'", (const char*) it.current()->name() );
        qDebug( "DEMO: MAC Address is '%s'", (const char*) it.current()->macAddress().toString() );
        qDebug( "DEMO: MAC Address is '%s'", (const char*) it.current()->macAddress().toString(true) );
        qDebug( "Demo: IPv4 Address is '%s'", (const char*) it.current()->ipV4Address() );
        if ( it.current()->isWireless() )
        {
            OWirelessNetworkInterface* iface = static_cast<OWirelessNetworkInterface*>( it.current() );
            qDebug( "DEMO: '%s' seems to feature the wireless extensions.", (const char*) iface->name() );
            qDebug( "DEMO: Current SSID is '%s'", (const char*) iface->SSID() );
            qDebug( "DEMO: Antenna is tuned to '%f', that is channel %d", iface->frequency(), iface->channel() );

            //if ( iface->mode() == OWirelessNetworkInterface::adhoc )
            //{
                qDebug( "DEMO: Associated AP has MAC Address '%s'", (const char*) iface->associatedAP() );
            //}

            // nickname
            qDebug( "DEMO: Current NickName is '%s'", (const char*) iface->nickName() );
            iface->setNickName( "MyNickName" );
            if ( iface->nickName() != "MyNickName" )
                qDebug( "DEMO: Warning! Can't change nickname" );
            else
                qDebug( "DEMO: Nickname change successful." );

            // operation mode
            qDebug( "DEMO: Current OperationMode is '%s'", (const char*) iface->mode() );
            iface->setMode( "adhoc" );
            if ( iface->mode() != "adhoc" )
                qDebug( "DEMO: Warning! Can't change operation mode" );
            else
                qDebug( "DEMO: Operation Mode change successful." );

            iface->setMode( "managed" );

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
            qDebug( "DEMO: MAC Address now is '%s'", (const char*) iface->macAddress().toString() );

            */


        }
        ++it;
    }

    return 0;

}
