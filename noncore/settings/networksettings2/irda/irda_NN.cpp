#include "irda_NN.h"
#include "irda_NNI.h"

static const char * IRDANeeds[] = 
    { 0
    };

/**
 * Constructor, find all of the possible interfaces
 */
IRDANetNode::IRDANetNode() : ANetNode() {
}

/**
 * Delete any interfaces that we own.
 */
IRDANetNode::~IRDANetNode(){
}

const QString IRDANetNode::nodeDescription(){
      return tr("\
<p>Sets up a infra red serial link.</p>\
"
);
}

ANetNodeInstance * IRDANetNode::createInstance( void ) {
      return new AIRDA( this );
}

const char ** IRDANetNode::needs( void ) {
      return IRDANeeds;
}

const char * IRDANetNode::provides( void ) {
      return "line";
}

bool IRDANetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool IRDANetNode::hasDataFor( const QString & ) {
      return 0;
}

bool IRDANetNode::generateDataForCommonFile( 
                                SystemFile & , 
                                long ,
                                ANetNodeInstance * ) {
      return 1;
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new IRDANetNode() );
}
}
