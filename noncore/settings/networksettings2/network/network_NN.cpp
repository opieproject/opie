#include "network_NN.h"
#include "network_NNI.h"

static const char * NetworkNeeds[] = 
    { "device", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
NetworkNetNode::NetworkNetNode() : ANetNode() {
}

/**
 * Delete any interfaces that we own.
 */
NetworkNetNode::~NetworkNetNode(){
}

const QString NetworkNetNode::nodeDescription(){
      return tr("\
<p>Sets up TCP/IP options.</p>\
<p>Use this to configure the TCP/IP protocol</p>\
"
);
}

ANetNodeInstance * NetworkNetNode::createInstance( void ) {
      return new ANetwork( this );
}

const char ** NetworkNetNode::needs( void ) {
      return NetworkNeeds;
}

const char * NetworkNetNode::provides( void ) {
      return "connection";
}

bool NetworkNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool NetworkNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 1;
}

void NetworkNetNode::setSpecificAttribute( QString & , QString & ) {
}

void NetworkNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new NetworkNetNode() );
}
}
