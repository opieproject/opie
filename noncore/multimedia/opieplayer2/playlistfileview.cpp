
#include "playlistfileview.h"

#include <qpe/global.h>

PlayListFileView::PlayListFileView( const QString &mimeTypePattern, QWidget *parent, const char *name )
    : PlayListView( parent, name ), m_mimeTypePattern( mimeTypePattern )
{
    addColumn( tr( "Title" ), 140);
    addColumn( tr( "Size" ), -1 );
    addColumn(tr( "Media" ), 0 );
    addColumn(tr( "Path" ), -1 );
    setColumnAlignment( 1, Qt::AlignRight );
    setColumnAlignment( 2, Qt::AlignRight );
    setAllColumnsShowFocus( TRUE );
    setSorting( 3, TRUE );
    setMultiSelection( TRUE );
    setSelectionMode( QListView::Extended );
}

PlayListFileView::~PlayListFileView()
{
}

void PlayListFileView::scanFiles()
{
    m_files.detachChildren();
    QListIterator<DocLnk> sdit( m_files.children() );
    for ( ; sdit.current(); ++sdit )
        delete sdit.current();

    Global::findDocuments( &m_files, m_mimeTypePattern );
}

/* vim: et sw=4 ts=4
 */
