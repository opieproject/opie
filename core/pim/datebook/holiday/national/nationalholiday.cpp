#include "nationalholiday.h"
#include "nationalcfg.h"

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
    odebug << "Read " << files << oendl;
}

void NationalHoliday::load_days()
{
    if (init_done) return;
    QStringList::ConstIterator it;
    NHcfg readit;
    for (it=files.begin();it!=files.end();++it) {
        odebug << QPEApplication::qpeDir()+"/etc/nationaldays/"+(*it) << oendl;
        if (!readit.load(QPEApplication::qpeDir()+"/etc/nationaldays/"+(*it)))
            continue;
        tholidaylist::ConstIterator it;
        for (it=readit.days().begin();it!=readit.days().end();++it) {
            _days[it.key()]+=(it.data());
        }

    }
    init_done = true;
}

QStringList NationalHoliday::entries(const QDate&aDate)
{
    load_days();
    QStringList ret;
    QDate d(0,aDate.month(),aDate.day());

    tholidaylist::Iterator it = _days.find(d);
    if (it!=_days.end()) {
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

EXPORT_HOLIDAY_PLUGIN(NationalHoliday);
