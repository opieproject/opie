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

short AUSB::generateFileEmbedded( const QString & ID,
                                 const QString & Path,
                                 QTextStream & TS,
                                 long DevNr ) {
 
    QString NIC = runtime()->device()->netNode()->nodeClass()->genNic( DevNr );
    short rvl, rvd;

    rvl = 1;

    if( ID == "interfaces" ) {
      Log(("Generate USB for %s\n", ID.latin1() ));
      // generate mapping stanza for this interface
      TS << "  pre-up " 
         << QPEApplication::qpeDir() 
         << "bin/setmacaddress.sh " 
         << NIC 
         << " || true" 
         << endl;
      rvl = 0;
    }
    rvd = ANetNodeInstance::generateFileEmbedded( ID, Path, TS, DevNr );

    return (rvd == 2 || rvl == 2 ) ? 2 :
           (rvd == 0 || rvl == 0 ) ? 0 : 1;

}
