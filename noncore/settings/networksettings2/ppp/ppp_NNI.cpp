#include <qfile.h>
#include <qfileinfo.h>
#include "PPPedit.h"
#include "ppp_NNI.h"
#include "ppp_NN.h"

APPP::APPP( PPPNetNode * PNN ) : ANetNodeInstance( PNN ) {
    Data.DNS.ServerAssigned = 1;
    Data.DNS.DomainName = "";

    Data.Auth.Mode = 0;
    Data.Auth.Login.Expect = "ogin:";
    Data.Auth.Password.Expect = "assword:";
    Data.Auth.PCEMode = 0;
    Data.Auth.Client = "*";
    Data.Auth.Server = "*";
    Data.Auth.Secret = "";

    Data.IP.LocalAddress = "10.0.0.1";
    Data.IP.RemoteAddress = "10.0.0.2";
    Data.IP.LocalOverrule = 1;
    Data.IP.RemoteOverrule = 1;

    Data.IP.GWAutomatic = 1;
    Data.IP.GWAddress = "";
    Data.IP.GWIsDefault = 1;
    Data.IP.GWIfNotSet = 1;

    Data.Run.PreConnect = "";
    Data.Run.PostConnect = "";
    Data.Run.PreDisconnect = "";
    Data.Run.PostDisconnect = "";

    GUI = 0;
    RT = 0;

}

void APPP::setSpecificAttribute( QString & A, QString & V ) {
    if( A.startsWith( "dns" ) ) {
      if( A == "dnsserverassigned" ) {
        Data.DNS.ServerAssigned = (V == "yes");
      } else if( A == "dnsdomainname" ) {
        Data.DNS.DomainName = V;
      } else if( A == "dnsserver" ) {
        Data.DNS.Servers.resize( Data.DNS.Servers.size()+1 );
        Data.DNS.Servers[Data.DNS.Servers.size()-1] = 
            new QString( V );
      }
    } else if( A.startsWith( "auth" ) ) {
      if( A == "authmode" ) {
        Data.Auth.Mode = V.toShort();
      } else if( A == "authloginexpect" ) {
        Data.Auth.Login.Expect = V;
      } else if( A == "authloginsend" ) {
        Data.Auth.Login.Send = V;
      } else if( A == "authpasswordexpect" ) {
        Data.Auth.Password.Expect = V;
      } else if( A == "authpasswordsend" ) {
        Data.Auth.Password.Send = V;
      } else if( A == "authpcemode" ) {
        Data.Auth.PCEMode = V.toShort();
      } else if( A == "authclient" ) {
        Data.Auth.Client = V;
      } else if( A == "authserver" ) {
        Data.Auth.Server = V;
      } else if( A == "authsecret" ) {
        Data.Auth.Secret = V;
      }
    } else if( A.startsWith( "ip" ) ) {
      if( A == "iplocaloverrule" ) {
        Data.IP.LocalOverrule = (V == "yes");
      } else if( A == "ipremoteoverrule" ) {
        Data.IP.RemoteOverrule = (V == "yes");
      } else if( A == "ipgwautomatic" ) {
        Data.IP.GWAutomatic = (V == "yes");
      } else if( A == "ipgwisdefault" ) {
        Data.IP.GWIsDefault = (V == "yes");
      } else if( A == "ipgwifnotset" ) {
        Data.IP.GWIfNotSet = (V == "yes");
      } else if( A == "iplocaladdress" ) {
        Data.IP.LocalAddress = V;
      } else if( A == "ipremoteaddress" ) {
        Data.IP.RemoteAddress = V;
      } else if( A == "ipgwaddress" ) {
        Data.IP.GWAddress = V;
      }
    } else if( A.startsWith( "run" ) ) {
      if( A == "runpreconnect" ) {
        Data.Run.PreConnect = V;
      } else if( A == "runpostconnect" ) {
        Data.Run.PostConnect = V;
      } else if( A == "runpredisconnect" ) {
        Data.Run.PreDisconnect = V;
      } else if( A == "runpostdisconnect" ) {
        Data.Run.PostDisconnect = V;
      }
    }
}

void APPP::saveSpecificAttribute( QTextStream & TS ) {
    TS << "dnsserverassigned=" << 
        ( ( Data.DNS.ServerAssigned ) ? "yes" : "no" ) << endl;
    TS << "dnsdomainname=" << Data.DNS.DomainName << endl;
    for( unsigned int i = 0; i < Data.DNS.Servers.size(); i ++ ) {
      TS << "dnsserver=" << *(Data.DNS.Servers[i]) << endl;
    }
    TS << "authmode=" << Data.Auth.Mode << endl;
    TS << "authloginexpect=" << quote( Data.Auth.Login.Expect ) << endl;
    TS << "authloginsend=" << quote( Data.Auth.Login.Send ) << endl;
    TS << "authpasswordexpect=" << quote( Data.Auth.Password.Expect ) << endl;
    TS << "authpasswordsend=" << quote( Data.Auth.Password.Send ) << endl;
    TS << "authpcemode=" << Data.Auth.PCEMode << endl;
    TS << "authclient=" << Data.Auth.Client << endl;
    TS << "authserver=" << Data.Auth.Server << endl;
    TS << "authsecret=" << quote( Data.Auth.Secret ) << endl;
    TS << "ipgwautomatic=" << ( ( Data.IP.GWAutomatic ) ? "yes" : "no" ) << endl;
    TS << "ipgwisdefault=" << ( ( Data.IP.GWIsDefault ) ? "yes" : "no" ) << endl;
    TS << "ipgwifnotset=" << ( ( Data.IP.GWIfNotSet ) ? "yes" : "no" ) << endl;
    TS << "iplocaloverrule=" << ( ( Data.IP.LocalOverrule ) ? "yes" : "no" ) << endl;
    TS << "ipremoteoverrule=" << ( ( Data.IP.RemoteOverrule ) ? "yes" : "no" ) << endl;
    TS << "iplocaladdress=" << Data.IP.LocalAddress << endl;
    TS << "ipremoteaddress=" << Data.IP.RemoteAddress << endl;
    TS << "ipgwaddress=" << Data.IP.GWAddress << endl;

    TS << "runpreconnect=" << Data.Run.PreConnect << endl;
    TS << "runpostconnect=" << Data.Run.PostConnect << endl;
    TS << "runpredisconnect=" << Data.Run.PreDisconnect << endl;
    TS << "runpostdisconnect=" << Data.Run.PostDisconnect << endl;

}

QWidget * APPP::edit( QWidget * parent ) {
    GUI = new PPPEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString APPP::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void APPP::commit( void ) {
    if( GUI && GUI->commit( Data ) ) {
      setModified( 1 );
    }
}

bool APPP::openFile( SystemFile & SF ) {
      if( SF.name() == "peers" ) {
        SF.setPath( removeSpaces( 
            QString( "/tmp/ppp/peers/" ) + networkSetup()->name() ) );
        return 1;
      } else if ( SF.name() == "chatscripts" ) {
        SF.setPath( removeSpaces( 
            QString( "/tmp/chatscripts/" ) + networkSetup()->name() ) );
        return 1;
      }
      return 0;
}

short APPP::generateFile( SystemFile & SF, long DevNr ) {
    short rvl, rvd;

    rvl = 1;
    rvd = 1;

    if( SF.name() == "pap-secrets" ) {
      Log(("Generate PPP for %s\n", SF.name().latin1() ));

      if( Data.Auth.Mode == 1 && Data.Auth.PCEMode == 0 ) {
        SF << "# secrets for " 
           << networkSetup()->name().latin1() 
           << endl;
        SF << Data.Auth.Client 
           << " " 
           << Data.Auth.Server 
           << " " 
           << Data.Auth.Secret
           << endl;
        rvl = 0;
        rvd = networkSetup()->getToplevel()->generateFileEmbedded( 
                SF, DevNr );
      }
    } else if( SF.name() == "chap-secrets" ) {
      Log(("Generate PPP for %s\n", SF.name().latin1() ));
      if( Data.Auth.Mode == 1 && Data.Auth.PCEMode != 0 ) {
        // used for both EAP and Chap
        SF << "# secrets for "
           << networkSetup()->name().latin1() 
           << endl;
        SF << Data.Auth.Client 
           << " " 
           << Data.Auth.Server 
           << " " 
           << Data.Auth.Secret
           << endl;

        rvl = 0;
        rvd = networkSetup()->getToplevel()->generateFileEmbedded( 
            SF, DevNr );
      }
    } else if ( SF.name() == "peers" ) {

      QFileInfo FI(SF.path());
      Log(("Generate PPP for %s\n", SF.name().latin1() ));

      SF << "connect \"/usr/sbin/chat -v -f /etc/chatscripts/"
         << FI.baseName()
         << "\""
         << endl;

      if( Data.IP.GWIsDefault ) {
        SF << "defaultroute" 
           << endl;
      }

      SF << "linkname "
         << removeSpaces( SF.name().latin1() )
         << endl;

      // insert other data here
      rvl = 0;
      rvd = networkSetup()->getToplevel()->generateFileEmbedded(
            SF, DevNr );
    } else if ( SF.name() == "chatscripts" ) {
      Log(("Generate PPP for %s\n", SF.name().latin1() ));
      rvl = 0;
      rvd = networkSetup()->getToplevel()->generateFileEmbedded(
            SF, DevNr );
    }

    return (rvd == 2 || rvl == 2 ) ? 2 :
           (rvd == 0 || rvl == 0 ) ? 0 : 1;
}
