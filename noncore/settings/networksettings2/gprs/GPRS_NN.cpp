#include <resources.h>
#include <qpe/qpeapplication.h>
#include <netnode.h>
#include "GPRS_NN.h"
#include "GPRS_NNI.h"

static const char * GPRSNeeds[] = 
    { "GPRS",
      0
    };

static const char * GPRSProvides[] = 
    { "connection", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
GPRSNetNode::GPRSNetNode() : ANetNode(tr("GPRS capable device")) {
      NSResources->addSystemFile( 
        "pap-secrets", "/etc/ppp/pap-secrets", 0 );
}

/**
 * Delete any interfaces that we own.
 */
GPRSNetNode::~GPRSNetNode(){
}

const QString GPRSNetNode::nodeDescription(){
      return tr("\
<p>provides access to a GPRS capable device.</p>\
"
);
}

ANetNodeInstance * GPRSNetNode::createInstance( void ) {
      return new AGPRSDevice( this );
}

bool GPRSNetNode::hasDataForFile( SystemFile & S ) {
      return S.name() == "pap-secrets";
}

short GPRSNetNode::generateFile( SystemFile & , 
                                 ANetNodeInstance * ,
                                 long ) {
                                   
      return 0;
}

const char ** GPRSNetNode::needs( void ) {
      return GPRSNeeds;
}

const char ** GPRSNetNode::provides( void ) {
      return GPRSProvides;
}

void GPRSNetNode::setSpecificAttribute( QString & , QString & ) {
}

void GPRSNetNode::saveSpecificAttribute( QTextStream & ) {
}

QStringList GPRSNetNode::properFiles( void ) {
      QStringList SL;

      SL << "peers";
      SL << "chatscripts";
      SL << "extra";
      return SL;
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new GPRSNetNode() );
}
}
