
#include "playlistfileview.h"

PlayListFileView::PlayListFileView( QWidget *parent, const char *name )
    : PlayListView( parent, name )
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

/* vim: et sw=4 ts=4
 */
