#include "lancard_NN.h"
#include "lancard_NNI.h"

static const char * LanCardNeeds[] = 
    { 0
    };

static const char * LanCardProvides[] = 
    { "device",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
LanCardNetNode::LanCardNetNode() : ANetNode(tr("LAN card")), NICMACAddresses() {
    InstanceCount = 2;
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

const char ** LanCardNetNode::provides( void ) {
      return LanCardProvides;
}

QString LanCardNetNode::genNic( long nr ) { 
      QString S; 
      return S.sprintf( "eth%ld", nr );
}

void LanCardNetNode::setSpecificAttribute( QString & A, QString & V ) {
      if( A == "interfacecount" ) {
        InstanceCount = V.toLong();
      } else if ( A == "macaddress" ) {
        NICMACAddresses.append( V );
      }
}

void LanCardNetNode::saveSpecificAttribute( QTextStream & TS) {
      TS << "interfacecount="
         << InstanceCount
         << endl;
      for( QStringList::Iterator it = NICMACAddresses.begin();
           it != NICMACAddresses.end(); 
           ++it ) {
        TS << "macaddress="
           << (*it)
           << endl;
      }
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new LanCardNetNode() );
}
}
