
#include <qheader.h>

#include <qpe/mimetype.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include "ofileselector.h"
#include "ofileselectoritem.h"
#include "ofilelistview.h"


OFileListView::OFileListView( QWidget* parent, OFileSelector* sel)
    : QListView( parent ), OFileView( sel )
{
    QPEApplication::setStylusOperation( viewport(),
					QPEApplication::RightOnHold);
    addColumn(" " );
    addColumn(tr("Name"), 135 );
    addColumn(tr("Size"), -1 );
    addColumn(tr("Date"), 60 );
    addColumn(tr("Mime Type"), -1 );
    QHeader *head = header();
    head->hide();
    setSorting( 1 );
    setAllColumnsShowFocus( TRUE );

    connect(this, SIGNAL(selectionChanged() ),
	    this, SLOT(slotSelectionChanged() ) );

    connect(this, SIGNAL(currentChanged(QListViewItem *) ),
	    this, SLOT(slotCurrentChanged(QListViewItem * ) ) );

    connect(this, SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint &, int) ),
	    this, SLOT(slotClicked( int, QListViewItem *, const QPoint &, int) ) );

    connect(this, SIGNAL(mouseButtonPressed(int, QListViewItem *, const QPoint &, int )),
	    this, SLOT(slotRightButton(int, QListViewItem *, const QPoint &, int  ) ) );
}
OFileListView::~OFileListView() {

}
void OFileListView::clear() {
    QListView::clear();
}
void OFileListView::addFile( const QPixmap& pix,
                             const QString&,
                             QFileInfo* info,
                             const QString&  extra,
                             bool isSymlink ) {
    QString dir;
    QString name;
    bool locked = false;

    dir = info->dirPath( true );

    if( isSymlink )
        name = info->fileName() + " -> " +info->dirPath() + "/" + info->readLink();
    else {
        name = info->fileName();
        if( ( selector()->mode() == OFileSelector::Open && !info->isReadable() ) ||
            ( selector()->mode() == OFileSelector::Save && !info->isWritable() ) ){

            locked = true;
        }
    }
    new OFileSelectorItem( this, pix, name,
                           info->lastModified().toString(),
                           QString::number( info->size() ),
                           dir, locked, extra );
}
void OFileListView::addFile( const QPixmap&,
                             const QString& /*mime*/, const QString& /*dir*/,
                             const QString& /*file*/,
                             const QString& /*extra*/,
                             bool /*isSyml*/ ) {

}
void OFileListView::addDir( const QPixmap& pix, const QString&,
                            QFileInfo* info,
                            const QString& extra ,
                            bool symlink ) {

    bool locked = false;
    QString name;

    name = symlink ? info->fileName() + "->" + info->dirPath(true) + "/" +info->readLink() : info->fileName() ;

    new OFileSelectorItem( this, pix, name,
			   info->lastModified().toString(),
			   QString::number( info->size() ),
			   info->dirPath( true ), locked, extra,
			   true );

}
void OFileListView::addDir( const QPixmap&,
                            const QString& /*mime*/,  const QString& /*dir*/,
                            const QString& /*file*/,
                            const QString& /*extra*/,
                            bool ) {

}
void OFileListView::addSymlink( const QPixmap&,
                                const QString& /*mime*/,
                                QFileInfo* /*info*/,
                                const QString& /*extra*/,
                                bool /*isSym*/ ) {

}
void OFileListView::addSymlink(const QPixmap&,
                               const QString& /*m*/,  const QString& /*path*/,
                               const QString& /*file*/,
                               const QString& /*extra*/,
                               bool /*isSym*/ ) {

}
void OFileListView::cd( const QString& ) {

}
QWidget* OFileListView::widget() {
    return this;
}
QString OFileListView::selectedName()const{
    QListViewItem *item  = currentItem();
    if (!item )
        return QString::null;

    return item->text( 1 );
}
QString OFileListView::selectedExtra()const{
    QListViewItem* item = currentItem();
    if (!item) return QString::null;
    OFileSelectorItem* fit = (OFileSelectorItem*)fit;

    return fit->extra();
}
QStringList OFileListView::selectedNames()const {
    QStringList list;
    list << selectedName();
    return list;
}
QString OFileListView::selectedPath()const {
    return QString::null;
}
QStringList OFileListView::selectedPaths()const {
    QStringList list;
    list << selectedPath();
    return list;
}
int OFileListView::fileCount() {
    return childCount();
}
void OFileListView::sort() {
    QListView::sort();
}
void OFileListView::slotSelectionChanged() {

}
void OFileListView::slotCurrentChanged( QListViewItem* item) {
    if (!item )
        return;

    OFileSelectorItem* sel = (OFileSelectorItem*) item;

    qWarning("current changed");
    if(!sel->isDir() ){
        updateLine( sel->text(1) );

        if (selector()->mode() == OFileSelector::Fileselector ) {
            QStringList str = QStringList::split("->", sel->text(1) );
            fileSelected(sel->directory(), str[0].stripWhiteSpace(),sel->extra() );
        }
    }
}
void OFileListView::slotClicked( int button, QListViewItem* item,
                                 const QPoint&, int ) {
  if ( !item )
    return;

  if( button != Qt::LeftButton )
    return;

  OFileSelectorItem *sel = (OFileSelectorItem*)item;

  if(!sel->isLocked() ){
      QStringList str = QStringList::split("->", sel->text(1) );
      if( sel->isDir() ){
          changedDir( sel->directory(), str[0].stripWhiteSpace(),sel->extra() );
      }else{
          updateLine(  str[0].stripWhiteSpace() );
          fileSelected(  sel->directory(),str[0].stripWhiteSpace(),  sel->extra() );
      }
  }
}
void OFileListView::slotRightButton( int button, QListViewItem* item,
                                     const QPoint&, int ) {
    if (!item || (button != Qt::RightButton ))
        return;

    /* raise contextmenu */
}
