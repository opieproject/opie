#include "metal.h"
#include "plugin.h"


MetalInterface::MetalInterface ( ) :  ref ( 0 )
{
}
    
MetalInterface::~MetalInterface ( )
{
}
        
QStyle *MetalInterface::create ( )
{
	return new MetalStyle ( );
}

QString MetalInterface::name ( )
{
	return QObject::tr( "Metal", "name" );
}

QString MetalInterface::description ( )
{
	return QObject::tr( "Metal style", "description" );
}

QCString MetalInterface::key ( )
{
	return QCString ( "metal" );
}

unsigned int MetalInterface::version ( )
{
	return 100; // 1.0.0 (\d+.\d.\d)
}

QRESULT MetalInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_Style )
		*iface = this;
	
	if ( *iface )
		(*iface)-> addRef ( );
		
	return QS_OK;
}

Q_EXPORT_INTERFACE()
{
	Q_CREATE_INSTANCE( MetalInterface )
}

