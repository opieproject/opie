
#ifndef OPIE_TOOTH_STD_POPUPS
#define OPIE_TOOTH_STD_POPUPS

#include <qlistview.h>
#include <qpopupmenu.h>

#include <services.h>

extern "C" {
QPopupMenu* newRfcComPopup( const OpieTooth::Services&, QListViewItem* );
QPopupMenu* newObexPushPopup( const OpieTooth::Services&, QListViewItem* );
}

#endif
