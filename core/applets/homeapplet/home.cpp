#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qiconset.h>
#include <qpopupmenu.h>

#include "home.h"


HomeApplet::HomeApplet ( )
	: QObject ( 0, "HomeApplet" ), ref ( 0 )
{
}

HomeApplet::~HomeApplet ( )
{
}

int HomeApplet::position ( ) const
{
    return 4;
}

QString HomeApplet::name ( ) const
{
	return tr( "Home shortcut" );
}

QString HomeApplet::text ( ) const
{
	return tr( "Home" );
}

QIconSet HomeApplet::icon ( ) const
{
	QPixmap pix;
	QImage img = Resource::loadImage ( "home" );
	
	if ( !img. isNull ( ))
		pix. convertFromImage ( img. smoothScale ( 14, 14 ));
	return pix;
}

QPopupMenu *HomeApplet::popup ( QWidget * ) const
{
	return 0;
}

void HomeApplet::activated ( )
{
	// to desktop (home)
	QCopEnvelope ( "QPE/Desktop", "home()" );
}


QRESULT HomeApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_MenuApplet )
		*iface = this;

	if ( *iface )
		(*iface)-> addRef ( );
	return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
	Q_CREATE_INSTANCE( HomeApplet )
}


