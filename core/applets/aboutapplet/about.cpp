#include "about.h"
#include "widget.h"

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qlabel.h>

AboutApplet::AboutApplet ( )
        : QObject ( 0, "AboutApplet" ), m_ad( 0l )
{}

AboutApplet::~AboutApplet ( )
{
    delete m_ad;
}

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

QIconSet AboutApplet::icon() const
{
    QPixmap pix;
    QImage img = Resource::loadImage ( "logo/opielogo" );

    if ( !img.isNull() )
        pix.convertFromImage( img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
    return pix;
}

QPopupMenu *AboutApplet::popup ( QWidget * ) const
{
    return 0;
}

void AboutApplet::activated()
{
    if ( !m_ad )
        m_ad = new AboutWidget( 0, "aboutDialog" );

    QPEApplication::showWidget( m_ad );
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


