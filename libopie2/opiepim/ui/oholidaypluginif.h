#ifndef _O_HOLIDAY_PLUGIN_IF_H
#define _O_HOLIDAY_PLUGIN_IF_H
#include <opie2/odebug.h>
#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
//017f90d4-34fc-4dc5-aed7-498c6da6571e
#define IID_HOLIDAY_PLUGIN QUuid(0x017f90d4,0x34fc,0x4dc5,0xae,0xd7,0x49,0x8c,0x6d,0xa6,0x57,0x1e)
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
        odebug << "HolidayPluginWrapper::queryInterface()" << oendl;
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

