#include <system.h>
#include <asdevice.h>
#include "networkedit.h"
#include "network_NNI.h"
#include "network_NN.h"

ANetwork::ANetwork( NetworkNetNode * PNN ) : ANetNodeInstance( PNN ) {
    Data.UseDHCP = 1;
    Data.IPAddress = "";
    Data.NetMask = "";
    Data.Broadcast = "";
    Data.Gateway = "";
    Data.DNS1 = "";
    Data.DNS2 = "";
    Data.SendHostname = 0;
    Data.Hostname = "";
    Data.PreUp_SL.clear();
    Data.PreDown_SL.clear();
    Data.PostUp_SL.clear();
    Data.PostDown_SL.clear();
    GUI = 0;
    RT = 0;
}

void ANetwork::setSpecificAttribute( QString & A, QString & V ) {
    if( A == "usedhcp" ) {
      Data.UseDHCP = (V == "yes");
    } else if( A == "sendhostname" ) {
      Data.SendHostname = (V=="yes");
    } else if( A == "hostname" ) {
      Data.Hostname = V;
    } else if( A == "ipaddress" ) {
      Data.IPAddress = V;
    } else if( A == "netmask" ) {
      Data.NetMask = V;
    } else if( A == "broadcast" ) {
      Data.Broadcast = V;
    } else if( A == "gateway" ) {
      Data.Gateway = V;
    } else if( A == "dns1" ) {
      Data.DNS1 = V;
    } else if( A == "dns2" ) {
      Data.DNS2 = V;
    } else if( A == "preup" ) {
      Data.PreUp_SL.append( V );
    } else if( A == "predown" ) {
      Data.PreDown_SL.append( V );
    } else if( A == "postup" ) {
      Data.PostUp_SL.append( V );
    } else if( A == "postdown" ) {
      Data.PostDown_SL.append( V );
    }
}

void ANetwork::saveSpecificAttribute( QTextStream & TS ) {
    TS << "usedhcp=" << ((Data.UseDHCP) ? "yes" : "no") << endl;
    TS << "sendhostname=" << ((Data.SendHostname) ? "yes" : "no") << endl;
    TS << "hostname=" << Data.Hostname << endl;
    TS << "ipaddress=" << Data.IPAddress << endl;
    TS << "netmask=" << Data.NetMask << endl;
    TS << "broadcast=" << Data.Broadcast << endl;
    TS << "gateway=" << Data.Gateway << endl;
    TS << "dns1=" << Data.DNS1 << endl;
    TS << "dns2=" << Data.DNS2 << endl;
    for ( QStringList::Iterator it = Data.PreUp_SL.begin(); 
          it != Data.PreUp_SL.end(); 
          ++it ) {
      TS << "preup=" << quote(*it) << endl;
    }
    for ( QStringList::Iterator it = Data.PreDown_SL.begin(); 
          it != Data.PreDown_SL.end(); 
          ++it ) {
      TS << "predown=" << quote(*it) << endl;
    }
    for ( QStringList::Iterator it = Data.PostUp_SL.begin(); 
          it != Data.PostUp_SL.end(); 
          ++it ) {
      TS << "postup=" << quote(*it) << endl;
    }
    for ( QStringList::Iterator it = Data.PostDown_SL.begin(); 
          it != Data.PostDown_SL.end(); 
          ++it ) {
      TS << "postdown=" << quote(*it) << endl;
    }
}

QWidget * ANetwork::edit( QWidget * parent ) {
    GUI = new NetworkEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString ANetwork::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void ANetwork::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

bool ANetwork::generateDataForCommonFile( SystemFile & S, long DevNr ) {
    AsDevice * Dev = runtime()->device();
    QString NIC = Dev->genNic( DevNr );

    if( S.name() == "interfaces" ) {
      // we can safely call from here since device item is deeper
      if( Data.UseDHCP ) {
        S << "iface " << NIC << "-c" << connection()->number() << 
              "-allowed inet dhcp" << endl;
        S << "  up echo \"" << NIC << "\" > /tmp/profile-" << connection()->number() << 
            ".up" << Data.IPAddress << endl;
        if( Data.SendHostname ) {
          S << "  hostname "<< Data.Hostname  << endl;
        }

        S << "  down rm -f /tmp/profile-" << connection()->number() << 
            ".up" << Data.IPAddress << endl;
      } else {
        S << "iface " << NIC << "-c" << connection()->number() << 
              "-allowed inet static" << endl;
        S << "  up echo \"" << NIC << "\" > /tmp/profile-" << connection()->number() << 
            ".up" << Data.IPAddress << endl;
        S << "  down rm -f /tmp/profile-" << connection()->number() << 
            ".up" << Data.IPAddress << endl;
        S << "  address   " << Data.IPAddress << endl;
        S << "  broadcast " << Data.Broadcast << endl;
        S << "  netmask   " << Data.NetMask << endl;

        // derive network address = IPAddress & netmask
        { QString NW;
          QStringList ipal = QStringList::split( '.', Data.IPAddress );
          QStringList nmal = QStringList::split( '.', Data.NetMask );

          NW = QString( "%1.%2.%3.%4" ).
              arg( ipal[0].toShort() & nmal[0].toShort() ).
              arg( ipal[1].toShort() & nmal[1].toShort() ).
              arg( ipal[2].toShort() & nmal[2].toShort() ).
              arg( ipal[3].toShort() & nmal[3].toShort() );
          S << "  network   " << NW << endl;
        }
      }
      for ( QStringList::Iterator it = Data.PreUp_SL.begin(); 
            it != Data.PreUp_SL.end(); 
            ++it ) {
        S << "  pre-up    " << (*it) << endl;
      }
      for ( QStringList::Iterator it = Data.PostUp_SL.begin(); 
            it != Data.PostUp_SL.end(); 
            ++it ) {
        S << "  up        " << (*it) << endl;
      }
      for ( QStringList::Iterator it = Data.PreDown_SL.begin(); 
            it != Data.PreDown_SL.end(); 
            ++it ) {
        S << "  down      " << (*it) << endl;
      }
      for ( QStringList::Iterator it = Data.PostDown_SL.begin(); 
            it != Data.PostDown_SL.end(); 
            ++it ) {
        S << "  post-down " << (*it) << endl;
      }
    }
    return 0;
}
