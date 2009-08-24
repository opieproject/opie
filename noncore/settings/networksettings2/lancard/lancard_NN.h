#ifndef LANCARD_NETNODE_H
#define LANCARD_NETNODE_H

#include <qstringlist.h>
#include "netnode.h"

class ALanCard;

class LanCardNetNode : public ANetNode{

    Q_OBJECT

public:

    LanCardNetNode();
    virtual ~LanCardNetNode();

    virtual const QString pixmapName()
      { return "Devices/card"; }

    virtual QString genNic( long );
    virtual long instanceCount( void )
      { return InstanceCount; }

    virtual const QString nodeDescription() ;
    virtual ANetNodeInstance * createInstance( void );
    virtual const char ** needs( void );
    virtual const char ** provides( void );

    QStringList & addressesOfNIC( void )
      { return NICMACAddresses; }

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

      // number of interfaces for this device
      long InstanceCount;

      QStringList    NICMACAddresses;

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
