#include "liquid.h"
#include "liquidset.h"
#include "plugin.h"



LiquidInterface::LiquidInterface ( ) :  ref ( 0 )
{
}
    
LiquidInterface::~LiquidInterface ( )
{
}
        
QStyle *LiquidInterface::create ( )
{
	return new LiquidStyle ( );
}

QString LiquidInterface::name ( )
{
	return QObject::tr( "Liquid", "name" );
}

QString LiquidInterface::description ( )
{
	return QObject::tr( "High Performance Liquid style by Mosfet", "description" );
}

QCString LiquidInterface::key ( )
{
	return QCString ( "liquid" );
}

unsigned int LiquidInterface::version ( )
{
	return 100; // 1.0.0 (\d+.\d.\d)
}

QRESULT LiquidInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	static LiquidSettingsInterface *setiface = 0;

	*iface = 0;
	
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_Style )
		*iface = this;
	else if ( uuid == IID_StyleSettings ) {
		if ( !setiface )
			setiface = new LiquidSettingsInterface ( );
		*iface = setiface;
	}
	
	if ( *iface )
		(*iface)-> addRef ( );
		
	return QS_OK;
}

Q_EXPORT_INTERFACE()
{
	Q_CREATE_INSTANCE( LiquidInterface )
}


LiquidSettingsInterface::LiquidSettingsInterface ( ) :  ref ( 0 )
{
	m_widget = 0;
}
    
LiquidSettingsInterface::~LiquidSettingsInterface ( )
{
}
        
QWidget *LiquidSettingsInterface::create ( QWidget *parent, const char *name )
{
	m_widget = new LiquidSettings ( parent, name ? name : "LIQUID-SETTINGS" );
		
	return m_widget;
}

bool LiquidSettingsInterface::accept ( )
{
	if ( !m_widget )
		return false;

	return m_widget-> writeConfig ( );
}

void LiquidSettingsInterface::reject ( )
{
}

QRESULT LiquidSettingsInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	
	
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_StyleSettings )
		*iface = this;
	
	if ( *iface )
		(*iface)-> addRef ( );
		
	return QS_OK;
}

