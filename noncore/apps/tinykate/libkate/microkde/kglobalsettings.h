#ifndef MICROKDE_KGLOBALSETTINGS_H
#define MICROKDE_KGLOBALSETTINGS_H

#include <qfont.h>
#include <qcolor.h>

class KGlobalSettings
{
  public:
    static QFont generalFont();
    static QColor baseColor();
    static QColor highlightColor();
};

#endif
