#ifndef _HOLIDAY_PLUGIN_H
#define _HOLIDAY_PLUGIN_H

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qmap.h>
#include <qwidget.h>

namespace Opie {
namespace Datebook {

class HolidayPluginConfigWidget;

class HolidayPlugin2
{
public:
    HolidayPlugin2(){};
    virtual ~HolidayPlugin2(){};
    virtual QString description()=0;
    virtual QStringList entries(const QDate&)=0;
    virtual QMap<QDate,QStringList> entries(const QDate&,const QDate&);

    virtual HolidayPluginConfigWidget*configWidget(QWidget *parent=0,  const char *name = 0, QWidget::WFlags fl = 0 ){return 0;}
};
}
}
#endif

