#include "oholidayplugin2.h"

#include <opie2/opimtemplatebase.h>
#include <opie2/odebug.h>

using namespace Opie;
using namespace Opie::Datebook;

QMap<QDate,QStringList> HolidayPlugin2::entries(const QDate& start, const QDate& end)
{
    // This is the default implementation - if you can do it more efficiently in
    // your plugin, then override it.

    QMap<QDate,QStringList> ret;
    if (start==end) {
        ret[start]=entries(start);
        return ret;
    }
    QDate d;
    int daysto;
    if (end < start) {
        d = end;
        daysto = end.daysTo(start);
    } else {
        d = start;
        daysto = start.daysTo(end);
    }
    QStringList temp;
    for (int i=0;i<=daysto;++i) {
        temp = entries(d.addDays(i));
        if (temp.count()==0) continue;
        ret[d.addDays(i)]+=temp;
        temp.clear();
    }
    return ret;
}
