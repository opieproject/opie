#include "lancardedit.h"
#include "lancard_NNI.h"
#include "lancard_NN.h"

ALanCard::ALanCard( LanCardNetNode * PNN ) : ANetNodeInstance( PNN ) {

    Data.AnyLanCard = 1;
    Data.HWAddresses.clear();

    GUI = 0;
    RT = 0;
}

void ALanCard::setSpecificAttribute( QString & A, QString & V ) {
    if( A == "matchanycard" ) {
      Data.AnyLanCard = (V == "yes" );
    } else if( A == "match" ) {
      Data.HWAddresses.append( V );
    }
}

void ALanCard::saveSpecificAttribute( QTextStream & TS) {
    TS << "matchanycard=" << 
      ((Data.AnyLanCard) ? "yes" : "no") << endl;
    for( QStringList::Iterator it = Data.HWAddresses.begin(); 
         it != Data.HWAddresses.end(); ++it ) {
      TS << "match=" << quote( *it ) << endl;
    }
}

QWidget * ALanCard::edit( QWidget * parent ) {
    GUI = new LanCardEdit( parent );
    GUI->showData( Data );
    return GUI;
}

QString ALanCard::acceptable( void ) {
    return ( GUI ) ? GUI->acceptable( ) : QString();
}

void ALanCard::commit( void ) {
    if( GUI && GUI->commit( Data ) )
      setModified( 1 );
}

bool ALanCard::generateDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 1;
}

