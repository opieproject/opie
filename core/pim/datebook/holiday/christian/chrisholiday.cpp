#include "chrisholiday.h"

#include <qobject.h>

QString ChrisHoliday::description()
{
    return QObject::tr("Christian holidays","holidays");
}

QStringList ChrisHoliday::entries(const QDate&aDate)
{
    QStringList ret;
    ret+=_internallist.entries(aDate);
    calcit(aDate.year());
    tDayMap::Iterator it = _days.find(aDate);
    if (it!=_days.end()) {
        ret+=*it;
    }

    return ret;
}

QStringList ChrisHoliday::entries(unsigned year, unsigned month, unsigned day)
{
    return entries(QDate(year,month,day));
}

void ChrisHoliday::calcit(int year)
{
    if (year==_lastyear) return;
    _lastyear = year;
    _days.clear();
    calc_easter();
    calc_christmas();
}

void ChrisHoliday::calc_christmas()
{
    QDate cday(_lastyear,12,24);
    int diff;
    if ( (diff=cday.dayOfWeek())!=7) {
        cday=cday.addDays(-diff);
        _days[cday].append(QObject::tr("4. Advent","holidays"));
    }
    _days[cday.addDays(-7)].append(QObject::tr("3. Advent","holidays"));
    _days[cday.addDays(-14)].append(QObject::tr("2. Advent","holidays"));
    _days[cday.addDays(-21)].append(QObject::tr("1. Advent","holidays"));
}

void ChrisHoliday::calc_easter()
{
  unsigned long n = 0;
  unsigned long p = 0;

  if ( _lastyear > 1582 ) {
    unsigned long a = _lastyear%19;
    unsigned long b = _lastyear/100;
    unsigned long c = _lastyear%100;
    unsigned long d = b/4;
    unsigned long e = b%4;
    unsigned long f = (b+8)/25;
    unsigned long g = (b+f+1)/3;
    unsigned long h = (19*a+b-d-g+15)%30;
    unsigned long i = c/4;
    unsigned long j = c%4;
    unsigned long k = j%100;
    unsigned long l = (32+2*e+2*i-h-k)%7;
    unsigned long m = (a+11*h+22*l)/451;
    n = (h+l-7*m+114)/31;
    p = (h+l-7*m+114)%31;
  } else {
    unsigned long a = _lastyear%4;
    unsigned long b = _lastyear%7;
    unsigned long c = _lastyear%19;
    unsigned long d = (19*c+15)%30;
    unsigned long e = (2*a+4*b-d+34)%7;
    n = (d+e+114)/31;
    p = (d+e+114)%31;
  }
  p++;
  QDate d(_lastyear,n,p);
  _days[d].append(QObject::tr("Eastersunday","holidays"));
  _days[d.addDays(49)].append(QObject::tr("Whitsunday","holidays"));
  _days[d.addDays(50)].append(QObject::tr("Whitmonday","holidays"));
  _days[d.addDays(-46)].append(QObject::tr("Ash Wednesday","holidays"));
  _days[d.addDays(60)].append(QObject::tr("Corpus Christi","holidays"));
}
