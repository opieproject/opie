#include "doc_lister.h"
#include "doc_ifaceinfo.h"
#include "doc_dirview.h"

PHUNK_VIEW_INTERFACE("Document View", Doc_DirView );


Doc_DirView::Doc_DirView( const Config& cfg)
    : PDirView(cfg)
{
    m_lister = 0;
    m_info = 0;
}

Doc_DirView::~Doc_DirView() {
}

PInterfaceInfo* Doc_DirView::interfaceInfo()const{
    if (!m_info )
        m_info =new DocInterfaceInfo;
    return m_info;
}

PDirLister* Doc_DirView::dirLister()const{
    if (!m_lister )
        m_lister = new Doc_DirLister();
    return m_lister;
}
