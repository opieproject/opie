
#ifndef OPIE_TOOTH_STD_POPUPS
#define OPIE_TOOTH_STD_POPUPS

#include <qlistview.h>
#include <qpopupmenu.h>

#include <services.h>

#include "btdeviceitem.h"

extern "C" {

    QPopupMenu* newRfcComPopup( const OpieTooth::Services&,  OpieTooth::BTDeviceItem* );
    QPopupMenu* newObexPushPopup( const OpieTooth::Services&,  OpieTooth::BTDeviceItem* );
}

#endif
