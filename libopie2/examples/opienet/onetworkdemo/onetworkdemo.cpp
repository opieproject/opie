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
        qDebug( "Demo: IPv4 Address is '%s'", (const char*) it.current()->ipV4Address() );
        if ( it.current()->isWireless() )
        {
            OWirelessNetworkInterface* iface = static_cast<OWirelessNetworkInterface*>( it.current() );
            qDebug( "DEMO: '%s' seems to feature the wireless extensions.", (const char*) iface->name() );
            qDebug( "DEMO: Current SSID is '%s'", (const char*) iface->SSID() );
            qDebug( "DEMO: Current NickName is '%s'", (const char*) iface->nickName() );
            qDebug( "DEMO: Antenna is tuned to '%f', that is channel %d", iface->frequency(), iface->channel() );

            //if ( iface->mode() == OWirelessNetworkInterface::adhoc )
            //{
                qDebug( "DEMO: Associated AP has MAC Address '%s'", (const char*) iface->associatedAP() );
            //}

        }
        ++it;
    }

    return 0;

}
