#include "vpn_NN.h"
#include "vpn_NNI.h"

#include "netnodeinterface.h"

static const char * VPNNeeds[] = 
    { 0
    };

static const char * VPNProvides[] = 
    { "connection",
      0
    };
/**
 * Constructor, find all of the possible interfaces
 */
VPNNetNode::VPNNetNode() : ANetNode(tr("VPN connection")) {
}

/**
 * Delete any interfaces that we own.
 */
VPNNetNode::~VPNNetNode(){
}

const QString VPNNetNode::nodeDescription(){
      return tr("\
<p>Configure private IP connection.</p>\
<p>Defines Secure tunnels over non secure IP sessions</p>\
"
);
}

ANetNodeInstance * VPNNetNode::createInstance( void ) {
      return new AVPN( this );
}

const char ** VPNNetNode::needs( void ) {
      return VPNNeeds;
}

const char ** VPNNetNode::provides( void ) {
      return VPNProvides;
}

void VPNNetNode::setSpecificAttribute( QString & , QString & ) {
}

void VPNNetNode::saveSpecificAttribute( QTextStream & ) {
}

OPIE_NS2_PLUGIN(  NetNodeInterface_T<VPNNetNode> )
