#include "birthday.h"

#include <opie2/ocontactaccess.h>
#include <qpe/event.h>

#include <qobject.h>

Birthday::Birthday()
    :Opie::Datebook::HolidayPlugin()
{
    m_contactdb = new Opie::OPimContactAccess("addressplugin");
/*
    connect( m_contactdb, SIGNAL( signalChanged(const Opie::OPimContactAccess*) ),
         this, SLOT( refresh(const Opie::OPimContactAccess*) ) );
*/
}

QString Birthday::description()
{
    return QObject::tr("Birthdays","holidays");
}

QStringList Birthday::entries(const QDate&aDate)
{
    QStringList ret;
    if ( m_contactdb->hasQuerySettings( Opie::OPimContactAccess::DateDay ) ){
        Opie::OPimContact querybirthdays,queryanniversary;
        QString pre;
        querybirthdays.setBirthday(aDate);
        queryanniversary.setAnniversary(aDate);
        m_list = m_contactdb->queryByExample( querybirthdays,Opie::OPimContactAccess::DateDay| Opie::OPimContactAccess::DateMonth);
        if ( m_list.count() > 0 ){
            pre = QObject::tr("Birthday","holidays")+" ";
            int z = 0;
            for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
                if (z) {
                    pre+=", ";
                }
                pre+=((*m_it).fullName());
                ++z;
            }
            ret.append(pre);
        }
        m_list = m_contactdb->queryByExample( queryanniversary,Opie::OPimContactAccess::DateDay| Opie::OPimContactAccess::DateMonth);
        if ( m_list.count() > 0 ){
            pre = QObject::tr("Anniversary","holidays")+" ";
            int z = 0;
            for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
                if (z) {
                    pre+=", ";
                }
                pre+=((*m_it).fullName());
                ++z;
            }
            ret.append(pre);
        }
    }
    return ret;
}

QStringList Birthday::entries(unsigned year, unsigned month, unsigned day)
{
    return entries(QDate(year,month,day));
}

QMap<QDate,QString> Birthday::_entries(const QDate&start,const QDate&end,bool anniversary)
{
    QMap<QDate,QString> ret;
    QDate s = (start<end?start:end);
    QDate e = (start<end?end:start);

    int daysto = start.daysTo(end);
    if (daysto < 0) {
        daysto = end.daysTo(start);
    }
    if ( m_contactdb->hasQuerySettings(Opie::OPimContactAccess::DateDiff ) ){
        Opie::OPimContact querybirthdays;
        QString pre;
        if (anniversary) {
            querybirthdays.setAnniversary(e);
        } else {
            querybirthdays.setBirthday(e);
        }
        QMap<QDate,QString> collector;
        QMap<QDate,QString>::ConstIterator sit;
        m_list = m_contactdb->queryByExample( querybirthdays,Opie::OPimContactAccess::DateDiff,s);
        QDate t;
        if ( m_list.count() > 0 ){
            if (anniversary) {
                pre = QObject::tr("Anniversary","holidays");
            } else {
                pre = QObject::tr("Birthday","holidays");
            }
            for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
                if (!anniversary) {
                    t.setYMD(e.year(),(*m_it).birthday().month(),(*m_it).birthday().day());
                } else {
                    t.setYMD(e.year(),(*m_it).anniversary().month(),(*m_it).anniversary().day());
                }
                if (ret[t].isEmpty()) {
                    ret[t]=pre;
                }
                ret[t]+=" "+(*m_it).fullName();
            }
        }
    }
    return ret;
}

QMap<QDate,QStringList> Birthday::entries(const QDate&start,const QDate&end)
{
    QMap<QDate,QStringList> ret;
    QMap<QDate,QString> collector;
    QMap<QDate,QString>::ConstIterator sit;

    collector = _entries(start,end,false);
    for (sit=collector.begin();sit!=collector.end();++sit) {
        ret[sit.key()].append(sit.data());
    }
    collector = _entries(start,end,true);
    for (sit=collector.begin();sit!=collector.end();++sit) {
        ret[sit.key()].append(sit.data());
    }
    return ret;
}

QValueList<EffectiveEvent> Birthday::events(const QDate&start,const QDate&end)
{
    QValueList<EffectiveEvent> ret;
    QMap<QDate,QString> collector;
    QMap<QDate,QString>::ConstIterator sit;

    collector = _entries(start,end,false);

    for (sit=collector.begin();sit!=collector.end();++sit) {
        Event ev;
        ev.setAllDay(true);
        ev.setStart(sit.key());
        ev.setEnd(sit.key());
        ev.setDescription(sit.data());
        odebug << sit.key() << oendl;
        ret.append(EffectiveEvent(ev,sit.key()));
    }

    collector = _entries(start,end,true);

    for (sit=collector.begin();sit!=collector.end();++sit) {
        Event ev;
        ev.setAllDay(true);
        ev.setStart(sit.key());
        ev.setEnd(sit.key());
        ev.setDescription(sit.data());
        odebug << sit.key() << oendl;
        ret.append(EffectiveEvent(ev,sit.key()));
    }
    return ret;
}

EXPORT_HOLIDAY_PLUGIN(Birthday);
