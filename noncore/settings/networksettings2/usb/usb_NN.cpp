#include <resources.h>
#include "usb_NN.h"
#include "usb_NNI.h"

#ifndef MYPLUGIN

#include "netnodeinterface.h"

#endif

static const char * USBNeeds[] = 
    { 0
    };

static const char * USBProvides[] = 
    { "device",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
USBNetNode::USBNetNode() : ANetNode(tr("USB Cable Connect")) {
    NSResources->addSystemFile(
      "interfaces", "/etc/network/interfaces", 1 );
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

const char ** USBNetNode::provides( void ) {
      return USBProvides;
}

QString USBNetNode::genNic( long ) {
    return QString( "usbf" ); 
}

void USBNetNode::setSpecificAttribute( QString & , QString & ) {
}

void USBNetNode::saveSpecificAttribute( QTextStream & ) {
}

#ifdef MYPLUGIN 

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new USBNetNode() );
}

#else

OPIE_NS2_PLUGIN( NetNodeInterface<USBNetNode> )

#endif

}
