/*
 * GPLv2 only zecke@handhelds.org
 */

#ifndef DIR_DIR_VIEW_H
#define DIR_DIR_VIEW_H

#include <iface/dirview.h>


struct Dir_DirView : public PDirView {
    Dir_DirView(  const Config& );
    ~Dir_DirView();

    PInterfaceInfo* interfaceInfo()const;
    PDirLister* dirLister()const;
private:
    bool m_cfg : 1;
    mutable PDirLister* m_lister;
    mutable PInterfaceInfo *m_info;
};


#endif
