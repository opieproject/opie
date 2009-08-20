#include <resources.h>
#include "wlan_NN.h"
#include "wlan_NNI.h"

#include "netnodeinterface.h"

static const char * WLanNeeds[] =
    { 0
    };

static const char * WLanProvides[] =
    { "device",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
WLanNetNode::WLanNetNode() : ANetNode(tr("WLan Device")) {
    NSResources->addSystemFile(
      "interfaces", "/etc/network/interfaces", 1 );
    InstanceCount = 2;
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

const char ** WLanNetNode::provides( void ) {
      return WLanProvides;
}

QString WLanNetNode::genNic( long nr ) {
      QString S;
      return S.sprintf( "wlan%ld", nr );
}

void WLanNetNode::setSpecificAttribute( QString & A, QString & V ) {
      if( A == "interfacecount" ) {
        InstanceCount = V.toLong();
      }
}

void WLanNetNode::saveSpecificAttribute( QTextStream & TS ) {
      TS << "interfacecount="
         << InstanceCount
         << endl;
}

OPIE_NS2_PLUGIN( NetNodeInterface_T<WLanNetNode> )
