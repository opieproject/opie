#ifndef MINIKDE_KGLOBAL_H
#define MINIKDE_KGLOBAL_H

#include "klocale.h"
#include "kiconloader.h"
#include "kstandarddirs.h"
#include <kateconfig.h>

class KGlobal {
  public:
    static KLocale *locale();
    static KateConfig *config();
    static KIconLoader *iconLoader();
    static KStandardDirs *dirs();

    static void setAppName( const QString & );

  private:
    static KLocale *mLocale;
    static KateConfig *mConfig;
    static KIconLoader *mIconLoader;
    static KStandardDirs *mDirs;

    static QString mAppName;
};

#endif
