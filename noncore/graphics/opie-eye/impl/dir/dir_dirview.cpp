#include "dir_lister.h"
#include "dir_ifaceinfo.h"
#include "dir_dirview.h"

PHUNK_VIEW_INTERFACE("Dir View", Dir_DirView );


Dir_DirView::Dir_DirView( const Config& cfg)
    : PDirView(cfg)
{
    m_cfg = cfg.readBoolEntry( "Dir_Check_All_Files", true);
    m_lister = 0;
    m_info = 0;
}

Dir_DirView::~Dir_DirView() {
}

PInterfaceInfo* Dir_DirView::interfaceInfo()const{
    if (!m_info )
        m_info =new DirInterfaceInfo;
    return m_info;
}

PDirLister* Dir_DirView::dirLister()const{
    if (!m_lister )
        m_lister = new Dir_DirLister(m_cfg);
    return m_lister;
}
