#include <qpe/qpeapplication.h>
#include "usbedit.h"
#include "usb_NNI.h"
#include "usb_NN.h"

AUSB::AUSB( USBNetNode * PNN ) : ANetNodeInstance( PNN ) {

    GUI = 0;
    RT = 0;
}

void AUSB::setSpecificAttribute( QString & , QString & ) {
}

void AUSB::saveSpecificAttribute( QTextStream & ) {
}


QWidget * AUSB::edit( QWidget * parent ) {
    GUI = new USBEdit( parent );
    GUI->showData( Data ); 
    return GUI;
}

QString AUSB::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void AUSB::commit( void ) {
    if( GUI && GUI->commit( Data ) ) {
      setModified( 1 );
    }
}

bool AUSB::generateDataForCommonFile( SystemFile & S, long DevNr ) {
    AsDevice * Dev = runtime()->device();
    QString NIC = Dev->genNic( DevNr );

    if( S.name() == "interfaces" ) {
      // generate mapping stanza for this interface
      S << "  pre-up " << QPEApplication::qpeDir() << "bin/setmacaddress.sh " << NIC << " || true" << endl;
    }
    return 0;
}


