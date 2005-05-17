#include <opie2/oresource.h>

#include "kiconloader.h"

QPixmap KIconLoader::loadIcon( const QString &name, int )
{
    return Opie::Core::OResource::loadPixmap( "kate/" + name, Opie::Core::OResource::SmallIcon );
}

QString KIconLoader::iconPath( const QString &, int )
{
  return QString::null;
}

QPixmap BarIcon( const QString &name )
{
    return Opie::Core::OResource::loadPixmap( "kate/" + name, Opie::Core::OResource::SmallIcon );
}

QPixmap DesktopIcon( const QString &name, int )
{
    return Opie::Core::OResource::loadPixmap( "kate/" + name, Opie::Core::OResource::SmallIcon );
}

QPixmap SmallIcon( const QString &name )
{
    return Opie::Core::OResource::loadPixmap( "kate/" + name, Opie::Core::OResource::SmallIcon );
}

QPixmap SmallIconSet( const QString &name )
{
    return Opie::Core::OResource::loadPixmap( "kate/" + name, Opie::Core::OResource::SmallIcon );
}
