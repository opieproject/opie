#include "cable_NN.h"
#include "cable_NNI.h"

static const char * CableNeeds[] = 
    { 0
    };

/**
 * Constructor, find all of the possible interfaces
 */
CableNetNode::CableNetNode() : ANetNode(tr("Cable Connection")) {
}

/**
 * Delete any interfaces that we own.
 */
CableNetNode::~CableNetNode(){
}

const QString CableNetNode::nodeDescription(){
      return tr("\
<p>Sets up a wired serial or parallel.</p>\
"
);
}

ANetNodeInstance * CableNetNode::createInstance( void ) {
      return new ACable( this );
}

const char ** CableNetNode::needs( void ) {
      return CableNeeds;
}

const char * CableNetNode::provides( void ) {
      return "line";
}

void CableNetNode::setSpecificAttribute( QString & , QString &  ) {
}

void CableNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new CableNetNode() );
}
}
