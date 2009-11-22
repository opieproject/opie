#ifndef __NATIONAL_HOLIDAY_H
#define __NATIONAL_HOLIDAY_H

#include "nationalcfg.h"

#include <opie2/oholidayplugin.h>
#include <opie2/oholidaypluginif.h>
#include <opie2/oholidayplugincfgwidget.h>

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
    virtual Opie::Datebook::HolidayPluginConfigWidget*configWidget(QWidget *parent=0,  const char *name = 0, QWidget::WFlags fl = 0);

protected:
    void init();
    void setyear(const QDate&);
    void calc_easterDate();
    static int weektonumber(const QString&);
    static QDate movedateday(const QDate&,int weekday,bool direction);
    static int monthtonumber(const QString&m);
    static int dayoftoint(const QString&d);

    unsigned int _lastyear;
    tholidaylist _days;
    tholidaylist _fdays;
    tentrylist floatingDates;

    QStringList files;
    bool init_done:1;
    void load_days();
    QDate easterDate;
};

#endif
