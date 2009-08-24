#include <resources.h>
#include "profile_NN.h"
#include "profile_NNI.h"

#include "netnodeinterface.h"

static const char * ProfileNeeds[] =
    { "connection",
      0
    };
static const char * ProfileProvides[] =
    { "fullsetup",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
ProfileNetNode::ProfileNetNode() : ANetNode( tr("Regular connection profile")) {

    NSResources->addSystemFile(
      "interfaces", "/etc/network/interfaces", 1 );

}

/**
 * Delete any interfaces that we own.
 */
ProfileNetNode::~ProfileNetNode(){
}

const QString ProfileNetNode::nodeDescription(){
      return tr("\
<p>Define use of an IP connection.</p>\
<p>Configure if and when this connection needs to be established</p>\
"
);
}

ANetNodeInstance * ProfileNetNode::createInstance( void ) {
      return new AProfile( this );
}

const char ** ProfileNetNode::needs( void ) {
      return ProfileNeeds;
}

const char ** ProfileNetNode::provides( void ) {
      return ProfileProvides;
}

void ProfileNetNode::setSpecificAttribute( QString & , QString & ) {
}

void ProfileNetNode::saveSpecificAttribute( QTextStream & ) {
}

OPIE_NS2_PLUGIN( NetNodeInterface_T<ProfileNetNode> )
