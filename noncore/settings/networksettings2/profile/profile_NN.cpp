#include "profile_NN.h"
#include "profile_NNI.h"

static const char * ProfileNeeds[] = 
    { "connection", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
ProfileNetNode::ProfileNetNode() : ANetNode( tr("Regular profile")) {
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

const char * ProfileNetNode::provides( void ) {
      return "fullsetup";
}

bool ProfileNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 0;
}

bool ProfileNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 0;
}

void ProfileNetNode::setSpecificAttribute( QString & , QString & ) {
}

void ProfileNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new ProfileNetNode() );
}
}
