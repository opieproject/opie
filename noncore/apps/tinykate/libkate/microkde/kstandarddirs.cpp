#include "kdebug.h"

#include "kstandarddirs.h"

QString KStandardDirs::mAppDir = QString::null;

QString locate( const char *type, const QString& filename )
{
  QString path = KStandardDirs::appDir() + type + "_" + filename;

  kdDebug() << "locate: '" << path << "'" << endl;

  return path;
}

QString locateLocal( const char *type, const QString& filename )
{
  return locate( type, filename );
}

QStringList KStandardDirs::findAllResources( const QString &, const QString &, bool, bool)
{
  QStringList list;
  list.append("/cpp.xml");
  return list;
//  return QStringList();
}

QString KStandardDirs::findResourceDir( const QString &, const QString & )
{
  return QString::null;
}

void KStandardDirs::setAppDir( const QString &appDir )
{
  mAppDir = appDir;

  if ( mAppDir.right( 1 ) != "/" ) mAppDir += "/";
}
