#include "about.h"
#include "dialog.h"

#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qapplication.h>
#include <qlabel.h>

AboutApplet::AboutApplet ( )
        : QObject ( 0, "AboutApplet" )
{}

AboutApplet::~AboutApplet ( )
{}

int AboutApplet::position ( ) const
{
    return 1;
}

QString AboutApplet::name ( ) const
{
    return tr( "About shortcut" );
}

QString AboutApplet::text ( ) const
{
    return tr( "About" );
}

QString AboutApplet::tr( const char* s ) const
{
    return qApp->translate( "AboutApplet", s, 0 );
}

QString AboutApplet::tr( const char* s, const char* p ) const
{
    return qApp->translate( "AboutApplet", s, p );
}

QIconSet AboutApplet::icon ( ) const
{
    QPixmap pix;
    QImage img = Resource::loadImage ( "logo/opielogo" );

    if ( !img. isNull ( ) )
        pix. convertFromImage ( img. smoothScale ( 14, 14 ) );
    return pix;
}

QPopupMenu *AboutApplet::popup ( QWidget * ) const
{
    return 0;
}

void AboutApplet::activated()
{
    AboutDialog* d = new AboutDialog( qApp->mainWidget(), "aboutDialog", true );
    if ( qApp->desktop()->width() == 240 )
    {
        d->logo->hide();
        d->setFixedWidth( qApp->desktop()->width() - 5 );
        d->setFixedHeight( qApp->desktop()->height() - 50 );
    }
    d->exec();
}

QRESULT AboutApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
        * iface = this;
    else if ( uuid == IID_MenuApplet )
        * iface = this;
    else
        return QS_FALSE;

    if ( *iface )
        ( *iface ) -> addRef ( );
    return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
    Q_CREATE_INSTANCE( AboutApplet )
}


