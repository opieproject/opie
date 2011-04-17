
#ifndef OPIE_TOOTH_STD_POPUPS
#define OPIE_TOOTH_STD_POPUPS

#include <qlistview.h>
#include <qpopupmenu.h>

#include <opie2/obluetoothservices.h>

#include "btdeviceitem.h"

extern "C" {

    QPopupMenu* newRfcComPopup( const Opie::Bluez::OBluetoothServices&,  OpieTooth::BTDeviceItem* );
    QPopupMenu* newObexPushPopup( const Opie::Bluez::OBluetoothServices&,  OpieTooth::BTDeviceItem* );
    QPopupMenu* newPanPopup( const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem* );
    QPopupMenu* newDunPopup( const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem* );
    QPopupMenu* newHidPopup( const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem* );
    QPopupMenu* newObexFtpPopup( const Opie::Bluez::OBluetoothServices&, OpieTooth::BTDeviceItem* );
}

#endif
