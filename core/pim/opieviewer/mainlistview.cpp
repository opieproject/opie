
#include "mainlistviewitem.h"
#include "mainlistview.h"

using namespace Viewer;

MainListView::MainListView( QWidget* parent, const char* name )
    : QListView( parent, name ) {
// initialize the ListView look
    addColumn( "Pixmap",  10 );
    addColumn( "Text" );
    setAllColumnsShowFocus( true );
    setRootIsDecorated( true );

    connect(this, SIGNAL(currentChanged( QListViewItem* ) ),
            SLOT(slotCurrentChanged( QListViewItem* ) ) );
    connect(this, SIGNAL(selectionChanged() ),
            SLOT(slotSelectionChanged() ) );
}
MainListView::~MainListView() {

}
void MainListView::insertCategory( const QString& name,
                                   int id ) {

    m_items.append( new MainListViewItem( this, name, id ) );

}
void MainListView::insertEntry( int id,
                                const QString& category,
                                int categoryId,
                                const QString& appName,
                                const QString& text,
                                const QString& richText,
                                const QPixmap& pixmap ) {
    MainListViewItem* parent =0l;
    for (parent = m_items.first() ; parent != 0; parent = m_items.next() ) {
        if ( parent->id() == categoryId )
            break;
        else if ( parent->category() == category )
            break;
        else if ( parent->category() == tr("Unfiled") )
            break;
    }
    if ( parent != 0 )
        new MainListViewItem( parent, appName, text, richText, pixmap, id );
}
void MainListView::insertRelation( const QString& appSrc, int idSrc,
                                   const QString& appRel, int relateWith ) {

}
void MainListView::clear() {
    m_items.clear();
    QListView::clear();
}
MainListViewItem* MainListView::find( const QString& app, int id ) {
    return 0l;
}
void MainListView::slotSelectionChanged() {
    MainListViewItem *item = (MainListViewItem*)selectedItem();
    if ( item == 0 ) return;

    if ( !item->isCategory() ) emit selected( item->appName(), item->richText() );
    qWarning("changed");
}
void MainListView::slotCurrentChanged( QListViewItem* ) {
    qWarning("Current cha");
}
void MainListView::slotClicked(int, QListViewItem*,  const QPoint&, int ) {

}
void MainListView::slotRightButton(int, QListViewItem*, const QPoint&, int ) {

}
