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

    Data.IP.IPAutomatic = 1;
    Data.IP.IPAddress = "";
    Data.IP.IPSubMask = "";
    Data.IP.GWAutomatic = 1;
    Data.IP.GWAddress = "";
    Data.IP.GWIsDefault = 1;
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
      if( A == "ipautomatic" ) {
        Data.IP.IPAutomatic = (V == "yes");
      } else if( A == "gwautomatic" ) {
        Data.IP.GWAutomatic = (V == "yes");
      } else if( A == "gwisdefault" ) {
        Data.IP.GWIsDefault = (V == "yes");
      } else if( A == "ipaddress" ) {
        Data.IP.IPAddress = V;
      } else if( A == "ipsubmask" ) {
        Data.IP.IPSubMask = V;
      } else if( A == "gwaddress" ) {
        Data.IP.GWAddress = V;
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
    TS << "ipautomatic=" << ( ( Data.IP.IPAutomatic ) ? "yes" : "no" ) << endl;
    TS << "gwautomatic=" << ( ( Data.IP.GWAutomatic ) ? "yes" : "no" ) << endl;
    TS << "gwisdefault=" << ( ( Data.IP.GWIsDefault ) ? "yes" : "no" ) << endl;
    TS << "ipaddress=" << Data.IP.IPAddress << endl;
    TS << "ipsubmask=" << Data.IP.IPSubMask << endl;
    TS << "gwaddress=" << Data.IP.GWAddress << endl;
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

bool APPP::generateDataForCommonFile( 
                                SystemFile & , 
                                long) {
      return 1;
}

