#ifndef _HOLIDAY_PLUGIN_CFG_WIDGET
#define _HOLIDAY_PLUGIN_CFG_WIDGET
#include <qwidget.h>

namespace Opie {
namespace Datebook {
class HolidayPluginConfigWidget:public QWidget
{
    Q_OBJECT
public:
    /**
     * This will construct the widget. The widget gets deleted once the parent
     * gets deleted as in any Qt application
     *
     * A Parent is required!
     *
     * @param parent The parent of the widget
     * @param name The name of the object
     */
    HolidayPluginConfigWidget(QWidget *parent,  const char *name = 0, WFlags fl = 0 );
    virtual ~HolidayPluginConfigWidget();

    virtual void saveConfig()=0;
};

}
}

#endif

