#ifndef __NATIONAL_HOLIDAY_H
#define __NATIONAL_HOLIDAY_H

#include "nationalcfg.h"

#include <opie2/oholidayplugin.h>
#include <opie2/oholidaypluginif.h>

#include <qmap.h>
#include <qstringlist.h>

class NationalHoliday:public Opie::Datebook::HolidayPlugin
{

public:
    NationalHoliday();
    virtual ~NationalHoliday(){}

    virtual QString description();
    virtual QStringList entries(const QDate&);
    virtual QStringList entries(unsigned year, unsigned month, unsigned day);
    virtual QMap<QDate,QStringList> entries(const QDate&,const QDate&);
    virtual QValueList<EffectiveEvent> events(const QDate&,const QDate&);

protected:
    void init();
    unsigned int _lastyear;
    tholidaylist _days;
    QStringList files;
    bool init_done:1;
    void load_days();
};

#endif
