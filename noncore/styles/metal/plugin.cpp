#include <qapplication.h>

#include "metal.h"
#include "plugin.h"


MetalInterface::MetalInterface ( ) 
{
}
    
MetalInterface::~MetalInterface ( )
{
}
        
QStyle *MetalInterface::style ( )
{
	return new MetalStyle ( );
}

QString MetalInterface::name ( ) const
{
	return qApp-> translate ( "Styles", "Metal" );
}


QRESULT MetalInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_Style )
		*iface = this;
	else
	    return QS_FALSE;
	
	if ( *iface )
		(*iface)-> addRef ( );
		
	return QS_OK;
}

Q_EXPORT_INTERFACE()
{
	Q_CREATE_INSTANCE( MetalInterface )
}

