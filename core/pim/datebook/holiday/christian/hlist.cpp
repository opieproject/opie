#include "hlist.h"

#include <qobject.h>

const QStringList HList::entries(const QDate&aDate)const
{
    QStringList ret;
#if 0
    if (aDate.month()==12) {
        if (aDate.day()==24) {
            ret.append(QObject::tr("Christmas Eve","holidays"));
        } else if (aDate.day()==25) {
            ret.append(QObject::tr("Christmas Day","holidays"));
        } else if (aDate.day()==26) {
            ret.append(QObject::tr("Boxing Day","holidays"));
        }
    } else if (aDate.month()==1) {
        if (aDate.day()==1) {
            ret.append(QObject::tr("New Year","holidays"));
        }
    }
#endif
    return ret;
}
