#ifndef PPP_NETNODE_H
#define PPP_NETNODE_H

#include "netnode.h"

class APPP;

class PPPNetNode : public ANetNode{

      Q_OBJECT

public:

      PPPNetNode();
      virtual ~PPPNetNode();

      virtual const QString pixmapName() 
        { return "Devices/ppp"; }

      virtual bool hasDataForFile( const QString & S );

      virtual const QString nodeDescription() ;
      virtual ANetNodeInstance * createInstance( void );
      virtual const char ** needs( void );
      virtual const char * provides( void );

      virtual QString genNic( long NicNr );
      virtual QStringList * properFiles( void );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

      static QStringList * ProperFiles;
};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
