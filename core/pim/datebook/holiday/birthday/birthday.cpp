#include "birthday.h"

#include <opie2/ocontactaccess.h>

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
        Opie::OPimContact querybirthdays;
        querybirthdays.setBirthday(aDate);
        m_list = m_contactdb->queryByExample( querybirthdays,Opie::OPimContactAccess::DateDay| Opie::OPimContactAccess::DateMonth);
        if ( m_list.count() > 0 ){
            QString pre = QObject::tr("Birthday","holidays")+" ";
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

QMap<QDate,QStringList> Birthday::entries(const QDate&,const QDate&)
{
    QMap<QDate,QStringList> ret;
    return ret;
}

EXPORT_HOLIDAY_PLUGIN(Birthday);
