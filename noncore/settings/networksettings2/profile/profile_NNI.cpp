#include "profileedit.h"
#include "profile_NNI.h"
#include "profile_NN.h"

AProfile::AProfile( ProfileNetNode * PNN ) : ANetNodeInstance( PNN ) {
      Data.Automatic = 1;
      Data.Confirm = 0;
      Data.Description = "";
      Data.Disabled = 0;
      Data.TriggerVPN = 0;
      GUI = 0;
      RT = 0;
}

void AProfile::setSpecificAttribute( QString & Attr, QString & Value ) {

      if ( Attr == "automatic" ) {
        Data.Automatic = (Value=="yes");
      } else if ( Attr == "preconfirm" ) {
        Data.Confirm = (Value=="yes");
      } else if ( Attr == "disabled" ) {
        Data.Disabled = (Value=="yes");
      } else if ( Attr == "triggervpn" ) {
        Data.TriggerVPN = (Value=="yes");
      } else if ( Attr == "description" ) {
        Data.Description = Value;
      }
}

void AProfile::saveSpecificAttribute( QTextStream & TS ) {
      TS << "automatic=" << ((Data.Automatic) ? "yes" : "no") << endl;
      TS << "preconfirm=" << ((Data.Confirm) ? "yes" : "no") << endl;
      TS << "disabled=" << ((Data.Disabled) ? "yes" : "no") << endl;
      TS << "triggervpn=" << ((Data.TriggerVPN) ? "yes" : "no") << endl;
      TS << "description=" << Data.Description << endl;
}

QWidget * AProfile::edit( QWidget * parent ) {
    GUI = new ProfileEdit( parent, this );
    GUI->showData( Data );
    return GUI;
}

QString AProfile::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void AProfile::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

short AProfile::generateFileEmbedded( const QString & ID,
                                     const QString & Path,
                                     QTextStream & TS,
                                     long DevNr ) {

      short rvl, rvd;

      rvl = 1;

      if( ID == "interfaces" ) {
        Log(("Generate Profile for %s\n", ID.latin1() ));
        if( Data.TriggerVPN ) {
          // this profile triggers VPN -> insert trigger
          TS << "  up        networksettings2 --triggervpn" 
             << endl;
          rvl = 0;
        }
      }
      rvd = ANetNodeInstance::generateFileEmbedded( ID, Path, TS, DevNr );
      return (rvd == 2 || rvl == 2 ) ? 2 :
             (rvd == 0 || rvl == 0 ) ? 0 : 1;
}



