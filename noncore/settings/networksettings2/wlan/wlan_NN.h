#ifndef WLAN_NETNODE_H
#define WLAN_NETNODE_H

#include "netnode.h"

class AWLan;

class WLanNetNode : public ANetNode{

    Q_OBJECT

public:

    WLanNetNode();
    virtual ~WLanNetNode();

    virtual const QString pixmapName() 
      { return "Devices/wlan"; }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );

    virtual bool hasDataFor( const QString & S );
    virtual bool generateDeviceDataForCommonFile( 
        SystemFile & SF, long DevNr );

    virtual long instanceCount( void )
      { return InstanceCount; }

    virtual QString genNic( long );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

      // number of interfaces for this device
      long InstanceCount;

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
