#ifndef MINIKDE_KRUN_H
#define MINIKDE_KRUN_H

#include <qstring.h>

class KRun
{
  public:
    static bool runCommand(const QString &a, const QString &b=QString::null,
                           const QString &c=QString::null);
};

#endif
