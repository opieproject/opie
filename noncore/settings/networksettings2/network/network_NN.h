#ifndef NETWORK_NETNODE_H
#define NETWORK_NETNODE_H

#include "netnode.h"

class ANetwork;

class NetworkNetNode : public ANetNode{

      Q_OBJECT

public:

      NetworkNetNode();
      virtual ~NetworkNetNode();

      virtual bool hasDataForFile( const QString & S );

      virtual short generateFile( const QString & ID,
                                 const QString & Path,
                                 QTextStream & TS, 
                                 ANetNodeInstance * NNI,
                                 long DevNr );
      virtual const QString pixmapName() 
        { return "Devices/tcpip"; }

      virtual const QString nodeDescription() ;
      virtual ANetNodeInstance * createInstance( void );
      virtual const char ** needs( void );
      virtual const char * provides( void );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
