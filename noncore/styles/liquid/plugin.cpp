#include <qapplication.h>

#include "liquid.h"
#include "liquidset.h"
#include "plugin.h"



LiquidInterface::LiquidInterface ( ) :  ref ( 0 )
{
	m_widget = 0;
}
    
LiquidInterface::~LiquidInterface ( )
{
}
        
QStyle *LiquidInterface::style ( )
{
	return new LiquidStyle ( );
}

QString LiquidInterface::name ( ) const
{
	return qApp-> translate ( "Styles", "Liquid" );
}

QString LiquidInterface::description ( ) const
{
	return qApp-> translate ( "Styles", "High Performance Liquid style by Mosfet" );
}

bool LiquidInterface::hasSettings ( ) const
{
	return true;
}

QWidget *LiquidInterface::create ( QWidget *parent, const char *name )
{
	m_widget = new LiquidSettings ( parent, name ? name : "LIQUID-SETTINGS" );
		
	return m_widget;
}

bool LiquidInterface::accept ( )
{
	if ( !m_widget )
		return false;

	return m_widget-> writeConfig ( );
}

void LiquidInterface::reject ( )
{
}


QRESULT LiquidInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_Style )
		*iface = this;
	else if ( uuid == IID_StyleExtended ) 
		*iface = this;
	
	if ( *iface )
		(*iface)-> addRef ( );
		
	return QS_OK;
}

Q_EXPORT_INTERFACE()
{
	Q_CREATE_INSTANCE( LiquidInterface )
}

