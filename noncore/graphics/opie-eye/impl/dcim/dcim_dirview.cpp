#include "dcim_dirview.h"

#include "dcim_lister.h"
#include "dcim_ifaceinfo.h"

PHUNK_VIEW_INTERFACE( "Digital Camera", DCIM_DirView );

DCIM_DirView::DCIM_DirView( const Config& cfg )
    : PDirView( cfg ), m_lister( 0l ), m_info( 0l )
{}

DCIM_DirView::~DCIM_DirView()
{}

PInterfaceInfo* DCIM_DirView::interfaceInfo()const {
    if ( !m_info )
        m_info = new DCIM_InterfaceInfo;

    return m_info;
}


PDirLister* DCIM_DirView::dirLister()const {
    if ( !m_lister )
        m_lister = new DCIM_DirLister();

    return m_lister;
}
