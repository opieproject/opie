#ifndef __DUMMY_HOLIDAY_H
#define __DUMMY_HOLIDAY_H

#include <opie2/oholidayplugin.h>
#include <opie2/oholidaypluginif.h>
#include <opie2/ocontactaccess.h>

#include <qmap.h>

class Birthday:public Opie::Datebook::HolidayPlugin
{
typedef QMap<QDate,QStringList> tDayMap;

public:
    Birthday();
    virtual ~Birthday(){}

    virtual QString description();
    virtual QStringList entries(const QDate&);
    virtual QStringList entries(unsigned year, unsigned month, unsigned day);
    virtual QMap<QDate,QStringList> entries(const QDate&,const QDate&);
protected:
    Opie::OPimContactAccess * m_contactdb;
    Opie::OPimContactAccess::List m_list;
    Opie::OPimContactAccess::List::Iterator m_it;
    virtual QMap<QDate,QString> _entries(const QDate&,const QDate&,bool anniversary);
};

#endif
