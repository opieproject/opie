#ifndef __DUMMY_HOLIDAY_H
#define __DUMMY_HOLIDAY_H

#include <opie2/oholidayplugin.h>
#include <opie2/oholidaypluginif.h>

class DummyHoliday:public Opie::Datebook::HolidayPlugin
{
public:
    DummyHoliday():Opie::Datebook::HolidayPlugin(){}
    virtual ~DummyHoliday(){}

    virtual QString description();
    virtual QStringList entries(const QDate&);
    virtual QStringList entries(unsigned year, unsigned month, unsigned day);
    virtual QMap<QDate,QStringList> entries(const QDate&,const QDate&);
};

#endif
