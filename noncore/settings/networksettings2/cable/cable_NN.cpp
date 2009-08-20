#include "cable_NN.h"
#include "cable_NNI.h"

#include "netnodeinterface.h"

static const char * CableNeeds[] =
    { 0
    };

static const char * CableProvides[] =
    { "line",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
CableNetNode::CableNetNode() : ANetNode(tr("Cable connection")) {
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

const char ** CableNetNode::provides( void ) {
      return CableProvides;
}

void CableNetNode::setSpecificAttribute( QString & , QString &  ) {
}

void CableNetNode::saveSpecificAttribute( QTextStream & ) {
}

OPIE_NS2_PLUGIN(  NetNodeInterface_T<CableNetNode> )
