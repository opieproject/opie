#include "olister.h"
#include "ofileview.h"
#include "opixmapprovider.h"
#include "ofileselector.h"


OLister::OLister( OFileSelector* view)
    : m_view( view )
{
    m_prov = new OPixmapProvider( view );
}
OLister::~OLister() {
    delete m_prov;
}
void OLister::setPixmapProvider( OPixmapProvider* prov ) {
    delete m_prov;
    m_prov = prov;
}
bool OLister::showFiles()const {
    return m_view->showFiles();
}
bool OLister::showDirs()const {
    return m_view->showDirs();
}
void OLister::addFile( const QString& mine,
                       QFileInfo* info,
                       bool isSymlink ) {
    int t = isSymlink ?  OPixmapProvider::File | OPixmapProvider::Symlink :
            OPixmapProvider::File;
    QPixmap pix = provider()->pixmap(t, mine,
                                     info);
    view()->currentView()->addFile( pix,
                                    mine,
                                    info,
                                    isSymlink );
}
void OLister::addFile( const QString& mine,
                       const QString& path,
                       const QString& file,
                       bool isSymlink ) {
    int t = isSymlink ? OPixmapProvider::File | OPixmapProvider::Symlink :
            OPixmapProvider::File;

    QPixmap pix = provider()->pixmap(t, mine, path, file );
    view()->currentView()->addFile( pix,
                                    mine,
                                    path,
                                    file,
                                    isSymlink );
}
void OLister::addDir( const QString& mine,
                      QFileInfo* info,
                      bool isSymlink  ) {
    int t = isSymlink ? OPixmapProvider::Dir | OPixmapProvider::Symlink :
            OPixmapProvider::Dir;
    QPixmap pix = provider()->pixmap(t, mine, info );
    view()->currentView()->addDir( pix,
                                   mine,
                                   info,
                                   isSymlink );
}
void OLister::addDir( const QString& mine,
                      const QString& path,
                      const QString& dir,
                      bool isSymlink ) {

    int t = isSymlink ? OPixmapProvider::Dir | OPixmapProvider::Symlink :
            OPixmapProvider::Dir;
    QPixmap pix = provider()->pixmap(t, mine, path, dir );

    view()->currentView()->addDir( pix,
                                   mine,
                                   path,
                                   dir,
                                   isSymlink );
}
void OLister::addSymlink( const QString& mine,
                          QFileInfo* info,
                          bool isSymlink ) {
    QPixmap pix = provider()->pixmap( OPixmapProvider::Symlink, mine, info );
    view()->currentView()->addSymlink( pix,
                                       mine,
                                       info,
                                       isSymlink );
}
void OLister::addSymlink( const QString& mine,
                          const QString& path,
                          const QString& name,
                          bool isSymlink ) {
    QPixmap pix = provider()->pixmap( OPixmapProvider::Symlink, mine,
                                      path, name );
    view()->currentView()->addSymlink( pix,
                                       mine,
                                       path,
                                       name,
                                       isSymlink );
}
OFileSelector* OLister::view() {
    return m_view;
}
OPixmapProvider* OLister::provider() {
    return m_prov;
}
bool OLister::compliesMime( const QString& mime ) {
    return view()->compliesMime( mime );
}
