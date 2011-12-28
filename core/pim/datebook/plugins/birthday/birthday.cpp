#include "birthday.h"

#include <opie2/ocontactaccess.h>

#include <qobject.h>

Birthday::Birthday()
    :Opie::Datebook::HolidayPlugin()
{
    m_contactdb = new Opie::OPimContactAccess("addressplugin");
    m_contactdb->load();
/*
    connect( m_contactdb, SIGNAL( signalChanged(const Opie::OPimContactAccess*) ),
         this, SLOT( refresh(const Opie::OPimContactAccess*) ) );
*/
    // Get owner contact in case that has an anniversary/birthday
    owner = Opie::OPimContactAccess::businessCard();
}

QString Birthday::description()
{
    return QObject::tr("Birthdays & Anniversaries","holidays");
}

QStringList Birthday::entries(const QDate&aDate)
{
    QStringList ret;
    QStringList items;
    Opie::OPimContactAccess::List list;
    Opie::OPimContactAccess::List::Iterator it;

    if ( m_contactdb->hasQuerySettings( Opie::OPimContactAccess::DateDay ) ){
        Opie::OPimContact querybirthdays,queryanniversary;
        QString pre;
        querybirthdays.setBirthday(aDate);
        queryanniversary.setAnniversary(aDate);

        // Birthdays
        list = m_contactdb->queryByExample( querybirthdays,Opie::OPimContactAccess::DateDay | Opie::OPimContactAccess::DateMonth );
        for( it = list.begin(); it != list.end(); ++it )
            items += (*it).fullName();
        if( !owner.birthday().isNull() && owner.birthday().dayOfYear() == aDate.dayOfYear() )
            items += owner.fullName();
        if( items.count() > 0 )
            ret.append( QObject::tr("Birthday","holidays") + ": " + items.join(", ") );

        // Anniversaries
        items.clear();
        list = m_contactdb->queryByExample( queryanniversary, Opie::OPimContactAccess::DateDay | Opie::OPimContactAccess::DateMonth );
        for( it = list.begin(); it != list.end(); ++it )
            items += (*it).fullName();
        if( !owner.anniversary().isNull() && owner.anniversary().dayOfYear() == aDate.dayOfYear() )
            items += owner.fullName();
        if( items.count() > 0 )
            ret.append( QObject::tr("Anniversary","holidays") + ": " + items.join(", ") );
    }
    return ret;
}

QStringList Birthday::entries(unsigned year, unsigned month, unsigned day)
{
    return entries(QDate(year,month,day));
}

void Birthday::addMapEntry( QMap<QDate, QString> &map, const QDate &date, const QDate &start, const QDate &end, const QString &prefix, const QString &name )
{
    int year;
    if( start.month() > end.month() ) {
        // Crossing a year boundary (Dec -> Jan)
        if( date.month() == end.month() )
            year = end.year();
        else
            year = start.year();
    }
    else
        year = end.year();
    QDate t(year, date.month(), date.day());

    if( map[t].isEmpty() )
        map[t] = prefix + name;
    else
        map[t] += ", " + name;
}

QMap<QDate,QString> Birthday::_entries(const QDate&start,const QDate&end,bool anniversary)
{
    QMap<QDate,QString> ret;
    QDate s = (start<end?start:end);
    QDate e = (start<end?end:start);
    Opie::OPimContactAccess::List list;
    Opie::OPimContactAccess::List::Iterator it;

    if ( m_contactdb->hasQuerySettings(Opie::OPimContactAccess::DateDiff ) ){
        Opie::OPimContact querybirthdays;
        if( anniversary )
            querybirthdays.setAnniversary(e);
        else
            querybirthdays.setBirthday(e);

        QString pre;
        if (anniversary)
            pre = QObject::tr("Anniversary","holidays") + ": ";
        else
            pre = QObject::tr("Birthday","holidays") + ": ";

        QDate t;
        list = m_contactdb->queryByExample( querybirthdays, Opie::OPimContactAccess::DateDiff, s );
        for ( it = list.begin(); it != list.end(); ++it ) {
            if( anniversary )
                t = (*it).anniversary();
            else
                t = (*it).birthday();
            addMapEntry( ret, t, s, e, pre, (*it).fullName() );
        }

        // Check owner contact
        if( anniversary )
            t = owner.anniversary();
        else
            t = owner.birthday();
        if( !t.isNull() && t.dayOfYear() >= s.dayOfYear() && t.dayOfYear() <= e.dayOfYear() )
            addMapEntry( ret, t, s, e, pre, owner.fullName() );
    }
    return ret;
}

QMap<QDate,QStringList> Birthday::entries(const QDate&start,const QDate&end)
{
    QMap<QDate,QStringList> ret;
    QMap<QDate,QString> collector;
    QMap<QDate,QString>::ConstIterator sit;

    // Birthdays
    collector = _entries(start,end,false);
    for (sit=collector.begin();sit!=collector.end();++sit) {
        ret[sit.key()].append(sit.data());
    }
    // Anniversaries
    collector = _entries(start,end,true);
    for (sit=collector.begin();sit!=collector.end();++sit) {
        ret[sit.key()].append(sit.data());
    }
    return ret;
}

EXPORT_HOLIDAY_PLUGIN(Birthday);
