
#ifndef OPIE_TOOTH_DEVICE_HANDLER_H
#define OPIE_TOOTH_DEVICE_HANDLER_H

#include <qvaluelist.h>

#include <remotedevice.h>

namespace OpieTooth {
    /**
     * DeviceHandler is responsible for loading
     * and saving devices from a config File
     */
    class DeviceHandler {
    public:
        /**
         * c'tor
         */
        DeviceHandler();

        /**
         * d'tor
         */
        ~DeviceHandler();

        /**
         * loads from $HOME/Settings/bluetooth/ *
         */
        RemoteDevice::ValueList load();

        /**
         * Saves to $HOME/Settings/bluetooth
         */
        void save( const RemoteDevice::ValueList & );

    };


};

#endif
