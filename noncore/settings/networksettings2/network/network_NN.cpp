#include <qpe/qpeapplication.h>
#include <resources.h>
#include <netnode.h>
#include "network_NN.h"
#include "network_NNI.h"

static const char * NetworkNeeds[] = 
    { "device", 
      0
    };

static const char * NetworkProvides[] = 
    { "connection",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
NetworkNetNode::NetworkNetNode() : ANetNode(tr("IP Configuration")) {

      NSResources->addSystemFile(
        "interfaces", "/etc/network/interfaces", 1 );
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

bool NetworkNetNode::hasDataForFile( SystemFile & S ) {
      return S.name() == "interfaces";
}

short NetworkNetNode::generateFile( SystemFile & SF, 
                                   ANetNodeInstance * NNI,
                                   long DevNr ) {
                                   
      if( DevNr < 0 ) {
        // generate device specific but common part
        return 1;
      }

      QString NIC = NNI->runtime()->device()->netNode()->nodeClass()->genNic( DevNr );

      if( SF.name() == "interfaces" ) {
        Log(("Generate entry for %s in %s\n", NIC.latin1(), SF.name().latin1() ));
        // generate mapping stanza for this interface
        SF << "# check if " 
           << NIC 
           << " can be brought UP" 
           << endl;
        SF << "mapping " 
           << NIC 
           << endl;
        SF << "  script "
           << QPEApplication::qpeDir()
           << "bin/networksettings2-request" 
           << endl 
           << endl;
        return 0;
      }
      return 1;
}

const char ** NetworkNetNode::needs( void ) {
      return NetworkNeeds;
}

const char ** NetworkNetNode::provides( void ) {
      return NetworkProvides;
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
