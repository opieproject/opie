#ifndef CABLE_NETNODE_H
#define CABLE_NETNODE_H

#include "netnode.h"

class ACable;

class CableNetNode : public ANetNode {

    Q_OBJECT

public:

    CableNetNode();
    virtual ~CableNetNode();

    virtual const QString pixmapName() 
      { return "cable"; }

    virtual const QString nodeName() 
      { return tr("Cable Connection"); }

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
