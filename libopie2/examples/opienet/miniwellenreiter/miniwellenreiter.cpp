#include <qdict.h>
#include <qsocketnotifier.h>
#include <qstring.h>
#include <opie2/onetwork.h>
#include <qapplication.h>
#include <opie2/opcap.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//======================== Station help class ===============================

class Station
{
  public:
    Station( QString t, int c, bool w ) : type(t), channel(c), wep(w), beacons(1) {};
    ~Station() {};

    QString type;
    int channel;
    bool wep;
    int beacons;
};

QDict<Station> stations;

//======================== Application class ===============================

class Wellenreiter : public QApplication
{
Q_OBJECT
public:
  Wellenreiter( int argc, char** argv ) : QApplication( argc, argv )
  {

    ONetwork* net = ONetwork::instance();

    if ( argc < 3 )
    {
        printf( "Usage: ./%s <interface> <driver> <interval>\n", argv[0] );
        printf( "\n" );
        printf( "Valid wireless interfaces (detected) are:\n" );

        ONetwork::InterfaceIterator it = net->iterator();
        while ( it.current() )
        {
            if ( it.current()->isWireless() )
            {
                printf( " - '%s' (MAC=%s) (IPv4=%s)\n", (const char*) it.current()->name(),
                                                    (const char*) it.current()->macAddress().toString(),
                                                    (const char*) it.current()->ipV4Address() );
            }
            ++it;
        }
    exit( -1 );
    }

    printf( "****************************************************\n" );
    printf( "* Wellenreiter mini edition 1.0 (C) 2003 M-M-M *\n" );
    printf( "****************************************************\n" );
    printf( "\n\n" );

    QString interface( argv[1] );
    QString driver( argv[2] );

    printf( "Trying to use '%s' as %s-controlled device...\n", (const char*) interface, (const char*) driver );

    // sanity checks before downcasting
    ONetworkInterface* iface = net->interface( interface );
    if ( !iface )
    {
        printf( "Interface '%s' doesn't exist. Exiting.\n", (const char*) interface );
        exit( -1 );
    }
    if ( !iface->isWireless() )
    {
        printf( "Interface '%s' doesn't support wireless extensions. Exiting.\n", (const char*) interface );
        exit( -1 );
    }

    // downcast should be safe now
    wiface = (OWirelessNetworkInterface*) iface;
    printf( "Using wireless interface '%s' for scanning (current SSID is '%s')...\n", (const char*) interface, (const char*) wiface->SSID() );

    // ifconfig +promisc the interface to receive all packets
    if ( !wiface->promiscuousMode() )
    {
        printf( "Interface status is not promisc... switching to promisc... " );
        wiface->setPromiscuousMode( true );
        if ( !wiface->promiscuousMode() )
        {
            printf( "failed (%s). Exiting.\n", strerror( errno ) );
            exit( -1 );
        }
        else
        {
            printf( "ok.\n" );
        }
    }
    else
        printf( "Interface status is already promisc - good.\n" );

    // connect a monitoring strategy to the interface
    if ( driver == "orinoco" )
        new OOrinocoMonitoringInterface( wiface, false );
    else
    {
        printf( "Unknown driver. Exiting\n" );
        exit( -1 );
    }

    // enable monitoring mode
    printf( "Enabling monitor mode...\n" );
    //wiface->setMonitorMode( true );

    // open a packet capturer
    cap = new OPacketCapturer();
    cap->open( interface );
    if ( !cap->isOpen() )
    {
        printf( "Unable to open libpcap (%s). Exiting.\n", strerror( errno ) );
        exit( -1 );
    }

    // set capturer to non-blocking mode
    cap->setBlocking( false );

    // start channel hopper
    //wiface->setChannelHopping( 1000 );

    // connect
    connect( cap, SIGNAL( receivedPacket(OPacket*) ), this, SLOT( receivePacket(OPacket*) ) );

  }

  ~Wellenreiter() {};

public slots:
  void receivePacket(OPacket* p)
  {
    if (!p)
    {
        printf( "(empty packet received)\n" );
        return;
    }

    OWaveLanManagementPacket* beacon = (OWaveLanManagementPacket*) p->child( "802.11 Management" );
    if ( beacon )
    {
        OWaveLanManagementSSID* ssid = static_cast<OWaveLanManagementSSID*>( p->child( "802.11 SSID" ) );
        QString essid = ssid ? ssid->ID() : "<unknown>";

        if ( stations.find( essid ) )
            stations[essid]->beacons++;
        else
        {
            printf( "found new network @ channel %d, SSID = '%s'\n", wiface->channel(), (const char*) essid );
            stations.insert( essid, new Station( "unknown", wiface->channel(),
            ((OWaveLanPacket*) beacon->parent())->usesWep() ) );
        }
        return;
    }

    OWaveLanDataPacket* data = (OWaveLanDataPacket*) p->child( "802.11 Data" );
    if ( data )
    {
        OWaveLanPacket* wlan = (OWaveLanPacket*) p->child( "802.11" );
        if ( wlan->fromDS() && !wlan->toDS() )
        {
            printf( "FromDS: '%s' -> '%s' via '%s'\n",
                (const char*) wlan->macAddress3().toString(true),
                (const char*) wlan->macAddress1().toString(true),
                (const char*) wlan->macAddress2().toString(true) );
        }
        else
        if ( !wlan->fromDS() && wlan->toDS() )
        {
            printf( "ToDS: '%s' -> '%s' via '%s'\n",
                (const char*) wlan->macAddress2().toString(true),
                (const char*) wlan->macAddress3().toString(true),
                (const char*) wlan->macAddress1().toString(true) );
        }
        else
        if ( wlan->fromDS() && wlan->toDS() )
        {
            printf( "WSD(bridge): '%s' -> '%s' via '%s' and '%s'\n",
                (const char*) wlan->macAddress4().toString(true),
                (const char*) wlan->macAddress3().toString(true),
                (const char*) wlan->macAddress1().toString(true),
                (const char*) wlan->macAddress2().toString(true) );
        }
        else
        {
            printf( "IBSS(AdHoc): '%s' -> '%s' (Cell: '%s')'\n",
                (const char*) wlan->macAddress2().toString(true),
                (const char*) wlan->macAddress1().toString(true),
                (const char*) wlan->macAddress3().toString(true) );
        }
        return;
    }
  }
private:
  OPacketCapturer* cap;
  OWirelessNetworkInterface* wiface;
};


int main( int argc, char** argv )
{
    Wellenreiter w( argc, argv );
    w.exec();
    return 0;
}

#include "miniwellenreiter.moc"

