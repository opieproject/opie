#ifndef OPIE_FONTMANAGER_H
#define OPIE_FONTMANAGER_H

#include <qfont.h>

class FontManager {
public:
    enum Spacing { Fixed, Proportional };
    static bool hasUnicodeFont();
    static QFont unicodeFont( Spacing );
};

#endif
