
#include "playlistfileview.h"

#include <qfile.h>
#include <qpe/global.h>
#include <qpe/resource.h>

PlayListFileView::PlayListFileView( const QString &mimeTypePattern, const QString &itemPixmapName, QWidget *parent, const char *name )
    : PlayListView( parent, name ), m_mimeTypePattern( mimeTypePattern ), m_itemPixmapName( itemPixmapName ), m_scannedFiles( false ),
      m_viewPopulated( false )
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

    if ( m_viewPopulated ) {
        m_viewPopulated = false;
        populateView();
    }
}

void PlayListFileView::populateView()
{
    if ( m_viewPopulated )
        return;

    clear();

    if( !m_scannedFiles ) {
        m_viewPopulated = false; // avoid a recursion :)
        scanFiles();
        m_scannedFiles = true;
    }

    m_viewPopulated = true;

    QString storage;
    QListIterator<DocLnk> dit( m_files.children() );
    for ( ; dit.current(); ++dit ) {

        QListViewItem *newItem;

        if ( QFile::exists( dit.current()->file() ) ||
             dit.current()->file().left( 4 ) == "http" ) {

            unsigned long size = QFile( dit.current()->file() ).size();

            newItem = new QListViewItem( this, dit.current()->name(),
                                               QString::number( size ), "" /*storage*/,
                                               dit.current()->file() );
            newItem->setPixmap( 0, Resource::loadPixmap( m_itemPixmapName ) );
        }
    }
}

/* vim: et sw=4 ts=4
 */
