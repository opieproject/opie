#ifndef VPN_NETNODE_H
#define VPN_NETNODE_H

#include "netnode.h"

class AVPN;

class VPNNetNode : public ANetNode{

    Q_OBJECT

public:

    VPNNetNode();
    virtual ~VPNNetNode();

    virtual const QString pixmapName() 
      { return "Devices/vpn"; }

    virtual const QString nodeName() 
      { return tr("VPN Connection"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S, bool DS );
    virtual bool generateDataForCommonFile( 
        SystemFile & SF, long DevNr, ANetNodeInstance * NNI );
    virtual bool generateDeviceDataForCommonFile( 
        SystemFile & SF, long DevNr, ANetNodeInstance * NNI );

private:

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
