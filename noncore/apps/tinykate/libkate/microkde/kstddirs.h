#ifndef MINIKDE_KSTANDARDDIRS_H
#define MINIKDE_KSTANDARDDIRS_H

#include <qstring.h>
#include <qstringlist.h>

QString locate( const char *type, const QString& filename );
QString locateLocal( const char *type, const QString& filename );

class KStandardDirs
{
  public:
    QStringList findAllResources( const QString &, const QString &, bool, bool);
    QString findResourceDir( const QString &, const QString & ); 

    static void setAppDir( const QString & );
    static QString appDir() { return mAppDir; }
  
  private:
    static QString mAppDir;
};

#endif
