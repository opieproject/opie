#ifndef LANCARD_NETNODE_H
#define LANCARD_NETNODE_H

#include "netnode.h"

class ALanCard;

class LanCardNetNode : public ANetNode{

    Q_OBJECT

public:

    LanCardNetNode();
    virtual ~LanCardNetNode();

    virtual const QString pixmapName() 
      { return "card"; }

    virtual const QString nodeName() 
      { return tr("LAN card"); }

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
