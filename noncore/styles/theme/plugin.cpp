#include "othemestyle.h"
#include "themeset.h"
#include "plugin.h"



ThemeInterface::ThemeInterface ( ) :  ref ( 0 )
{
}
    
ThemeInterface::~ThemeInterface ( )
{
}
        
QStyle *ThemeInterface::create ( )
{
	return new OThemeStyle ( "" );
}

QString ThemeInterface::name ( )
{
	return QObject::tr( "Themed style", "name" );
}

QString ThemeInterface::description ( )
{
	return QObject::tr( "KDE2 theme compatible style engine", "description" );
}

QCString ThemeInterface::key ( )
{
	return QCString ( "theme" );
}

unsigned int ThemeInterface::version ( )
{
	return 100; // 1.0.0 (\d+.\d.\d)
}

QRESULT ThemeInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	static ThemeSettingsInterface *setiface = 0;

	*iface = 0;
	
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_Style )
		*iface = this;
	else if ( uuid == IID_StyleSettings ) {
		if ( !setiface )
			setiface = new ThemeSettingsInterface ( );
		*iface = setiface;
	}
	
	if ( *iface )
		(*iface)-> addRef ( );
		
	return QS_OK;
}

Q_EXPORT_INTERFACE()
{
	Q_CREATE_INSTANCE( ThemeInterface )
}


ThemeSettingsInterface::ThemeSettingsInterface ( ) :  ref ( 0 )
{
	m_widget = 0;
}
    
ThemeSettingsInterface::~ThemeSettingsInterface ( )
{
}
        
QWidget *ThemeSettingsInterface::create ( QWidget *parent, const char *name )
{
	m_widget = new ThemeSettings ( parent, name ? name : "THEME-SETTINGS" );
		
	return m_widget;
}

bool ThemeSettingsInterface::accept ( )
{
	if ( !m_widget )
		return false;

	return m_widget-> writeConfig ( );
}

void ThemeSettingsInterface::reject ( )
{
}

QRESULT ThemeSettingsInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
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

