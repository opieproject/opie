#ifndef BLUETOOTHRFCOMM_NETNODE_H
#define BLUETOOTHRFCOMM_NETNODE_H

#include "netnode.h"

class BluetoothRFCOMMNetNode : public ANetNode {

      Q_OBJECT

public:

      BluetoothRFCOMMNetNode();
      virtual ~BluetoothRFCOMMNetNode();

      virtual const QString pixmapName() 
        { return "Devices/bluetooth"; }

      virtual const QString nodeDescription() ;
      virtual ANetNodeInstance * createInstance( void );
      virtual const char ** needs( void );
      virtual const char ** provides( void );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
};

#endif
