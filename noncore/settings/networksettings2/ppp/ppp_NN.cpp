#include <qfile.h>
#include <resources.h>
#include <qtextstream.h>
#include "ppp_NN.h"
#include "ppp_NNI.h"

#include "netnodeinterface.h"

QStringList * PPPNetNode::ProperFiles = 0;

static const char * PPPNeeds[] = 
    { "modem", 
      0
    };

static const char * PPPProvides[] = 
    { "connection", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
PPPNetNode::PPPNetNode() : ANetNode(tr("PPP Connection")) {

      NSResources->addSystemFile( 
        "pap-secrets", "/etc/ppp/pap-secrets", 0 );
      NSResources->addSystemFile( 
        "chap-secrets", "/etc/ppp/chap-secrets", 0 );
}

/**
 * Delete any interfaces that we own.
 */
PPPNetNode::~PPPNetNode(){
}

const QString PPPNetNode::nodeDescription(){
      return tr("\
<p>Sets up IP using PPP.</p>\
<p>Use this for dialup devices or serial setups</p>\
"
);
}

ANetNodeInstance * PPPNetNode::createInstance( void ) {
      return new APPP( this );
}

const char ** PPPNetNode::needs( void ) {
      return PPPNeeds;
}

const char ** PPPNetNode::provides( void ) {
      return PPPProvides;
}

QStringList PPPNetNode::properFiles( void ) {
      QStringList SL;
      SL << "peers" << "chatscripts"; 
      return SL;
}

// need to generate :
//      /etc/ppp/pap-secrets
//      /etc/ppp/pap-secrets
bool PPPNetNode::hasDataForFile( SystemFile & S ) {
      return S.name() == "pap-secrets" || 
             S.name() == "chap-secrets" ;
}

QString PPPNetNode::genNic( long NicNr ) { 
      QString S; 
      return S.sprintf( "ppp%ld", NicNr ); 
}

void PPPNetNode::setSpecificAttribute( QString & , QString & ) {
}

void PPPNetNode::saveSpecificAttribute( QTextStream & ) {
}

OPIE_NS2_PLUGIN( NetNodeInterface_T<PPPNetNode> )
