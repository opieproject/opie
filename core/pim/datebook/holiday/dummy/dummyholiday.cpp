#include "dummyholiday.h"

#include <qobject.h>

QString DummyHoliday::description()
{
    return QObject::tr("Test holiday plugin","dummyholiday");
}

QStringList DummyHoliday::entries(const QDate&aDate)
{
    return entries(0,0,aDate.day());
}

QStringList DummyHoliday::entries(unsigned year, unsigned month, unsigned day)
{
    QStringList ret;
    if (day%2==0) ret.append(QObject::tr("You have a holiday!","dummyholiday"));
    return ret;
}

QMap<QDate,QStringList> DummyHoliday::entries(const QDate&,const QDate&)
{
    QMap<QDate,QStringList> ret;
    return ret;
}

EXPORT_HOLIDAY_PLUGIN(DummyHoliday);
