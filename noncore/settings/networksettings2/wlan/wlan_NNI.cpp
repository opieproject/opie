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

bool AWLan::generateDataForCommonFile( 
                                SystemFile &, 
                                long ) {
      return 1;
}

