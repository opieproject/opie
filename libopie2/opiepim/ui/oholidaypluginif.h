#ifndef _O_HOLIDAY_PLUGIN_IF_H
#define _O_HOLIDAY_PLUGIN_IF_H
#include <opie2/odebug.h>
#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
//c0a5f73f-975e-4492-9285-af555284c4ab
#define IID_HOLIDAY_PLUGIN        QUuid(0xc0a5f73f,0x975e,0x4492,0x92,0x85,0xaf,0x55,0x52,0x84,0xc4,0xab)
#endif

namespace Opie {
namespace Datebook {

class HolidayPlugin;

class HolidayPluginIf:public QUnknownInterface
{
public:
   HolidayPluginIf():QUnknownInterface(){}
   virtual ~HolidayPluginIf(){}

   virtual HolidayPlugin*plugin()=0;
};

template<class T> class HolidayPluginWrapper:public HolidayPluginIf
{
public:
    HolidayPluginWrapper():HolidayPluginIf(),_plugin(0){}
    virtual ~HolidayPluginWrapper(){if (_plugin) delete _plugin;}

    QRESULT queryInterface( const QUuid& uuid, QUnknownInterface** iface ) {
        *iface = 0;
        if (uuid == IID_HOLIDAY_PLUGIN || uuid == IID_QUnknown) {
            *iface = this;
        } else {
            return  QS_FALSE;
        }
        if (*iface) (*iface)->addRef();
        return QS_OK;
    }

    // from qcom
    Q_REFCOUNT

    virtual T*plugin() {
        if (!_plugin) {_plugin = new T();}
        return _plugin;
    }
protected:
    T*_plugin;
};

#define EXPORT_HOLIDAY_PLUGIN(  Plugin ) \
    Q_EXPORT_INTERFACE() { \
    Q_CREATE_INSTANCE(  Opie::Datebook::HolidayPluginWrapper<Plugin> ) \
    }
}
}
#endif

