#ifndef _O_HOLIDAY_PLUGIN_IF_H
#define _O_HOLIDAY_PLUGIN_IF_H

#include <opie2/odebug.h>

#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
//"b981b4e9-6d5d-4ee0-a193-f8d0e443809b"
#define IID_HOLIDAY_PLUGIN QUuid(0xb981b4e9, 0x6d5d, 0x4ee0 0xa1, 0x93,0xf8, 0xd0, 0xe4, 0x43, 0x80, 0x9b)
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

template<class T>HolidayPluginWrapper:public HolidayPluginIf
{
public:
    HolidayPluginWrapper():HolidayPluginIf(),_plugin(0){}
    virtual ~HolidayPluginWrapper(){if (_plugin) delete _plugin;}

    QRESULT queryInterface( const QUuid& uuid, QUnknownInterface** iface ) {
        odebug << "HolidayPluginWrapper::queryInterface()" << oendl;
        *iface = 0;
        if (uuid==IID_HOLIDAY_PLUGIN||uuid==IID_QUnknown) {
            *iface = this;
        } else {
            return  QS_FALSE;
        }
        if (*iface) *iface->addRef();
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

