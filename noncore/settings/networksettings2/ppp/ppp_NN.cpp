#include "ppp_NN.h"
#include "ppp_NNI.h"

static const char * PPPNeeds[] = 
    { "line", 
      "modem", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
PPPNetNode::PPPNetNode() : ANetNode(tr("PPP Connection")) {
}

/**
 * Delete any interfaces that we own.
 */
PPPNetNode::~PPPNetNode(){
}

const QString PPPNetNode::nodeDescription(){
      return tr("\
<p>Sets up IP using PPP.</p>\
<p>Use this for dialup devices or serial setups</p>\
"
);
}

ANetNodeInstance * PPPNetNode::createInstance( void ) {
      return new APPP( this );
}

const char ** PPPNetNode::needs( void ) {
      return PPPNeeds;
}

const char * PPPNetNode::provides( void ) {
      return "connection";
}

bool PPPNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 0;
}

bool PPPNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 0;
}

QString PPPNetNode::genNic( long NicNr ) { 
      QString S; 
      return S.sprintf( "ppp%ld", NicNr ); 
}

void PPPNetNode::setSpecificAttribute( QString & , QString & ) {
}

void PPPNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new PPPNetNode() );
}
}
