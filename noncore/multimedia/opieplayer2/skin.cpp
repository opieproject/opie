
#include "skin.h"

#include <qpe/resource.h>

Skin::Skin( const QString &name, const QString &fileNameInfix )
    : m_name( name ), m_fileNameInfix( fileNameInfix )
{
    m_skinPath = "opieplayer2/skins/" + name;
}

QImage Skin::backgroundImage() const
{
    return QImage( Resource::findPixmap( QString( "%1/background" ).arg( m_skinPath ) ) );
}

QImage Skin::buttonUpImage() const
{
    return QImage( Resource::findPixmap( QString( "%1/skin%2_up" ).arg( m_skinPath ).arg( m_fileNameInfix ) ) );
}

QImage Skin::buttonDownImage() const
{
    return QImage( Resource::findPixmap( QString( "%1/skin%2_down" ).arg( m_skinPath ).arg( m_fileNameInfix ) ) );
}

QImage Skin::buttonMaskImage( const QString &fileName ) const
{
    QString prefix = m_skinPath + QString::fromLatin1( "/skin%1_mask_" ).arg( m_fileNameInfix );
    QString path = prefix + fileName + ".png";
    return QImage( Resource::findPixmap( path ) );
}

/* vim: et sw=4 ts=4
 */
