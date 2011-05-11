
#include "rfcpopup.h"
#include "obexpopup.h"
#include "obexftpopup.h"
#include "panpopup.h"
#include "dunpopup.h"
#include "hidpopup.h"

#include "stdpopups.h"

extern "C" {

    QPopupMenu* newRfCommPopup( const Opie::Bluez::OBluetoothServices& service,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::RfCommPopup(service, item);
    }
    QPopupMenu* newObexPushPopup( const Opie::Bluez::OBluetoothServices& service,  OpieTooth::BTDeviceItem* item) {
        return  new OpieTooth::ObexPopup(service, item);
    }
    QPopupMenu* newObexFtpPopup( const Opie::Bluez::OBluetoothServices& service,  OpieTooth::BTDeviceItem* item) {
        return  new OpieTooth::ObexFtpPopup(service, item);
    }
    QPopupMenu* newPanPopup( const Opie::Bluez::OBluetoothServices&,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::PanPopup( item );
    }

    QPopupMenu* newDunPopup( const Opie::Bluez::OBluetoothServices& service,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::DunPopup(service, item);
    }

    QPopupMenu* newHidPopup( const Opie::Bluez::OBluetoothServices& service,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::HidPopup(service, item);
    }
}

