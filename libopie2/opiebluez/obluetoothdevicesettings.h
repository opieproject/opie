
#ifndef OBLUETOOTHDEVICESETTINGS_H
#define OBLUETOOTHDEVICESETTINGS_H

#include <qvaluelist.h>

#include <opie2/obluetoothdevicerecord.h>

namespace Opie {
namespace Bluez {

/**
 * DeviceSettings is responsible for loading
 * and saving devices from a config File
 */
class DeviceSettings {
  public:
    DeviceSettings();
    ~DeviceSettings();
    DeviceRecord::ValueList load();
    void save( const DeviceRecord::ValueList & );
};

}
}

#endif
