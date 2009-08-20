#ifndef GPRS_NETNODE_H
#define GPRS_NETNODE_H

#include "netnode.h"

class ANetwork;

class GPRSNetNode : public ANetNode{

      Q_OBJECT

public:

      GPRSNetNode();
      virtual ~GPRSNetNode();

      virtual bool hasDataForFile( SystemFile & S );

      virtual short generateFile( SystemFile & Sf,
                                 ANetNodeInstance * NNI,
                                 long DevNr );
      virtual const QString pixmapName()
        { return "Devices/gprs"; }

      virtual QStringList properFiles( void );

      virtual const QString nodeDescription() ;
      virtual ANetNodeInstance * createInstance( void );
      virtual const char ** needs( void );
      virtual const char ** provides( void );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
