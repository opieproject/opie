#ifndef MINIKDE_KSIMPLECONFIG_H
#define MINIKDE_KSIMPLECONFIG_H

#include "kconfig.h"

class KSimpleConfig : public KConfig
{
  public:
    KSimpleConfig( const QString &file ) : KConfig( file ) {}
};

#endif
