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
      { return "wlan"; }

    virtual const QString nodeName() 
      { return tr("WLan Device"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S );
    virtual bool generateDataForCommonFile( 
        SystemFile & SF, long DevNr, ANetNodeInstance * NNI );

private:

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
