#ifndef PROFILE_NETNODE_H
#define PROFILE_NETNODE_H

#include "netnode.h"

class AProfile;

class ProfileNetNode : public ANetNode{

    Q_OBJECT

public:

    ProfileNetNode();
    virtual ~ProfileNetNode();

    virtual const QString pixmapName() 
      { return "Devices/commprofile"; }

    virtual const QString nodeName() 
      { return tr("Regular profile"); }

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
