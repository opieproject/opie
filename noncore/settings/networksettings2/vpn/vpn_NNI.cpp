#include "vpnedit.h"
#include "vpn_NNI.h"
#include "vpn_NN.h"

AVPN::AVPN( VPNNetNode * PNN ) : ANetNodeInstance( PNN ) {
    GUI = 0;
    RT = 0;
}

void AVPN::setSpecificAttribute( QString & , QString & ) {
}

void AVPN::saveSpecificAttribute( QTextStream & ) {
}

QWidget * AVPN::edit( QWidget * parent ) {
    GUI = new VPNEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString AVPN::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void AVPN::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

bool AVPN::generateDataForCommonFile( 
                                SystemFile & , 
                                long) {
      return 1;
}

