
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
    for ( uint i = 0; i < buttonCount; ++i )
        ( void )buttonMaskImage( skinButtonInfo[ i ].fileName );
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
