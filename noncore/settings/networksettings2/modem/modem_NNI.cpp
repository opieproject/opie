#include "modemedit.h"
#include "modem_NNI.h"
#include "modem_NN.h"

AModem::AModem( ModemNetNode * PNN ) : ANetNodeInstance( PNN ) {

    GUI = 0;
    RT = 0;
}

void AModem::setSpecificAttribute( QString & , QString & ) {
}

void AModem::saveSpecificAttribute( QTextStream & ) {
}

QWidget * AModem::edit( QWidget * parent ) {
    GUI = new ModemEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString AModem::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void AModem::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

bool AModem::generateDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 1;
}

