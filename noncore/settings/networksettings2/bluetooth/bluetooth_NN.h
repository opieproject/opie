#ifndef BLUETOOTH_NETNODE_H
#define BLUETOOTH_NETNODE_H

#include "netnode.h"

class ABluetoothBNEP;

class BluetoothBNEPNetNode : public ANetNode {

    Q_OBJECT

public:

    BluetoothBNEPNetNode();
    virtual ~BluetoothBNEPNetNode();

    virtual const QString pixmapName() 
      { return "Devices/bluetooth"; }

    virtual const QString nodeName() 
      { return tr("Bluetooth PAN/NAP"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S );
    virtual bool generateDeviceDataForCommonFile( 
        SystemFile & , long DevNr );

    virtual long instanceCount( void )
      { return InstanceCount; }
    virtual QString genNic( long );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

      // number of interfaces for this device
      long InstanceCount;

};

class BluetoothRFCOMMNetNode : public ANetNode {

    Q_OBJECT

public:

    BluetoothRFCOMMNetNode();
    virtual ~BluetoothRFCOMMNetNode();

    virtual const QString pixmapName() 
      { return "Devices/bluetooth"; }

    virtual const QString nodeName() 
      { return tr("Bluetooth serial link"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S );
    virtual bool generateDeviceDataForCommonFile( 
        SystemFile & , long );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
