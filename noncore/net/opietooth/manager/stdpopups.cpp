
#include "rfcpopup.h"
#include "obexpopup.h"

#include "stdpopups.h"

extern "C" {

    QPopupMenu* newRfcComPopup( const OpieTooth::Services& servive,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::RfcCommPopup(/* servive,  item*/ ); // fix spellin RfComm vs. RfcComm and paramaters
        //return 0l;
    }
    QPopupMenu* newObexPushPopup( const OpieTooth::Services&,  OpieTooth::BTDeviceItem* ) {
        return  new OpieTooth::ObexPopup();
    }
}
