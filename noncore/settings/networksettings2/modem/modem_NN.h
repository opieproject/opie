#ifndef MODEM_NETNODE_H
#define MODEM_NETNODE_H

#include "netnode.h"

class AModem;

class ModemNetNode : public ANetNode{

    Q_OBJECT

public:

    ModemNetNode();
    virtual ~ModemNetNode();

    virtual const QString pixmapName() 
      { return "Devices/modem"; }

    virtual const QString nodeName() 
      { return tr("Dialup modem"); }

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
