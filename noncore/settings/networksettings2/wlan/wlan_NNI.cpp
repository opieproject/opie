#include "wlanedit.h"
#include "wlan_NNI.h"
#include "wlan_NN.h"

AWLan::AWLan( WLanNetNode * PNN ) : ANetNodeInstance( PNN ) {
    GUI = 0;
    RT = 0;
}

void AWLan::setSpecificAttribute( QString & , QString & ) {
}

void AWLan::saveSpecificAttribute( QTextStream & ) {
}

QWidget * AWLan::edit( QWidget * parent ) {
    GUI = new WLanEdit( parent );
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

bool AWLan::generateDeviceDataForCommonFile( SystemFile & S, long DevNr ) {
    AsDevice * Dev = runtime()->device();
    QString NIC = Dev->genNic( DevNr );

    if( S.name() == "interfaces" ) {
      // generate mapping stanza for this interface
      S << "# check if " << NIC << " can be brought UP" << endl;
      S << "mapping " << NIC << endl;
      S << "  script networksettings2-request" << endl << endl;
    }
    return 0;
}
