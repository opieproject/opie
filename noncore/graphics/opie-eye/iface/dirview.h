/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_DIR_VIEW_H
#define PHUNK_DIR_VIEW_H

#include <qmap.h>

#include <qpe/config.h>

class PInterfaceInfo;
class PDirLister;

struct PDirView {
    PDirView(  const Config& );
    virtual ~PDirView();
    virtual PInterfaceInfo* interfaceInfo()const = 0;
    virtual PDirLister* dirLister()const = 0;
};

typedef PDirView* (*phunkViewCreateFunc )(const Config& );
typedef QMap<QString,phunkViewCreateFunc> ViewMap;

ViewMap* viewMap();
PDirView* currentView();
void setCurrentView( PDirView* );



#define PHUNK_VIEW_INTERFACE( NAME, IMPL ) \
    static PDirView *create_ ## IMPL( const Config& cfg ) { \
        return new IMPL( cfg ); \
    }                                          \
    static ViewMap::Iterator dummy_ ## IMPL = viewMap()->insert( NAME, create_ ## IMPL );



#endif
