#include "bluetoothRFCOMM_NN.h"
#include "bluetoothRFCOMM_NNI.h"

static const char * BluetoothRFCOMMNeeds[] =
    { 0
    };

static const char * BluetoothRFCOMMProvides[] =
    { "line",
      "GPRS"
    };

BluetoothRFCOMMNetNode::BluetoothRFCOMMNetNode() :
      ANetNode( tr("Bluetooth serial link") ) {
}

BluetoothRFCOMMNetNode::~BluetoothRFCOMMNetNode(){
}

const QString BluetoothRFCOMMNetNode::nodeDescription(){
      return tr("\
<p>Sets up a bluetooth link using the bluetooth serial profile.</p>\
<p>Use this to connect to a GSM.</p>\
"
);
}

ANetNodeInstance * BluetoothRFCOMMNetNode::createInstance( void ) {
      return new ABluetoothRFCOMM( this );
}

const char ** BluetoothRFCOMMNetNode::needs( void ) {
      return BluetoothRFCOMMNeeds;
}

const char ** BluetoothRFCOMMNetNode::provides( void ) {
      return BluetoothRFCOMMProvides;
}

void BluetoothRFCOMMNetNode::setSpecificAttribute( QString &, QString & ) {
}

void BluetoothRFCOMMNetNode::saveSpecificAttribute( QTextStream & ) {
}

// create plugin -> see BNEP
