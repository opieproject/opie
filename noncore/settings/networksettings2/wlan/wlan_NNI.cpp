#include <unistd.h>
#include "wlanedit.h"
#include "wlan_NNI.h"
#include "wlan_NN.h"

AWLan::AWLan( WLanNetNode * PNN ) : ANetNodeInstance( PNN ) {
    GUI = 0;
    RT = 0;
    Data.ESSID = "";
    Data.NodeName = tr("<UseHostName>");
    Data.Mode = 0;
    Data.SpecificAP = 0;
    Data.APMac = "";
    Data.Encrypted = 0;
    Data.AcceptNonEncrypted = 0;
    Data.Key[0] = "";
    Data.Key[1] = "";
    Data.Key[2] = "";
    Data.Key[3] = "";
}

void AWLan::setSpecificAttribute( QString & A, QString & V ) {
    if( A == "essid" ) {
      Data.ESSID = V;
    } else if( A == "nodename" ) {
      Data.NodeName = V;
    } else if( A == "mode" ) {
      Data.Mode = V.toShort();
    } else if( A == "specificap" ) {
      Data.SpecificAP = (V=="yes");
    } else if( A == "apmac" ) {
      Data.APMac = V;
    } else if( A == "encrypted" ) {
      Data.Encrypted = (V=="yes");
    } else if( A == "acceptnonencrypted" ) {
      Data.AcceptNonEncrypted = (V=="yes");
    } else if( A == "key0" ) {
      Data.Key[0] = V;
    } else if( A == "key1" ) {
      Data.Key[1] = V;
    } else if( A == "key2" ) {
      Data.Key[2] = V;
    } else if( A == "key3" ) {
      Data.Key[3] = V;
    }
}

void AWLan::saveSpecificAttribute( QTextStream & S ) {
    S << "essid=" << quote( Data.ESSID ) << endl;
    S << "nodename=" << quote( Data.NodeName ) << endl;
    S << "mode=" << Data.Mode << endl;
    S << "specificap=" 
      << ((Data.SpecificAP) ? "yes" : "no") 
      << endl;
    S << "apmac=" << Data.APMac << endl;
    S << "encrypted=" 
      << ((Data.Encrypted) ? "yes" : "no") 
      << endl;
    S << "acceptnonencrypted=" 
      << ((Data.AcceptNonEncrypted) ? "yes" : "no") 
      << endl;
    for( int i = 0 ;i < 4 ; i ++ ) {
      S << "key" << i << "=" << Data.Key[i] << endl;
    }
}

QWidget * AWLan::edit( QWidget * parent ) {
    GUI = new WLanEdit( parent, this );
    GUI->showData( Data );
    return GUI;
}

QString AWLan::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void AWLan::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

short AWLan::generateFileEmbedded( const QString & ID,
                                  const QString & Path,
                                  QTextStream & TS,
                                  long DevNr ) {

      short rvl, rvd;

      rvl = 1;

      if( ID == "interfaces" ) {
        Log(("Generate WLanNNI for %s\n", ID.latin1() ));
        TS << "  wireless_essid \""
           << Data.ESSID
           << "\""
           << endl;

        if( ! Data.NodeName.isEmpty() ) {
          if( Data.NodeName == tr("<UseHostName>") ) {
            char Buf[100];
            if( gethostname(Buf, sizeof(Buf) ) == 0 ) {
              Buf[99] = '\0'; // just to make sure
              TS << "  wireless_nick "  
                 << Buf
                 << endl;
            }
          } else {
            TS << "  wireless_nick \""  
               << Data.NodeName
               << "\""
               << endl;
          }
        }

        char * M = "Auto";
        switch ( Data.Mode ) {
          case 0 : 
            break;
          case 1 : 
            M = "Managed";
            break;
          case 2 : 
            M = "Ad-Hoc";
            break;
        }

        TS << "  wireless_mode "
           << M
           << endl;
        if( Data.Encrypted ) {
          for( int i = 0; i < 4; i ++ ) {
            if( ! Data.Key[i].isEmpty() ) {
              TS << "  wireless_key"
                 << i
                 << " "
                 << Data.Key[i]
                 << endl;
            }
          }
          TS << "  wireless_keymode "
             << ((Data.AcceptNonEncrypted) ? "open" : "restricted")
             << endl;
        }
        rvl = 0;
      }
      rvd = ANetNodeInstance::generateFileEmbedded( ID, Path, TS, DevNr);

      return (rvd == 2 || rvl == 2 ) ? 2 :
             (rvd == 0 || rvl == 0 ) ? 0 : 1;
}

