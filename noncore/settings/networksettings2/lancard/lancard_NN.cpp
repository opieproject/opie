#include "lancard_NN.h"
#include "lancard_NNI.h"

static const char * LanCardNeeds[] = 
    { 0 };

/**
 * Constructor, find all of the possible interfaces
 */
LanCardNetNode::LanCardNetNode() : ANetNode() {
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

const char * LanCardNetNode::provides( void ) {
      return "device";
}

bool LanCardNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool LanCardNetNode::hasDataFor( const QString & S ) {
      return S == "interfaces";
}

bool LanCardNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & S , 
                                long DevNr ) {
      QString NIC = genNic( DevNr );

      if( S.name() == "interfaces" ) {
        // generate mapping stanza for this interface
        S << "# check if " << NIC << " can be brought UP" << endl;
        S << "mapping " << NIC << endl;
        S << "  script networksettings2-request" << endl << endl;
      }
      return 0;
}

QString LanCardNetNode::genNic( long nr ) { 
      QString S; 
      return S.sprintf( "eth%ld", nr );
}

void LanCardNetNode::setSpecificAttribute( QString & A, QString & V ) {
      if( A == "interfacecount" ) {
        InstanceCount = V.toLong();
      }
}

void LanCardNetNode::saveSpecificAttribute( QTextStream & TS) {
      TS << "interfacecount="
         << InstanceCount
         << endl;
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new LanCardNetNode() );
}
}
