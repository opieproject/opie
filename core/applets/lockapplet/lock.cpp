#include "lock.h"

/* OPIE */
#include <opie2/multiauthpassword.h>

#include <qpe/applnk.h>
#include <qpe/resource.h>

/* QT */
#include <qiconset.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>


LockMenuApplet::LockMenuApplet()
             :QObject( 0, "LockMenuApplet" )
{
}

LockMenuApplet::~LockMenuApplet ( )
{}

int LockMenuApplet::position() const
{
    return 3;
}

QString LockMenuApplet::name() const
{
    return tr( "Lock shortcut" );
}

QString LockMenuApplet::text() const
{
    return tr( "Lock" );
}


QIconSet LockMenuApplet::icon() const
{
    QPixmap pix;
    QImage img = Resource::loadImage( "security/lock" );
    if ( !img.isNull() )
        pix.convertFromImage( img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
    return pix;
}

QPopupMenu* LockMenuApplet::popup(QWidget*) const
{
    /* no subdir */
    return 0;
}

void LockMenuApplet::activated()
{
    /*
    QMessageBox::information(0,tr("No white rabbit found"),
			     tr("<qt>No white rabbit was seen near Opie."
			        "Only the beautiful OpieZilla is available"
				"for your pleassure</qt>"));
                */
    Opie::Security::MultiauthPassword::authenticate(Opie::Security::LockNow);
}


QRESULT LockMenuApplet::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
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
    Q_CREATE_INSTANCE( LockMenuApplet )
}
