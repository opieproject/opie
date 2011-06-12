
#ifndef OPIE_TOOTH_STD_POPUPS
#define OPIE_TOOTH_STD_POPUPS

#include <qlistview.h>
#include <qpopupmenu.h>

#include <opie2/obluetoothservices.h>
#include <opie2/obluetoothdevicehandler.h>

#include "btdeviceitem.h"

using Opie::Bluez::OBluetoothServices;
using Opie::Bluez::DeviceHandlerPool;
using OpieTooth::BTDeviceItem;

extern "C" {

    QPopupMenu* newRfCommPopup( const OBluetoothServices&,  BTDeviceItem*, DeviceHandlerPool * );
    QPopupMenu* newObexPushPopup( const OBluetoothServices&,  BTDeviceItem*, DeviceHandlerPool * );
    QPopupMenu* newPanPopup( const OBluetoothServices&, BTDeviceItem*, DeviceHandlerPool * );
    QPopupMenu* newDunPopup( const OBluetoothServices&, BTDeviceItem*, DeviceHandlerPool * );
    QPopupMenu* newHidPopup( const OBluetoothServices&, BTDeviceItem*, DeviceHandlerPool * );
    QPopupMenu* newObexFtpPopup( const OBluetoothServices&, BTDeviceItem*, DeviceHandlerPool * );
}

#endif
