#include <qapplication.h>

#include "othemestyle.h"
#include "themeset.h"
#include "plugin.h"



ThemeInterface::ThemeInterface ( ) :  ref ( 0 )
{
	m_widget = 0;
}
    
ThemeInterface::~ThemeInterface ( )
{
}
        
QStyle *ThemeInterface::style ( )
{
	return new OThemeStyle ( "" );
}

QString ThemeInterface::name ( ) const
{
	return qApp-> translate ( "Styles", "Themed style" );
}

QString ThemeInterface::description ( ) const
{
	return qApp-> translate ( "Styles", "KDE2 theme compatible style engine" );
}

bool ThemeInterface::hasSettings ( ) const
{
	return true;
}

QWidget *ThemeInterface::create ( QWidget *parent, const char *name )
{
	m_widget = new ThemeSettings ( parent, name ? name : "THEME-SETTINGS" );
		
	return m_widget;
}

bool ThemeInterface::accept ( )
{
	if ( !m_widget )
		return false;

	return m_widget-> writeConfig ( );
}

void ThemeInterface::reject ( )
{
}


QRESULT ThemeInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
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
	Q_CREATE_INSTANCE( ThemeInterface )
}



