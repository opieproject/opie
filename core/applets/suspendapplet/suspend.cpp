#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qiconset.h>
#include <qpopupmenu.h>

#include "suspend.h"


SuspendApplet::SuspendApplet ( )
	: QObject ( 0, "SuspendApplet" ), ref ( 0 )
{
}

SuspendApplet::~SuspendApplet ( )
{
}

int SuspendApplet::position ( ) const
{
    return 2;
}

QString SuspendApplet::name ( ) const
{
	return tr( "Suspend shortcut" );
}

QString SuspendApplet::text ( ) const
{
	return tr( "Suspend" );
}

QIconSet SuspendApplet::icon ( ) const
{
	QPixmap pix;
	QImage img = Resource::loadImage ( "suspend" );
	
	if ( !img. isNull ( ))
		pix. convertFromImage ( img. smoothScale ( 14, 14 ));
	return pix;
}

QPopupMenu *SuspendApplet::popup ( QWidget * ) const
{
	return 0;
}

void SuspendApplet::activated ( )
{
	// suspend
	QCopEnvelope ( "QPE/System", "suspend()" );
}


QRESULT SuspendApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
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
	Q_CREATE_INSTANCE( SuspendApplet )
}


