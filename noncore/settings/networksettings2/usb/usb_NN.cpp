#include "usb_NN.h"
#include "usb_NNI.h"

static const char * USBNeeds[] = 
    { 0
    };

/**
 * Constructor, find all of the possible interfaces
 */
USBNetNode::USBNetNode() : ANetNode(tr("USB Cable Connect")) {
}

/**
 * Delete any interfaces that we own.
 */
USBNetNode::~USBNetNode(){
}

const QString USBNetNode::nodeDescription(){
      return tr("\
<p>Configure Ethernet over USB.</p>\
<p>Use this for a computer to computer USB cable connection</p>\
"
);
}

ANetNodeInstance * USBNetNode::createInstance( void ) {
      return new AUSB( this );
}

const char ** USBNetNode::needs( void ) {
      return USBNeeds;
}

const char * USBNetNode::provides( void ) {
      return "device";
}

bool USBNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 0;
}

bool USBNetNode::hasDataFor( const QString & S ) {
      return (S== "interfaces");
}

bool USBNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & S , 
                                long DevNr ) {
    QString NIC = genNic( DevNr );

    if( S.name() == "interfaces" ) {
      // generate mapping stanza for this interface
      S << "# check if " << NIC << " can be brought UP" << endl;
      S << "mapping " << NIC << endl;
      S << "  script networksettings2-request" << endl << endl;
    }
    return 0;
}

QString USBNetNode::genNic( long ) {
    return QString( "usbf" ); 
}

void USBNetNode::setSpecificAttribute( QString & , QString & ) {
}

void USBNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new USBNetNode() );
}
}
