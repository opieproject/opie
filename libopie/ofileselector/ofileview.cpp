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
void OFileView::fileSelected( const QString& s ) {
    m_sel->internFileSelected( s );
}
void OFileView::fileSelected( const DocLnk& s) {
    m_sel->internFileSelected( s );
}
void OFileView::contextMenu() {
    m_sel->internContextMenu();
}
void OFileView::changedDir( const QString& s) {
    m_sel->internChangedDir( s );
}
void OFileView::changedDir( const QDir& d ) {
    m_sel->internChangedDir( d );
}
OFileSelector* OFileView::selector() const {
    return m_sel;
}
void OFileView::updateLine( const QString& str ) {
    if (m_sel->m_shLne )
        m_sel->m_edit->setText( str );
}
