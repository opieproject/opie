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
#include <opie2/oglobal.h>

#include <qtopia/private/categories.h>
#include <qtopia/categoryselect.h>

using namespace Opie::Core;

/* QT */
#include <qdir.h>
#include <qfileinfo.h>
#include <qtopia/qcopenvelope_qws.h>

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
    m_docreads = false;
    syschannel = new QCopChannel("QPE/System", this);
    connect(syschannel, SIGNAL(received(const QCString&,const QByteArray&)),
        this, SLOT(systemMsg(const QCString&,const QByteArray&)) );
}

Doc_DirLister::~Doc_DirLister()
{
}

QString Doc_DirLister::defaultPath()const {
    return QString::null;
}

bool Doc_DirLister::matchCat(const AppLnk* app)const
{
    if (!app) return false;
    if (m_catFilter==0 || app->categories().contains(m_catFilter) || m_catFilter == -1 && app->categories().count() == 0 ) {
        return true;
    }
    return false;
}

QString Doc_DirLister::setStartPath(const QString&) {
    static const QString Mtype_str("image/jpeg;image/gif;image/bmp;image/png");
    if (!m_docreads) {
        Global::findDocuments(&m_ds,Mtype_str);
        m_docreads = true;
    }
    return QString::null;
}

QString Doc_DirLister::currentPath()const
{
    return QString::null;
}


QStringList Doc_DirLister::folders()const {
    return QStringList();
}

QStringList Doc_DirLister::files()const {
    QStringList out;
    QListIterator<DocLnk> dit(m_ds.children());
    for( ; dit.current(); ++dit) {
        if (!matchCat((*dit))) continue;
        QString s = (*dit)->name();
        s+=char(0);
        s+=(*dit)->file();
        out.append(s);
    }
    return out;
}

void Doc_DirLister::deleteImage( const QString& )
{
}

void Doc_DirLister::thumbNail( const QString& str, int w, int h)
{
    SlaveMaster::self()->thumbNail( str, w, h );
}

QImage Doc_DirLister::image( const QString& str, Factor f, int m)
{
    return SlaveMaster::self()->image(str, f, m );
}

void Doc_DirLister::imageInfo( const QString& str) {
    SlaveMaster::self()->thumbInfo( str );
}

void Doc_DirLister::fullImageInfo( const QString& str) {
    SlaveMaster::self()->imageInfo(str);
}

void Doc_DirLister::slotFullInfo(const QString&f, const QString&t)
{
    emit sig_fullInfo(f, t);
}

void Doc_DirLister::slotThumbInfo(const QString&f, const QString&t)
{
    emit sig_thumbInfo(f, t);
}

void Doc_DirLister::slotThumbNail(const QString&f, const QPixmap&p)
{
    emit sig_thumbNail(f, p);
}

QString Doc_DirLister::nameToFname(const QString&name)const
{
    return name;
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
    setStartPath("");
    emit sig_reloadDir();
}

void Doc_DirLister::systemMsg(const QCString &msg, const QByteArray &data)
{
    if ( msg != "linkChanged(QString)"||!m_docreads) {
        return;
    }
    QString link;
    QDataStream stream( data, IO_ReadOnly );
    stream >> link;
    odebug << "Doc_DirLister systemMsg -> linkchanged( " << link << " )" << oendl;
    if ( link.isNull() || OGlobal::isAppLnkFileName(link) ) {
        return;
    }
    QListIterator<DocLnk> dit(m_ds.children());
    bool must_reload = false;
    bool found = false;
    while ( dit.current() ) {
        DocLnk *doc = dit.current();
        ++dit;
        if (doc->linkFile() == link) {
            found = true;
            DocLnk* dl = new DocLnk(link);
            if (dl->fileKnown()) {
                // changing
                m_ds.add(dl);
            } else {
                delete dl;
            }
            if (matchCat(doc) || matchCat(dl)) {
                must_reload = true;
            }
            m_ds.remove( doc ); // remove old link from docLnkSet
            delete doc;
        }
    }
    if (must_reload) {
        setStartPath("");
        emit sig_reloadDir();
    }
}
