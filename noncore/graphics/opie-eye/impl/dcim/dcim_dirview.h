/*
 * GPLv2 only zecke@handhelds.org
 */

#ifndef DCIM_DIR_VIEW_H
#define DCIM_DIR_VIEW_H

#include <iface/dirview.h>

/*
 * Implementation for USB Host Digital Cameras
 * like my Casio QV 500 and boring Yakumo
 */
struct DCIM_DirView : public PDirView {
    DCIM_DirView( const Config& );
    ~DCIM_DirView();

    PInterfaceInfo* interfaceInfo()const;
    PDirLister    * dirLister    ()const;

private:
    mutable PDirLister     *m_lister;
    mutable PInterfaceInfo *m_info  ;
};

#endif
