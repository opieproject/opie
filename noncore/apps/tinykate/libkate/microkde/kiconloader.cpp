#include <qpe/resource.h>

#include "kiconloader.h"

QPixmap KIconLoader::loadIcon( const QString &name, int )
{
  return Resource::loadPixmap( "kate/" + name );
}

QString KIconLoader::iconPath( const QString &, int )
{
  return QString::null;
}

QPixmap BarIcon( const QString &name )
{
  return Resource::loadPixmap( "kate/" + name );
}

QPixmap DesktopIcon( const QString &name, int )
{
  return Resource::loadPixmap( "kate/" + name );
}

QPixmap SmallIcon( const QString &name )
{
  return Resource::loadPixmap( "kate/" + name );
}

QPixmap SmallIconSet( const QString &name )
{
  return Resource::loadPixmap( "kate/" + name );
}
