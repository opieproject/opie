#include "modem_NN.h"
#include "modem_NNI.h"

#ifndef MYPLUGIN

#include "netnodeinterface.h"

#endif

static const char * ModemNeeds[] = 
    { "line",
      0
    };
static const char * ModemProvides[] = 
    { "modem",
      0
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

const char ** ModemNetNode::provides( void ) {
      return ModemProvides;
}

void ModemNetNode::setSpecificAttribute( QString & , QString & ) {
}

void ModemNetNode::saveSpecificAttribute( QTextStream & ) {
}

#ifdef MYPLUGIN 

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new ModemNetNode() );
}

#else

OPIE_NS2_PLUGIN( NetNodeInterface<ModemNetNode> )

#endif

}
