#include "nationalholiday.h"
#include "nationalcfg.h"
#include "nationalcfgwidget.h"

#include <opie2/odebug.h>

#include <qobject.h>
#include <qpe/event.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

NationalHoliday::NationalHoliday()
    :Opie::Datebook::HolidayPlugin()
{
    _lastyear=0;
    init_done = false;
    init();
}

QString NationalHoliday::description()
{
    return QObject::tr("National holidays","holidays");
}

void NationalHoliday::init()
{
    Config cfg("nationaldays");
    cfg.setGroup("entries");
    files = cfg.readListEntry("files");
}

void NationalHoliday::load_days()
{
    if (init_done) return;
    Config cfg("nationaldays");
    cfg.setGroup("entries");
    QStringList::ConstIterator it;
    floatingDates.clear();
    NHcfg readit;
    for (it=files.begin();it!=files.end();++it) {
        if (!readit.load(QPEApplication::qpeDir()+"/etc/nationaldays/"+(*it))) {
            continue;
        }

        tentrylist::ConstIterator it;

        for (it=readit.fixDates().begin();it!=readit.fixDates().end();++it) {
            _days[(*it).date()]+=(*it).name();
        }
        floatingDates+=readit.floatingDates();
    }
    init_done = true;
}

void NationalHoliday::calc_easterDate()
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
  easterDate = QDate(_lastyear,n,p);
  odebug << "Easterdate = " << easterDate << oendl;
}


void NationalHoliday::setyear(const QDate&aDate)
{
    if (aDate.year()==_lastyear) return;
    odebug << "calc year" << oendl;
    _lastyear = aDate.year();
    _fdays.clear();
    calc_easterDate();
    tentrylist::ConstIterator it;
    QDate tempdate,t;

    for (it = floatingDates.begin();it!=floatingDates.end();++it) {
        odebug << "Float day " << (*it).name() << oendl;

        if ( (*it).date().year()==9999) {
            tempdate = QDate(easterDate.year(),easterDate.month(),easterDate.day());
        } else {
            tempdate = QDate(_lastyear,(*it).date().month(),(*it).date().day());
        }

        odebug << "Start calc date: " << tempdate << oendl;
        int weekday = weektonumber((*it).weekday());
        int mo;

        if (weekday>0) {
            /* specific weekday set */
            bool dir = true;
            if (!(*it).daydep().isEmpty()) {
                dir = ((*it).daydep()=="before"?true:false);
                tempdate = movedateday(tempdate,weekday,dir);
            } else if (!(*it).dayofmonth().isEmpty() &&
                (mo=monthtonumber((*it).month())) !=0 ) {
                t = QDate(_lastyear,mo,1);
                if ((*it).dayofmonth()=="last") {
                    int l = t.daysInMonth();
                    tempdate.setYMD(_lastyear,mo,l);
                    tempdate = movedateday(tempdate,weekday,true);
                } else {
                    tempdate = movedateday(t,weekday,false);
                    tempdate = tempdate.addDays(dayoftoint((*it).dayofmonth()));
                }
            }

        }
        tempdate = tempdate.addDays((*it).offset());
        odebug << "Moved date to " << tempdate << oendl;
        _fdays[tempdate]+=(*it).name();
    }
}

int NationalHoliday::dayoftoint(const QString&d)
{
    if (d=="first") return 0;
    else if (d=="second") return 7;
    else if (d=="third") return 14;
    else if (d=="fourth") return 21;
    return 0;
}

int NationalHoliday::monthtonumber(const QString&m)
{
    if (m=="january")return 1;
    else if (m=="february")return 2;
    else if (m=="march") return 3;
    else if (m=="april") return 4;
    else if (m=="may") return 5;
    else if (m=="june") return 6;
    else if (m=="july") return 7;
    else if (m=="august") return 8;
    else if (m=="september") return 9;
    else if (m=="october") return 10;
    else if (m=="november") return 11;
    else if (m=="december") return 12;
    return 0;
}

QDate NationalHoliday::movedateday(const QDate&start,int weekday,bool direction)
{
    QDate d = start;
    if (weekday==0) return d;
    int c = (direction?-1:1);

    while (d.dayOfWeek()!=weekday) {
        d = d.addDays(c);
    }
    return d;
}

int NationalHoliday::weektonumber(const QString&w)
{
    if (w=="monday") return 1;
    else if (w=="tuesday") return 2;
    else if (w=="wednesday") return 3;
    else if (w=="thursday" ) return 4;
    else if (w=="friday") return 5;
    else if (w=="saturday") return 6;
    else if (w=="sunday") return 7;
    return 0;
}

QStringList NationalHoliday::entries(const QDate&aDate)
{
    load_days();
    setyear(aDate);

    QStringList ret;
    QDate d(0,aDate.month(),aDate.day());

    tholidaylist::Iterator it = _days.find(d);
    if (it!=_days.end()) {
        ret+=*it;
    }
    QDate d2(_lastyear,d.month(),d.day());
    it = _fdays.find(d2);
    if (it!=_fdays.end()) {
        ret+=*it;
    }
    return ret;
}

QStringList NationalHoliday::entries(unsigned year, unsigned month, unsigned day)
{
    return entries(QDate(0,month,day));
}

QMap<QDate,QStringList> NationalHoliday::entries(const QDate&start,const QDate&end)
{
    load_days();
    setyear(start);
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

QValueList<EffectiveEvent> NationalHoliday::events(const QDate&start,const QDate&end)
{
    QValueList<EffectiveEvent> ret;
    QDate d = (start<end?start:end);
    int daysto = start.daysTo(end);
    if (daysto < 0) {
        daysto = end.daysTo(start);
    }

    QStringList temp;
    for (int i =0; i<=daysto;++i) {
        temp = entries(d.addDays(i));
        if (temp.count()==0) {
            continue;
        }
        for (unsigned j=0;j<temp.count();++j) {
            Event ev;
            ev.setDescription(temp[j]);
            ev.setStart(d.addDays(i));
            ev.setAllDay(true);
            ret.append(EffectiveEvent(ev,d.addDays(i)));
        }
    }

    return ret;
}

Opie::Datebook::HolidayPluginConfigWidget*NationalHoliday::configWidget(QWidget *parent,  const char *name, QWidget::WFlags fl)
{
    return new NationalHolidayConfigWidget(parent,name,fl);
}

EXPORT_HOLIDAY_PLUGIN(NationalHoliday);
