
#include <qpe/mimetype.h>
#include <qpe/resource.h>

#include "ofileselector.h"
#include "ofileselectoritem.h"
#include "ofilelistview.h"


OFileListView::OFileListView( QWidget* parent, OFileSelector* sel)
    : QListView( parent ), OFileView( sel )
{

}
OFileListView::~OFileListView() {

}
void OFileListView::clear() {
    QListView::clear();
}
void OFileListView::addFile( const QString&,
                             QFileInfo* info,
                             bool isSymlink ) {
    MimeType type( info->absFilePath() );
    QPixmap pix = type.pixmap();
    QString dir;
    QString name;
    bool locked = false;

    if( pix.isNull() )
        pix = Resource::loadPixmap( "UnknownDocument-14");

    dir = info->dirPath( true );

    if( isSymlink )
        name = info->fileName() + " -> " +info->dirPath() + "/" + info->readLink();
    else {
        name = info->fileName();
        if( ( selector()->mode() == OFileSelector::Open && !info->isReadable() ) ||
            ( selector()->mode() == OFileSelector::Save && !info->isWritable() ) ){

            locked = true; pix = Resource::loadPixmap("locked");
        }
    }
    new OFileSelectorItem( this, pix, name,
                           info->lastModified().toString(),
                           QString::number( info->size() ),
                           dir, locked );
}
void OFileListView::addFile( const QString& /*mime*/, const QString& /*dir*/,
                             const QString& /*file*/,  bool /*isSyml*/ ) {

}
void OFileListView::addDir( const QString&,
                            QFileInfo* info,  bool symlink ) {

    bool locked = false;
    QString name;
    QPixmap pix;

    if( ( selector()->mode() == OFileSelector::Open && !info->isReadable() ) ||
        ( selector()->mode() == OFileSelector::Save && !info->isWritable() ) ){

      locked = true;

      if( symlink )
	pix = selector()->pixmap("symlinkedlocked");
      else
	pix = Resource::loadPixmap("lockedfolder");

    }else { // readable
      pix = symlink ?  selector()->pixmap("dirsymlink") : Resource::loadPixmap("folder") ;
    }

    name = symlink ? info->fileName() + "->" + info->dirPath(true) + "/" +info->readLink() : info->fileName() ;

    new OFileSelectorItem( this, pix, name,
			   info->lastModified().toString(),
			   QString::number( info->size() ),
			   info->dirPath( true ), locked,
			   true );

}
void OFileListView::addDir( const QString& /*mime*/,  const QString& /*dir*/,
                            const QString& /*file*/, bool ) {

}
void OFileListView::addSymlink( const QString& /*mime*/,
                                QFileInfo* /*info*/,
                                bool /*isSym*/ ) {

}
void OFileListView::addSymlink( const QString& /*mime*/,  const QString& /*path*/,
                                const QString& /*file*/,  bool /*isSym*/ ) {

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
