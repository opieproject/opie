/*
 * GPLv2 only zecke@handhelds.org
 */

#ifndef DOC_DIR_VIEW_H
#define DOC_DIR_VIEW_H

#include <iface/dirview.h>

class Doc_DirLister;

struct Doc_DirView : public PDirView {
    Doc_DirView(  const Config& );
    ~Doc_DirView();

    PInterfaceInfo* interfaceInfo()const;
    PDirLister* dirLister()const;
    QWidget* widget(QWidget*parent);
private:
    bool m_cfg : 1;
    mutable Doc_DirLister* m_lister;
    mutable PInterfaceInfo *m_info;
};


#endif
