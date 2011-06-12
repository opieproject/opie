
#include "rfcpopup.h"
#include "obexpopup.h"
#include "obexftpopup.h"
#include "panpopup.h"
#include "dunpopup.h"
#include "hidpopup.h"

#include "stdpopups.h"

extern "C" {

    QPopupMenu* newRfCommPopup( const OBluetoothServices& service,  BTDeviceItem* item, DeviceHandlerPool *devHandlerPool ) {
        return new OpieTooth::RfCommPopup(service, item, devHandlerPool);
    }
    QPopupMenu* newObexPushPopup( const OBluetoothServices& service,  BTDeviceItem* item, DeviceHandlerPool * ) {
        return  new OpieTooth::ObexPopup(service, item);
    }
    QPopupMenu* newObexFtpPopup( const OBluetoothServices& service,  BTDeviceItem* item, DeviceHandlerPool * ) {
        return  new OpieTooth::ObexFtpPopup(service, item);
    }
    QPopupMenu* newPanPopup( const OBluetoothServices&,  BTDeviceItem* item, DeviceHandlerPool * ) {
        return new OpieTooth::PanPopup( item );
    }

    QPopupMenu* newDunPopup( const OBluetoothServices& service,  BTDeviceItem* item, DeviceHandlerPool * ) {
        return new OpieTooth::DunPopup(service, item);
    }

    QPopupMenu* newHidPopup( const OBluetoothServices& service,  BTDeviceItem* item, DeviceHandlerPool *devHandlerPool ) {
        return new OpieTooth::HidPopup(service, item, devHandlerPool);
    }
}

