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

bool WLanNetNode::hasDataFor( const QString & S, bool DS ) {
      return DS && S == "interfaces";
}

bool WLanNetNode::generateDataForCommonFile( 
                                SystemFile &, 
                                long ,
                                ANetNodeInstance * ) {
      return 1;
}

bool WLanNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & S, 
                                long DevNr,
                                ANetNodeInstance * NNI ) {
      return ((AWLan *)NNI)->generateDeviceDataForCommonFile(S, DevNr);
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new WLanNetNode() );
}
}
