#include "example.h"

#include <qpe/applnk.h>
#include <qpe/resource.h>

/* QT */
#include <qiconset.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>


MenuAppletExample::MenuAppletExample()
             :QObject( 0, "MenuAppletExample" )
{
}

MenuAppletExample::~MenuAppletExample ( )
{}

int MenuAppletExample::position() const
{
    return 3;
}

QString MenuAppletExample::name() const
{
    return tr( "MenuApplet Example Name" );
}

QString MenuAppletExample::text() const
{
    return tr( "Click the white rabbit" );
}


QIconSet MenuAppletExample::icon() const
{
    QPixmap pix;
    QImage img = Resource::loadImage( "Tux" );
    if ( !img.isNull() )
        pix.convertFromImage( img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
    return pix;
}

QPopupMenu* MenuAppletExample::popup(QWidget*) const
{
    /* no subdir */
    return 0;
}

void MenuAppletExample::activated()
{
    QMessageBox::information(0,tr("No white rabbit found"),
			     tr("<qt>No white rabbit was seen near Opie."
			        "Only the beautiful OpieZilla is available"
				"for your pleassure</qt>"));
}


QRESULT MenuAppletExample::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
        *iface = this;
    else if ( uuid == IID_MenuApplet )
        *iface = this;
    else
	return QS_FALSE;

    if ( *iface )
        (*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( MenuAppletExample )
}
