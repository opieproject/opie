#include "hlist.h"

#include <qobject.h>

const QStringList HList::entries(const QDate&aDate)const
{
    QStringList ret;
    if (aDate.month()==12) {
        if (aDate.day()==24) {
            ret.append(QObject::tr("Christmas","holidays"));
        }
    } else if (aDate.month()==1) {
        if (aDate.day()==1) {
            ret.append(QObject::tr("New Year","holidays"));
        }
    }
    return ret;
}
