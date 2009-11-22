#ifndef __DUMMY_HOLIDAY_H
#define __DUMMY_HOLIDAY_H

#include "hlist.h"

#include <opie2/oholidayplugin.h>
#include <opie2/oholidaypluginif.h>

#include <qmap.h>

class ChrisHoliday:public Opie::Datebook::HolidayPlugin
{
typedef QMap<QDate,QStringList> tDayMap;

public:
    ChrisHoliday():Opie::Datebook::HolidayPlugin(){_lastyear=0;}
    virtual ~ChrisHoliday(){}

    virtual QString description();
    virtual QStringList entries(const QDate&);
    virtual QStringList entries(unsigned year, unsigned month, unsigned day);
    virtual QMap<QDate,QStringList> entries(const QDate&,const QDate&);

protected:
    HList _internallist;
    unsigned int _lastyear;
    tDayMap _days;

    void calcit(int year);
    void calc_easter();
    void calc_christmas();
};

#endif
