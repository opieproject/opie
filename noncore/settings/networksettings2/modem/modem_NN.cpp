#include "modem_NN.h"
#include "modem_NNI.h"

static const char * ModemNeeds[] = 
    { "line"
    };

/**
 * Constructor, find all of the possible interfaces
 */
ModemNetNode::ModemNetNode() : ANetNode(tr("Dialup modem")) {
}

/**
 * Delete any interfaces that we own.
 */
ModemNetNode::~ModemNetNode(){
}

const QString ModemNetNode::nodeDescription(){
      return tr("\
<p>Sets up a dialing procedures.</p>\
<p>Use this to dial up over modems, ISDN, GSM, ...</p>\
"
);
}

ANetNodeInstance * ModemNetNode::createInstance( void ) {
      return new AModem( this );
}

const char ** ModemNetNode::needs( void ) {
      return ModemNeeds;
}

const char * ModemNetNode::provides( void ) {
      return "line";
}

bool ModemNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool ModemNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 1;
}

void ModemNetNode::setSpecificAttribute( QString & , QString & ) {
}

void ModemNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new ModemNetNode() );
}
}
