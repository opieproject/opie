#ifndef _HOLIDAY_PLUGIN_H
#define _HOLIDAY_PLUGIN_H

namespace Opie {
namespace Datebook {

#include <qstring.h>
#include <qstringlist.h>
#include <qdate.h>

class HolidayPlugin
{
public:
    HolidayPlugin(){};
    virtual ~HolidayPlugin(){};
    virtual QString description()=0;
    virtual QStringList entries(const QDate&)=0;
    virtual QStringList entries(unsigned year, unsigned month, unsigned day)=0;
};

}
}
#endif

