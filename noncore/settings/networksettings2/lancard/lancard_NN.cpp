#include "lancard_NN.h"
#include "lancard_NNI.h"

static const char * LanCardNeeds[] = 
    { 0 };

/**
 * Constructor, find all of the possible interfaces
 */
LanCardNetNode::LanCardNetNode() : ANetNode() {
}

/**
 * Delete any interfaces that we own.
 */
LanCardNetNode::~LanCardNetNode(){
}

const QString LanCardNetNode::nodeDescription(){
      return tr("\
<p>Sets up a wired regular LAN card.</p>\
<p>Use this to set up 10/100/1000 MBit LAN cards.</p>\
"
);
}

ANetNodeInstance * LanCardNetNode::createInstance( void ) {
      return new ALanCard( this );
}


const char ** LanCardNetNode::needs( void ) {
      return LanCardNeeds;
}

const char * LanCardNetNode::provides( void ) {
      return "device";
}

bool LanCardNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool LanCardNetNode::hasDataFor( const QString & ) {
      return 0;
}

bool LanCardNetNode::generateDataForCommonFile( 
                                SystemFile & , 
                                long ,
                                ANetNodeInstance * ) {
      return 1;
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new LanCardNetNode() );
}
}
