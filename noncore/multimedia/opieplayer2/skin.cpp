
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

/* vim: et sw=4 ts=4
 */
