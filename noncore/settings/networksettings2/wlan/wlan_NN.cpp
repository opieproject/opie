#include "wlan_NN.h"
#include "wlan_NNI.h"

static const char * WLanNeeds[] = 
    { 0
    };

/**
 * Constructor, find all of the possible interfaces
 */
WLanNetNode::WLanNetNode() : ANetNode() {
}

/**
 * Delete any interfaces that we own.
 */
WLanNetNode::~WLanNetNode(){
}

const QString WLanNetNode::nodeDescription(){
      return tr("\
<p>Configure Wi/Fi or WLan network cards.</p>\
<p>Defines Wireless options for those cards</p>\
"
);
}

ANetNodeInstance * WLanNetNode::createInstance( void ) {
      return new AWLan( this );
}

const char ** WLanNetNode::needs( void ) {
      return WLanNeeds;
}

const char * WLanNetNode::provides( void ) {
      return "device";
}

bool WLanNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool WLanNetNode::hasDataFor( const QString & S ) {
      return S == "interfaces";
}

bool WLanNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & S, 
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

QString WLanNetNode::genNic( long nr ) { 
      QString S; 
      return S.sprintf( "wlan%ld", nr );
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new WLanNetNode() );
}
}
