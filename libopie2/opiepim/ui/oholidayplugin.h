#ifndef _HOLIDAY_PLUGIN_H
#define _HOLIDAY_PLUGIN_H

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qmap.h>

class EffectiveEvent;

namespace Opie {
namespace Datebook {

class HolidayPlugin
{
public:
    HolidayPlugin(){};
    virtual ~HolidayPlugin(){};
    virtual QString description()=0;
    virtual QStringList entries(const QDate&)=0;
    virtual QStringList entries(unsigned year, unsigned month, unsigned day)=0;
    virtual QMap<QDate,QStringList> entries(const QDate&,const QDate&)=0;
    virtual QValueList<EffectiveEvent> events(const QDate&,const QDate&)=0;
};

}
}
#endif

