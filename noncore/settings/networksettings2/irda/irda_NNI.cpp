#include "irdaedit.h"
#include "irda_NNI.h"
#include "irda_NN.h"

AIRDA::AIRDA( IRDANetNode * PNN ) : ANetNodeInstance( PNN ) {

    GUI = 0;
    RT = 0;
}

void AIRDA::setSpecificAttribute( QString & , QString & ) {
}

void AIRDA::saveSpecificAttribute( QTextStream & ) {
}

QWidget * AIRDA::edit( QWidget * parent ) {
    GUI = new IRDAEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString AIRDA::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void AIRDA::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

bool AIRDA::generateDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 1;
}

