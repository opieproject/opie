#include <system.h>
#include <netnode.h>
#include "GPRSedit.h"
#include "GPRS_NNI.h"
#include "GPRS_NN.h"

AGPRSDevice::AGPRSDevice( GPRSNetNode * PNN ) : ANetNodeInstance( PNN ) {
    Data.APN = "";
    Data.User = "";
    Data.Password = "";
    Data.DefaultGateway = 1;
    Data.SetIfSet = 0;
    Data.Debug = 0;
    Data.Routing.setAutoDelete( TRUE );
    GUI = 0;
    RT = 0;
}

void AGPRSDevice::setSpecificAttribute( QString & A, QString & V ) {
    if( A == "apn" ) {
      Data.APN = V;
    } else if( A == "user" ) {
      Data.User = V;
    } else if( A == "password" ) {
      Data.Password = V;
    } else if( A == "dns2" ) {
      Data.DNS2 = V;
    } else if( A == "dns1" ) {
      Data.DNS1 = V;
    } else if( A == "defaultgateway" ) {
      Data.DefaultGateway = (V=="yes");
    } else if( A == "setifset" ) {
      Data.SetIfSet = (V == "yes");
    } else if( A == "routes" ) {
      Data.Routing.resize( V.toULong() );
    } else if( A.startsWith( "route" ) ) {
      QStringList SL = QStringList::split( "/", V );
      GPRSRoutingEntry * E = new GPRSRoutingEntry;

      E->Address = SL[0];
      E->Mask = SL[1].toULong();

      Data.Routing.insert( A.mid(5).toULong(), E );
    } else if( A == "debug" ) {
      Data.Debug = V.toShort();
    }
}

void AGPRSDevice::saveSpecificAttribute( QTextStream & TS ) {
    TS << "apn=" << Data.APN << endl;
    TS << "user=" << Data.User << endl;
    TS << "password=" << Data.Password << endl;
    TS << "dns1=" << Data.DNS1 << endl;
    TS << "dns2=" << Data.DNS2 << endl;
    TS << "defaultgateway=" << ( (Data.DefaultGateway) ? "yes" : "no" ) << endl;
    TS << "setifset=" << ((Data.SetIfSet) ? "yes" : "no") << endl;
    TS << "debug=" << Data.Debug << endl;

    TS << "routes=" << Data.Routing.count() << oendl;
    for( unsigned int i = 0; i < Data.Routing.count(); i ++ ) {
      TS << "route" << i << "="
         << Data.Routing[i]->Address
         << "/"
         << Data.Routing[i]->Mask
         << oendl;
    }
}

QWidget * AGPRSDevice::edit( QWidget * parent ) {
    GUI = new GPRSEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString AGPRSDevice::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void AGPRSDevice::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

bool AGPRSDevice::hasDataForFile( SystemFile & S ) {
      return S.name() == "pap-secrets" ||
             S.name() == "peers" ||
             S.name() == "extra" ||
             S.name() == "chatscripts" ;
}

short AGPRSDevice::generateFile( SystemFile & SF,
                                 long 
                               ) {

    if( SF.name() == "pap-secrets" ) {
      SF << Data.User 
         << " * " 
         << Data.Password 
         << " *" 
         << endl;
      return 0;
    } else if( SF.name() == "chatscripts" ) {
      SF << "SAY          \"Starting\\n\"" << oendl;
      SF << "ECHO         OFF" << oendl;
      SF << "ABORT        BUSY" << oendl;
      SF << "ABORT        ERROR" << oendl;
      SF << "ABORT        VOICE" << oendl;
      SF << "ABORT        \"NO CARRIER\"" << oendl;
      SF << "ABORT        \"NO DIALTONE\"" << oendl;
      SF << "\"\"           AT" << oendl;
      SF << "OK           AT+CGATT=1" << oendl;
      SF << "OK           AT+CGDCONT=1,\"IP\",\""
         << Data.APN 
         << "\"" 
         << oendl;
      SF << "OK           ATD*99***1#\\n" << oendl;
      SF << "TIMEOUT      10" << oendl;
      SF << "CONNECT      \"\"" << oendl;
      SF << "SAY          \"READY\\n\"" << oendl;
      return 0;
    } else if( SF.name() == "peers" ) {
      SF << "noauth" << oendl;
      SF << "user " << Data.User << oendl;
      SF << "connect \"/usr/sbin/chat -s -v -f /etc/chatscripts/" 
         << removeSpaces( networkSetup()->name() ) 
         << "\"" 
         << oendl;
      SF << "ipcp-accept-local" << oendl;
      SF << "ipcp-accept-remote" << oendl;
      if( Data.DefaultGateway ) {
        SF << "defaultroute" << oendl;
        if( Data.SetIfSet ) {
          SF << "replacedefaultroute" << oendl;
        }
      }
      if( Data.Debug ) {
        SF << "logfile /tmp/" 
           << removeSpaces( networkSetup()->name() ) 
           << oendl;
        for( int i = 0; i < Data.Debug; i ++ ) {
          SF << "debug" << oendl;
        }
      }
      SF << "nocrtscts" << oendl;
      SF << "local" << oendl;
      SF << "lcp-echo-interval 0" << oendl;
      SF << "lcp-echo-failure 0" << oendl;
      SF << "usepeerdns" << oendl;
      SF << "linkname " << removeSpaces( networkSetup()->name() ) << oendl;
      SF << "nopersist" << oendl;
      SF << "ipparam " << removeSpaces( networkSetup()->name() ) <<oendl;
      SF << "maxfail 1" << oendl;
      return 0;
    } else if( SF.name() == "extra" ) {
      unsigned long Bits;
      // generate 'fixed' settings
      for( unsigned int i = 0 ; 
           i < Data.Routing.count();
           i ++ ) {
        if( Data.Routing[i]->Mask == 32 ) {
          Bits = 0xffffffff;
        } else {
          Bits = ~ ((1 << ((32-Data.Routing[i]->Mask))) - 1);
        }
        SF << "route add -net "
           << Data.Routing[i]->Address
           << " netmask "
           << ((Bits&0xff000000)>>24)
           << "."
           << ((Bits&0x00ff0000)>>16)
           << "."
           << ((Bits&0x0000ff00)>>8)
           << "."
           << ((Bits&0x000000ff))
           << " gw $PPP_REMOTE"
           << oendl;
        SF << "route del -net "
           << Data.Routing[i]->Address
           << " netmask "
           << ((Bits&0xff000000)>>24)
           << "."
           << ((Bits&0x00ff0000)>>16)
           << "."
           << ((Bits&0x0000ff00)>>8)
           << "."
           << ((Bits&0x000000ff))
           << " gw $PPP_REMOTE"
           << oendl;
      }

      if( ! Data.DNS1.isEmpty() ) {
        SF << "nameserver "
           << Data.DNS1
           << " # profile "
           << removeSpaces( networkSetup()->name() )
           <<oendl;
      }

      if( ! Data.DNS2.isEmpty() ) {
        SF << "nameserver "
           << Data.DNS2
           << " # profile "
           << removeSpaces( networkSetup()->name() )
           <<oendl;
      }
    }
    return 1;
}

bool AGPRSDevice::openFile( SystemFile & SF, QStringList & SL ) {
    bool retval = false;
    if( SF.name() == "peers" ) {
      SL << "/etc" << "ppp" << "peers";
      SF.setPath( 
          QString( "/etc/ppp/peers/" ) + 
              removeSpaces( networkSetup()->name() )
          );
      retval = true;
    } else if ( SF.name() == "chatscripts" ) {
      SL << "/etc" << "chatscripts";
      SF.setPath( 
          QString( "/etc/chatscripts/" ) + 
              removeSpaces( networkSetup()->name() )
          );
      retval = true;
    } else if ( SF.name() == "extra" ) {
      SF.setPath( 
          QString( "/etc/ppp/" ) + 
              removeSpaces( networkSetup()->name() ) + ".fixed"
          );
      retval = true;
    }
    return retval;
}
