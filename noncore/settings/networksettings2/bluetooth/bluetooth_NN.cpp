#include "bluetooth_NN.h"
#include "bluetoothBNEP_NNI.h"
#include "bluetoothRFCOMM_NNI.h"

//
//
// BLUETOOTH PAN/NAP node
//
//

static const char * BluetoothBNEPNeeds[] = 
    { 0
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

const char * BluetoothBNEPNetNode::provides( void ) {
      return "device";
}

bool BluetoothBNEPNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 1;
}

bool BluetoothBNEPNetNode::hasDataFor( const QString & S ) {
      return S == "interfaces";
}

bool BluetoothBNEPNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 1;
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

//
//
// BLUETOOTH RFCOMM
//
//

static const char * BluetoothRFCOMMNeeds[] = 
    { 0
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

const char * BluetoothRFCOMMNetNode::provides( void ) {
      return "line";
}

bool BluetoothRFCOMMNetNode::generateProperFilesFor( 
            ANetNodeInstance * ) {
      return 0;
}

bool BluetoothRFCOMMNetNode::hasDataFor( const QString &  ) {
      return 0;
}

bool BluetoothRFCOMMNetNode::generateDeviceDataForCommonFile( 
                                SystemFile & , 
                                long ) {
      return 0;
}

void BluetoothRFCOMMNetNode::setSpecificAttribute( QString &, QString & ) {
}

void BluetoothRFCOMMNetNode::saveSpecificAttribute( QTextStream & ) {
}

extern "C" {
void create_plugin( QList<ANetNode> & PNN ) {
      PNN.append( new BluetoothBNEPNetNode() );
      PNN.append( new BluetoothRFCOMMNetNode() );
}
}
