#ifndef _DATEBOOK_TYPES_H
#define _DATEBOOK_TYPES_H

#include <qpe/datebookdb.h>

#include <qvaluelist.h>
#include <qstringlist.h>

namespace Opie {
namespace Datebook {
    class HolidayPlugin;
    class HolidayPluginIf;
}
}

class QLibrary;

class DateBookDBHack : virtual public DateBookDB {
 public:
  virtual ~DateBookDBHack(){}
  Event eventByUID(int id);
};

class DateBookHoliday
{
public:
    DateBookHoliday();
    virtual ~DateBookHoliday();

    QStringList holidaylist(const QDate&);
    QStringList holidaylist(unsigned year, unsigned month, unsigned day);
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDate &from,const QDate &to );
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDateTime &start);

protected:
    void init();
    void deinit();

    struct HPlugin {
        Opie::Datebook::HolidayPlugin*_plugin;
        QLibrary*_lib;
        Opie::Datebook::HolidayPluginIf*_if;
    };
    QValueList<HPlugin*>_pluginlist;
};

class DateBookDBHoliday:virtual public DateBookDBHack {
public:
    DateBookDBHoliday():DateBookDBHack(){db_holiday=0;}
    virtual ~DateBookDBHoliday(){}
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDate &from,const QDate &to );
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDateTime &start);
    virtual QValueList<EffectiveEvent> getEffectiveEventsNoHoliday(const QDate &from,const QDate &to );
    virtual QValueList<EffectiveEvent> getEffectiveEventsNoHoliday(const QDateTime &start);

    DateBookHoliday*db_holiday;
};

#endif
