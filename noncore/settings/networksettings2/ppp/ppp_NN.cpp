#include <qfile.h>
#include <qtextstream.h>
#include "ppp_NN.h"
#include "ppp_NNI.h"

QStringList * PPPNetNode::ProperFiles = 0;

static const char * PPPNeeds[] = 
    { "line", 
      "modem", 
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
PPPNetNode::PPPNetNode() : ANetNode(tr("PPP Connection")) {

      // proper files : will leak
      ProperFiles =new QStringList;
      *ProperFiles << "peers";
      *ProperFiles << "chatscript";

      // system files
      NSResources->addSystemFile( 
        "pap-secrets", "/tmp/pap-secrets", 0 );
      NSResources->addSystemFile( 
        "chap-secrets", "/tmp/chap-secrets", 0 );
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

const char * PPPNetNode::provides( void ) {
      return "connection";
}

QStringList * PPPNetNode::properFiles( void ) {
      return ProperFiles;

}

// need to generate :
//      /etc/ppp/pap-secrets
//      /etc/ppp/pap-secrets
bool PPPNetNode::hasDataForFile( const QString & S ) {
      return S == "pap-secrets" || 
             S == "chap-secrets" ;
}

QString PPPNetNode::genNic( long NicNr ) { 
      QString S; 
      return S.sprintf( "ppp%ld", NicNr ); 
}

void PPPNetNode::setSpecificAttribute( QString & , QString & ) {
}

void PPPNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new PPPNetNode() );
}
}
