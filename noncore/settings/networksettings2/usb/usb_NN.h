#ifndef USB_NETNODE_H
#define USB_NETNODE_H

#include "netnode.h"

class AUSB;

class USBNetNode : public ANetNode{

    Q_OBJECT

public:

    USBNetNode();
    virtual ~USBNetNode();

    virtual const QString pixmapName() 
      { return "Devices/usb"; }

    virtual const QString nodeName() 
      { return tr("USB Cable Connect"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S, bool DeviceSpec );
    virtual bool generateDataForCommonFile( 
        SystemFile & SF, long DevNr, ANetNodeInstance * NNI );
    virtual bool generateDeviceDataForCommonFile( 
        SystemFile & SF, long DevNr, ANetNodeInstance * NNI );

private:

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
