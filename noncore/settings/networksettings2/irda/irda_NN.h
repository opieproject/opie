#ifndef IRDA_NETNODE_H
#define IRDA_NETNODE_H

#include "netnode.h"

class AIRDA;

class IRDANetNode : public ANetNode {

    Q_OBJECT

public:

    IRDANetNode();
    virtual ~IRDANetNode();

    virtual const QString pixmapName() 
      { return "Devices/irda"; }

    virtual const QString nodeName() 
      { return tr("Infrared link"); }

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
