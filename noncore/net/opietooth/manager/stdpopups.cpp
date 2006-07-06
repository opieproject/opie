
#include "rfcpopup.h"
#include "obexpopup.h"
#include "obexftpopup.h"
#include "panpopup.h"
#include "dunpopup.h"
#include "hidpopup.h"

#include "stdpopups.h"

extern "C" {

    QPopupMenu* newRfcComPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::RfcCommPopup(service, item); // fix spellin RfComm vs. RfcComm and paramaters
    }
    QPopupMenu* newObexPushPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item) {
        return  new OpieTooth::ObexPopup(service, item);
    }
    QPopupMenu* newObexFtpPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item) {
        return  new OpieTooth::ObexFtpPopup(service, item);
    }
    QPopupMenu* newPanPopup( const OpieTooth::Services&,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::PanPopup( item );
    }

    QPopupMenu* newDunPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::DunPopup(service, item);
    }

    QPopupMenu* newHidPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::HidPopup(service, item);
    }
}

