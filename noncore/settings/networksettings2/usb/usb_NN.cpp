#include "usb_NN.h"
#include "usb_NNI.h"

static const char * USBNeeds[] = 
    { 0
    };

/**
 * Constructor, find all of the possible interfaces
 */
USBNetNode::USBNetNode() : ANetNode() {
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
      return 1;
}

bool USBNetNode::hasDataFor( const QString & S, bool ) {
      return (S== "interfaces");
}

bool USBNetNode::generateDataForCommonFile( 
                                SystemFile & S , 
                                long DevNr,
                                ANetNodeInstance * NNI) {
      return ((AUSB *)NNI)->generateDataForCommonFile(S, DevNr);
}

bool USBNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & S , 
                                long DevNr,
                                ANetNodeInstance * NNI) {
      return ((AUSB *)NNI)->generateDeviceDataForCommonFile(S, DevNr);
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new USBNetNode() );
}
}
