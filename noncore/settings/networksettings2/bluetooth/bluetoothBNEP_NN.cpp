#include "bluetoothBNEP_NN.h"
#include "bluetoothBNEP_NNI.h"
#include "bluetoothRFCOMM_NNI.h"

#include "netnodeinterface.h"

//
//
// BLUETOOTH PAN/NAP node
//
//

static const char * BluetoothBNEPNeeds[] =
    { 0
    };

static const char * BluetoothBNEPProvides[] =
    { "device",
      0
    };

/**
 * Constructor, find all of the possible interfaces
 */
BluetoothBNEPNetNode::BluetoothBNEPNetNode() :
      ANetNode(tr("Bluetooth PAN/NAP")) {
      InstanceCount = 7; // default
}

/**
 * Delete any interfaces that we own.
 */
BluetoothBNEPNetNode::~BluetoothBNEPNetNode(){
}

const QString BluetoothBNEPNetNode::nodeDescription(){
      return tr("\
<p>Sets up a bluetooth link using the bluetooth Network profile.</p>\
<p>Use this to connect two computing devices.</p>\
"
);
}

ANetNodeInstance * BluetoothBNEPNetNode::createInstance( void ) {
      return new ABluetoothBNEP( this );
}

const char ** BluetoothBNEPNetNode::needs( void ) {
      return BluetoothBNEPNeeds;
}

const char ** BluetoothBNEPNetNode::provides( void ) {
      return BluetoothBNEPProvides;
}

QString BluetoothBNEPNetNode::genNic( long nr ) {
      QString S;
      return S.sprintf( "bnep%ld", nr );
}


void BluetoothBNEPNetNode::setSpecificAttribute( QString & A, QString & V ) {
      if( A == "interfacecount" ) {
        InstanceCount = V.toLong();
      }
}

void BluetoothBNEPNetNode::saveSpecificAttribute( QTextStream & TS) {
      TS << "interfacecount="
         << InstanceCount
         << endl;
}

typedef Opie::Core::MakeTypelist<BluetoothBNEPNetNode,  BluetoothRFCOMMNetNode>::Result BluetoothTypes;
OPIE_NS2_PLUGIN( NetNodeInterface_T<BluetoothTypes> )
