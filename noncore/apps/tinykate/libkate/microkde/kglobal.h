#ifndef MINIKDE_KGLOBAL_H
#define MINIKDE_KGLOBAL_H

#include "klocale.h"
#include "kiconloader.h"
#include "kstandarddirs.h"
#include "kconfig.h"

class KGlobal {
  public:
    static KLocale *locale();
    static KConfig *config();
    static KIconLoader *iconLoader();
    static KStandardDirs *dirs();

    static void setAppName( const QString & );

  private:
    static KLocale *mLocale;
    static KConfig *mConfig;
    static KIconLoader *mIconLoader;
    static KStandardDirs *mDirs;

    static QString mAppName;
};

#endif
