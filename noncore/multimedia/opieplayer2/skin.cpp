
#include "skin.h"

#include <qpe/resource.h>

Skin::Skin( const QString &name, const QString &fileNameInfix )
    : m_name( name ), m_fileNameInfix( fileNameInfix )
{
    m_skinPath = "opieplayer2/skins/" + name;
}

void Skin::preload( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount )
{
    backgroundImage();
    buttonUpImage();
    buttonDownImage();
    ( void )buttonMask( skinButtonInfo, buttonCount );
}

QImage Skin::backgroundImage() const
{
    if ( m_backgroundImage.isNull() )
        m_backgroundImage = QImage( Resource::findPixmap( QString( "%1/background" ).arg( m_skinPath ) ) );
    return m_backgroundImage;
}

QImage Skin::buttonUpImage() const
{
    if ( m_buttonUpImage.isNull() )
        m_buttonUpImage = QImage( Resource::findPixmap( QString( "%1/skin%2_up" ).arg( m_skinPath ).arg( m_fileNameInfix ) ) );
    return m_buttonUpImage;
}

QImage Skin::buttonDownImage() const
{
    if ( m_buttonDownImage.isNull() )
        m_buttonDownImage = QImage( Resource::findPixmap( QString( "%1/skin%2_down" ).arg( m_skinPath ).arg( m_fileNameInfix ) ) );
    return m_buttonDownImage;
}

QImage Skin::buttonMask( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount ) const
{
    if ( !m_buttonMask.isNull() )
        return m_buttonMask;

    QSize buttonAreaSize = buttonUpImage().size();

    m_buttonMask = QImage( buttonAreaSize, 8, 255 );
    m_buttonMask.fill( 0 );

    for ( uint i = 0; i < buttonCount; ++i )
        addButtonToMask( skinButtonInfo[ i ].command + 1, buttonMaskImage( skinButtonInfo[ i ].fileName ) );

    return m_buttonMask;
}

void Skin::addButtonToMask( int tag, const QImage &maskImage ) const
{
    if ( maskImage.isNull() )
        return;

    uchar **dest = m_buttonMask.jumpTable();
    for ( int y = 0; y < m_buttonMask.height(); y++ ) {
        uchar *line = dest[y];
        for ( int x = 0; x < m_buttonMask.width(); x++ )
            if ( !qRed( maskImage.pixel( x, y ) ) )
                line[x] = tag;
    }
}

QImage Skin::buttonMaskImage( const QString &fileName ) const
{
    ButtonMaskImageMap::Iterator it = m_buttonMasks.find( fileName );
    if ( it == m_buttonMasks.end() ) {
        QString prefix = m_skinPath + QString::fromLatin1( "/skin%1_mask_" ).arg( m_fileNameInfix );
        QString path = prefix + fileName + ".png";
        it = m_buttonMasks.insert( fileName, QImage( Resource::findPixmap( path ) ) );
    }
    return *it;
}

/* vim: et sw=4 ts=4
 */
