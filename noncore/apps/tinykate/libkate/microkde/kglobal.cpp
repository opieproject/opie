#include "kglobal.h"

KLocale *KGlobal::mLocale = 0;
KConfig *KGlobal::mConfig = 0;
KIconLoader *KGlobal::mIconLoader = 0;
KStandardDirs *KGlobal::mDirs = 0;

QString KGlobal::mAppName = "godot";

KLocale *KGlobal::locale()
{
  if ( !mLocale ) {
    mLocale = new KLocale();
  }
  
  return mLocale;
}

KConfig *KGlobal::config()
{
  if ( !mConfig ) {
    mConfig = new KConfig( KStandardDirs::appDir() + mAppName + "rc" );
  }
  
  return mConfig;
}

KIconLoader *KGlobal::iconLoader()
{
  if ( !mIconLoader ) {
    mIconLoader = new KIconLoader();
  }
  
  return mIconLoader;
}

KStandardDirs *KGlobal::dirs()
{
  if ( !mDirs ) {
    mDirs = new KStandardDirs();
  }
  
  return mDirs;
}

void KGlobal::setAppName( const QString &appName )
{
  mAppName = appName;
}
