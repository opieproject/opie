#include <qpe/qpeapplication.h>
#include <asdevice.h>
#include "network_NN.h"
#include "network_NNI.h"

static const char * NetworkNeeds[] = 
    { "device", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
NetworkNetNode::NetworkNetNode() : ANetNode(tr("IP Configuration")) {
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

bool NetworkNetNode::hasDataForFile( const QString & S ) {
      return S == "interfaces";
}

short NetworkNetNode::generateFile( const QString & ID,
                                   const QString & ,
                                   QTextStream & TS, 
                                   ANetNodeInstance * NNI,
                                   long DevNr ) {
                                   
      QString NIC = NNI->runtime()->device()->netNode()->nodeClass()->genNic( DevNr );

      if( ID == "interfaces" ) {
        Log(("Generate entry for %s in %s\n", NIC.latin1(), ID.latin1() ));
        // generate mapping stanza for this interface
        TS << "# check if " 
           << NIC 
           << " can be brought UP" 
           << endl;
        TS << "mapping " 
           << NIC 
           << endl;
        TS << "  script "
           << QPEApplication::qpeDir()
           << "/bin/networksettings2-request" 
           << endl 
           << endl;
        return 0;
      }
      return 1;
}

const char ** NetworkNetNode::needs( void ) {
      return NetworkNeeds;
}

const char * NetworkNetNode::provides( void ) {
      return "connection";
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
