#include <qlineedit.h>

#include <qpe/applnk.h>

#include "ofileselector.h"
#include "ofileview.h"


OFileView::OFileView( OFileSelector* sel)
    : m_sel( sel )
{
}
OFileView::~OFileView() {
}
void OFileView::fileSelected( const QString& dir,const QString& file,const QString& extra ) {
    m_sel->currentLister()->fileSelected( dir,file,extra );
}
void OFileView::contextMenu() {
    m_sel->internContextMenu();
}
void OFileView::changedDir( const QString& s,  const QString& file, const QString& extra) {
    m_sel->currentLister()->changedDir( s, file,extra );
}
OFileSelector* OFileView::selector() const {
    return m_sel;
}
void OFileView::updateLine( const QString& str ) {
    if (m_sel->m_shLne )
        m_sel->m_edit->setText( str );
}
