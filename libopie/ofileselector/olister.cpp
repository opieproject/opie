#include <qcombobox.h>

#include "olister.h"
#include "ofileview.h"
#include "opixmapprovider.h"
#include "ofileselector.h"


OLister::OLister( OFileSelector* view)
    : m_view( view ), m_acc( 0l )
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
                       const QString& extra,
                       bool isSymlink ) {
    int t = isSymlink ?  OPixmapProvider::File | OPixmapProvider::Symlink :
            OPixmapProvider::File;
    QPixmap pix = provider()->pixmap(t, mine,
                                     info);
    view()->currentView()->addFile( pix,
                                    mine,
                                    info,
                                    extra,
                                    isSymlink );
}
void OLister::addFile( const QString& mine,
                       const QString& path,
                       const QString& file,
                       const QString& extra,
                       bool isSymlink ) {
    int t = isSymlink ? OPixmapProvider::File | OPixmapProvider::Symlink :
            OPixmapProvider::File;

    QPixmap pix = provider()->pixmap(t, mine, path, file );
    view()->currentView()->addFile( pix,
                                    mine,
                                    path,
                                    file,
                                    extra,
                                    isSymlink );
}
void OLister::addDir( const QString& mine,
                      QFileInfo* info,
                      const QString& extra,
                      bool isSymlink  ) {
    int t = isSymlink ? OPixmapProvider::Dir | OPixmapProvider::Symlink :
            OPixmapProvider::Dir;
    QPixmap pix = provider()->pixmap(t, mine, info );
    view()->currentView()->addDir( pix,
                                   mine,
                                   info,
                                   extra,
                                   isSymlink );
}
void OLister::addDir( const QString& mine,
                      const QString& path,
                      const QString& dir,
                      const QString& extra,
                      bool isSymlink ) {

    int t = isSymlink ? OPixmapProvider::Dir | OPixmapProvider::Symlink :
            OPixmapProvider::Dir;
    QPixmap pix = provider()->pixmap(t, mine, path, dir );

    view()->currentView()->addDir( pix,
                                   mine,
                                   path,
                                   dir,
                                   extra,
                                   isSymlink );
}
void OLister::addSymlink( const QString& mine,
                          QFileInfo* info,
                          const QString& extra,
                          bool isSymlink ) {
    QPixmap pix = provider()->pixmap( OPixmapProvider::Symlink, mine, info );
    view()->currentView()->addSymlink( pix,
                                       mine,
                                       info,
                                       extra,
                                       isSymlink );
}
void OLister::addSymlink( const QString& mine,
                          const QString& path,
                          const QString& name,
                          const QString& extra,
                          bool isSymlink ) {
    QPixmap pix = provider()->pixmap( OPixmapProvider::Symlink, mine,
                                      path, name );
    view()->currentView()->addSymlink( pix,
                                       mine,
                                       path,
                                       name,
                                       extra,
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
OListerCmbAccess* OLister::comboBox() {
    if (!m_acc )
        m_acc = new OListerCmbAccess( view()->m_location );

    return m_acc;
}


OListerCmbAccess::OListerCmbAccess(QComboBox* box )
    : m_cmb( cmb )
{}
OListerCmbAccess::~OListerCmbAccess() {
}
void OListerCmbAccess::clear() {
    if ( m_cmb )
        m_cmb->clear();
}
void OListerCmbAccess::setCurrentItem( const QString& add, bool FORCE_ADD) {
    if ( !m_cmb ) return;


    int c = m_cmb->count();
    for ( int i = 0; i < m_cmb->count(); i++ ) {
        if ( m_cmb->text(i) == add ) {
            bo->setCurrentItem( i );
            return;
        }
    }
    m_cmb->insertItem(add );
    m_cmb->setCurrentItem( c );
}
void OListerCmbAccess::insert( const QString& str ) {
    if ( m_cmb )
        m_cmb->insertItem( str );
}
QString OListerCmbAccess::currentText()const {
    QString str;
    if (m_cmb )
        str = m_cmb->currentText();

    return str;
}
