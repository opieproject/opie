
#include "rfcpopup.h"
#include "obexpopup.h"
#include "panpopup.h"
#include "dunpopup.h"

#include "stdpopups.h"

extern "C" {

    QPopupMenu* newRfcComPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item ) {
        return new OpieTooth::RfcCommPopup(/* servive,  item*/ ); // fix spellin RfComm vs. RfcComm and paramaters
        //return 0l;
    }
    QPopupMenu* newObexPushPopup( const OpieTooth::Services&,  OpieTooth::BTDeviceItem* ) {
        return  new OpieTooth::ObexPopup();
    }
    QPopupMenu* newPanPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item ) {
	    return 0; /* new OpieTooth::PanPopup( item ); */
    }

    QPopupMenu* newDunPopup( const OpieTooth::Services& service,  OpieTooth::BTDeviceItem* item ) {
	    return 0; /* new OpieTooth::DunPopup( item ); */
    }
}

