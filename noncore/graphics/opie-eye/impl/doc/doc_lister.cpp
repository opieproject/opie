/*
 * GPLv2 zecke@handhelds.org
 */

#include "doc_lister.h"

#include <lib/slavemaster.h>

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>

#include <qtopia/private/categories.h>
#include <qtopia/categoryselect.h>

using namespace Opie::Core;

/* QT */
#include <qdir.h>
#include <qfileinfo.h>

Doc_DirLister::Doc_DirLister()
    : PDirLister( "doc_dir_lister" )
{
    /* connect the signals */
    m_catFilter = 0;
    SlaveMaster* master = SlaveMaster::self();
    connect( master, SIGNAL(sig_start()), this, SIGNAL(sig_start()) );
    connect( master, SIGNAL(sig_end()),   this, SIGNAL(sig_end()) );
    connect( master, SIGNAL(sig_thumbInfo(const QString&, const QString&)),
             this, SLOT(slotThumbInfo(const QString&, const QString&)) );
    connect( master, SIGNAL(sig_fullInfo(const QString&, const QString&)),
             this, SLOT(slotFullInfo(const QString&, const QString&)) );
    connect( master, SIGNAL(sig_thumbNail(const QString&, const QPixmap&)),
             this, SLOT(slotThumbNail(const QString&, const QPixmap&)) );

    m_namemap.clear();
    m_filemap.clear();
    m_docreads = false;
}

QString Doc_DirLister::defaultPath()const {
    return QString::null;
}

bool Doc_DirLister::matchCat(const AppLnk* app)
{
    if (!app) return false;
    if (m_catFilter==0 || app->categories().contains(m_catFilter) || m_catFilter == -1 && app->categories().count() == 0 ) {
        return true;
    }
    return false;
}

QString Doc_DirLister::setStartPath(const QString&) {
    static const QString Mtype_str("image/jpeg;image/gif;image/bmp;image/png");
    if (m_namemap.isEmpty()) {
        if (!m_docreads) {
            Global::findDocuments(&m_ds,Mtype_str);
            m_docreads = true;
        }
        QListIterator<DocLnk> dit(m_ds.children());
        for( ; dit.current(); ++dit) {
            if (!matchCat((*dit))) continue;
            m_namemap[(*dit)->name()]=(*dit)->file();
            m_filemap[(*dit)->file()]=(*dit)->name();
        }
    }
    return QString::null;
}

QString Doc_DirLister::currentPath()const {
      return QString::null;
}


QStringList Doc_DirLister::folders()const {
    return QStringList();
}

QStringList Doc_DirLister::files()const {
    QStringList out;
    QMap<QString,QString>::ConstIterator it;
    for (it = m_namemap.begin();it != m_namemap.end();++it) {
        out.append(it.key());
    }
    return out;
}

void Doc_DirLister::deleteImage( const QString& )
{
}

void Doc_DirLister::thumbNail( const QString& str, int w, int h) {
    if (m_namemap.find(str)==m_namemap.end()) {
        return;
    }
    QString fname = m_namemap[str];
    SlaveMaster::self()->thumbNail( fname, w, h );
}

QImage Doc_DirLister::image( const QString& str, Factor f, int m) {
    if (m_namemap.find(str)==m_namemap.end()) {
        return QImage();
    }
    QString fname = m_namemap[str];
    return SlaveMaster::self()->image( fname, f, m );
}

void Doc_DirLister::imageInfo( const QString& str) {
    if (m_namemap.find(str)==m_namemap.end()) {
        return;
    }
    QString fname = m_namemap[str];
    SlaveMaster::self()->thumbInfo( fname );
}

void Doc_DirLister::fullImageInfo( const QString& str) {
    if (m_namemap.find(str)==m_namemap.end()) {
        return;
    }
    QString fname = m_namemap[str];
    SlaveMaster::self()->imageInfo( fname );
}

void Doc_DirLister::slotFullInfo(const QString&f, const QString&t)
{
    if (m_filemap.find(f)==m_filemap.end()) {
        return;
    }
    QString name = m_filemap[f];
    emit sig_fullInfo(name, t);
}

void Doc_DirLister::slotThumbInfo(const QString&f, const QString&t)
{
    if (m_filemap.find(f)==m_filemap.end()) {
        return;
    }
    QString name = m_filemap[f];
    emit sig_thumbInfo(name, t);
}

void Doc_DirLister::slotThumbNail(const QString&f, const QPixmap&p)
{
    if (m_filemap.find(f)==m_filemap.end()) {
        return;
    }
    QString name = m_filemap[f];
    emit sig_thumbNail(name, p);
}

QString Doc_DirLister::nameToFname(const QString&name)const
{
    if (m_namemap.find(name)==m_namemap.end()) {
        return QString::null;
    }
    return m_namemap[name];
}

QString Doc_DirLister::dirUp( const QString& p ) const{
    return p;
}

QWidget* Doc_DirLister::widget(QWidget*parent)
{
    CategorySelect * catmb = new CategorySelect(parent);
    Categories cats( 0 );
    cats.load( categoryFileName() );
    QArray<int> vl( 0 );
    catmb->setCategories( vl, "Document View", // No tr
        "Document View" );
    catmb->setRemoveCategoryEdit( TRUE );
    catmb->setAllCategories( TRUE );
    connect(catmb, SIGNAL(signalSelected(int)), this, SLOT(showCategory(int)));
    catmb->setCurrentCategory(-2);
    return catmb;
}

void Doc_DirLister::showCategory(int which)
{
    m_catFilter = which==-2?0:which;
    m_namemap.clear();
    setStartPath("");
    emit sig_reloadDir();
}
