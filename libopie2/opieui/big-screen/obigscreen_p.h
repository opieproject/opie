#ifndef OPIE_BIG_SCREEN_PRIVATE
#define OPIE_BIG_SCREEN_PRIVATE

/* QT */
#include <qstring.h>

class QWidget;

namespace Opie {
namespace Ui  {
namespace Internal{

struct OSplitterContainer
{
    bool operator==( const OSplitterContainer& o) const
    {
        if (widget != o.widget ) return false;
        if (icon   != o.icon   ) return false;
        if (name   != o.name   ) return false;
        return true;
    }
    QWidget* widget;
    QString icon;
    QString name;
};

}
}
}

#endif
