/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#include "iconview.h"

#include <lib/imagecache.h>

#include <iface/dirview.h>
#include <iface/dirlister.h>

#include <opie2/oconfig.h>
#include <opie2/okeyconfigwidget.h>
#include <opie2/odebug.h>

#include <qpe/resource.h>
#include <qpe/qpemessagebox.h>
#include <qpe/ir.h>
#include <qpe/qcopenvelope_qws.h>


#include <qiconview.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qtimer.h>
#include <qstyle.h>


using Opie::Ui::OKeyConfigItem;

namespace {
    QPixmap* _dirPix = 0;
    QPixmap* _unkPix = 0;
    class IconViewItem : public QIconViewItem {
    public:
        IconViewItem( QIconView*, const QString& path, const QString& name, bool isDir = false);
        QPixmap* pixmap()const;
        QString path()const { return m_path; }
        bool isDir()const { return m_isDir; }
        void setText( const QString& );


    private:
        mutable QPixmap* m_pix;
        QString m_path;
        bool m_isDir : 1;
        bool m_noInfo :1;
    };


/*
 * If we request an Image or String
 * we add it to the map
 */
    QMap<QString, IconViewItem*> g_stringInf;
    QMap<QString, IconViewItem*> g_stringPix;

    IconViewItem::IconViewItem( QIconView* view,const QString& path,
                                const QString& name, bool isDir )
        : QIconViewItem( view ), m_path( path ), m_isDir( isDir ),
          m_noInfo( false )
    {
        QIconViewItem::setText( name );
        if ( isDir && !_dirPix )
            _dirPix = new QPixmap( Resource::loadPixmap("advancedfm/FileBrowser"));
        else if ( !isDir && !_unkPix )
            _unkPix = new QPixmap( Resource::loadPixmap( "UnknownDocument" ) );
    }
    inline QPixmap* IconViewItem::pixmap()const {
        qWarning(  "Name is " + m_path.right( 15 ) + " rect is %d %d %d %d | %d %d",
                   rect().x(),rect().y(),rect().width(),rect().height(),
                   iconView()->contentsX(), iconView()->contentsY());

        if ( m_isDir )
            return _dirPix;
        else{
            if (!m_noInfo && !g_stringInf.contains( m_path ) ) {
                currentView()->dirLister()->imageInfo( m_path );
                g_stringInf.insert( m_path, const_cast<IconViewItem*>(this));
            }

            m_pix = PPixmapCache::self()->cachedImage( m_path, 64, 64 );
            if ( !m_pix && !g_stringPix.contains( m_path )) {
                currentView()->dirLister()->thumbNail( m_path, 64, 64 );
                g_stringPix.insert( m_path, const_cast<IconViewItem*>(this));
            }
            return m_pix ? m_pix : _unkPix;
        }
    }
    inline void IconViewItem::setText( const QString& str ) {
        QString text = QIconViewItem::text()+"\n"+str;
        m_noInfo = true;
        QIconViewItem::setText( text );
    }
}


PIconView::PIconView( QWidget* wid, Opie::Core::OConfig* cfg )
    : QVBox( wid ), m_cfg( cfg )
{
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refUp()" );
    }
    m_path = QDir::homeDirPath();

    QHBox *hbox = new QHBox( this );
    QLabel* lbl = new QLabel( hbox );
    lbl->setText(  tr("View as" ) );

    m_views = new QComboBox( hbox, "View As" );
    connect( m_views, SIGNAL(activated(int)),
             this, SLOT(slotViewChanged(int)) );

    m_view= new QIconView( this );
    connect(m_view, SIGNAL(clicked(QIconViewItem*) ),
            this, SLOT(slotClicked(QIconViewItem*)) );

    m_view->setArrangement( QIconView::LeftToRight );
    m_view->setItemTextPos( QIconView::Right );

    int dw = QApplication::desktop()->width();
    int viewerWidth = dw-style().scrollBarExtent().width();
    m_view->setGridX( viewerWidth-3*m_view->spacing());
    m_view->setGridY( fontMetrics().height()*2+40 );


    initKeys();

    loadViews();
    slotViewChanged(  m_views->currentItem() );
}

PIconView::~PIconView() {
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refDown()" );
    }
    m_viewManager->save();
    delete m_viewManager;
}

Opie::Ui::OKeyConfigManager* PIconView::manager() {
    return m_viewManager;
}

void PIconView::initKeys() {
    Opie::Ui::OKeyPair::List lst;
    lst.append( Opie::Ui::OKeyPair::upArrowKey() );
    lst.append( Opie::Ui::OKeyPair::downArrowKey() );
    lst.append( Opie::Ui::OKeyPair::leftArrowKey() );
    lst.append( Opie::Ui::OKeyPair::rightArrowKey() );
    lst.append( Opie::Ui::OKeyPair::returnKey() );

    m_viewManager = new Opie::Ui::OKeyConfigManager(m_cfg, "View-KeyBoard-Config",
                                                    lst, false,this, "keyconfig name" );
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("Beam Current Item") , "beam",
                                                QString::fromLatin1("beam"), BeamItem,
                                                Opie::Ui::OKeyPair(Qt::Key_B, Qt::ShiftButton),
                                                this, SLOT(slotBeam())) );
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("Delete Current Item"), "delete",
                                                QString::fromLatin1("trash"), DeleteItem,
                                                Opie::Ui::OKeyPair(Qt::Key_D, Qt::ShiftButton),
                                                this, SLOT(slotTrash())) );
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("View Current Item"), "view",
                                                QString::fromLatin1("1to1"), ViewItem,
                                                Opie::Ui::OKeyPair(Qt::Key_V, Qt::ShiftButton),
                                                this, SLOT(slotShowImage())));
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("Show Image Info") , "info",
                                                QString::fromLatin1("DocumentTypeWord"), InfoItem,
                                                Opie::Ui::OKeyPair(Qt::Key_I, Qt::ShiftButton ),
                                                this, SLOT(slotImageInfo()) ) );
    m_viewManager->load();
    m_viewManager->handleWidget( m_view );
}

void PIconView::slotDirUp() {
    QDir dir( m_path );
    dir.cdUp();
    slotChangeDir( dir.absPath() );

}

void PIconView::slotChangeDir(const QString& path) {
    if ( !currentView() )
        return;

    PDirLister *lister = currentView()->dirLister();
    if (!lister )
        return;

    lister->setStartPath(  path );
    m_path = lister->currentPath();

    m_view->viewport()->setUpdatesEnabled( false );
    m_view->clear();
    addFolders( lister->folders() );
    addFiles( lister->files() );
    m_view->viewport()->setUpdatesEnabled( true );

    // Also invalidate the cache. We can't cancel the operations anyway
    g_stringPix.clear();
    g_stringInf.clear();

    // looks ugly
    static_cast<QMainWindow*>(parent())->setCaption( QObject::tr("%1 - O View", "Name of the dir").arg( m_path ) );
}

QString PIconView::currentFileName(bool &isDir)const {
    isDir = false;
    QIconViewItem* _it = m_view->currentItem();
    if ( !_it )
        return QString::null;

    IconViewItem* it = static_cast<IconViewItem*>( _it );
    isDir = it->isDir();
    return it->path();
}

void PIconView::slotTrash() {
    bool isDir;
    QString pa = currentFileName( isDir );
    if ( isDir && pa.isEmpty() )
        return;

    if (!QPEMessageBox::confirmDelete( this,
                                      tr("Delete Image" ),
                                      tr("the Image %1" ).arg(pa)))
        return


    currentView()->dirLister()->deleteImage( pa );
    delete m_view->currentItem();
}
void PIconView::loadViews() {
    ViewMap::Iterator it;
    ViewMap* map = viewMap();
    for ( it = map->begin(); it != map->end(); ++it )
        m_views->insertItem(  QObject::tr(it.key() ) );
}

void PIconView::resetView() {
    slotViewChanged(m_views->currentItem());
}

void PIconView::slotViewChanged( int i) {
    if (!m_views->count() ) {
        setCurrentView( 0l);
        return;
    }

    PDirView* cur = currentView();
    delete cur;
    QString str = m_views->text(i);
    cur = (*(*viewMap())[str])(*m_cfg);
    setCurrentView( cur );

    /* connect to the signals of the lister */
    PDirLister* lis = cur->dirLister();
    connect(lis, SIGNAL(sig_thumbInfo(const QString&, const QString& )),
            this, SLOT( slotThumbInfo(const QString&, const QString&)));
    connect(lis, SIGNAL( sig_thumbNail(const QString&, const QPixmap&)),
            this, SLOT(slotThumbNail(const QString&, const QPixmap&)));
    connect(lis, SIGNAL(sig_start()),
            this, SLOT(slotStart()));
    connect(lis, SIGNAL(sig_end()) ,
            this, SLOT(slotEnd()) );


    /*  reload now */
    QTimer::singleShot( 0,  this, SLOT(slotReloadDir()));
}


void PIconView::slotReloadDir() {
    slotChangeDir( m_path );
}


void PIconView::addFolders(  const QStringList& lst) {
    QStringList::ConstIterator it;

    for(it=lst.begin(); it != lst.end(); ++it ) {
        (void)new IconViewItem( m_view, m_path+"/"+(*it), (*it), true );
    }

}

void PIconView::addFiles(  const QStringList& lst) {
    QStringList::ConstIterator it;
    for (it=lst.begin(); it!= lst.end(); ++it )
        (void)new IconViewItem( m_view, m_path+"/"+(*it), (*it) );

}

void PIconView::slotClicked(QIconViewItem* _it) {
    if(!_it )
        return;

    IconViewItem* it = static_cast<IconViewItem*>(_it);
    if( it->isDir() )
        slotChangeDir( it->path() );
    else // view image
        ;
}

void PIconView::slotThumbInfo( const QString& _path, const QString& str ) {
    if ( g_stringInf.contains( _path ) ) {
        IconViewItem* item = g_stringInf[_path];
        /* if set the view shows nonsens!
           I dont know how to fix the format of displayed text :(*/
        item->setText( str );
        g_stringInf.remove( _path );
    }
}
void PIconView::slotThumbNail(const QString& _path, const QPixmap &pix) {
    if ( g_stringPix.contains( _path ) ) {
        IconViewItem* item = g_stringPix[_path];

        if (pix.width()>0) {
            PPixmapCache::self()->insertImage( _path, pix, 64, 64 );
            /* required for a recalculated rectangle. otherwise the view show nonsense! */
        } else {
            PPixmapCache::self()->insertImage(_path,Resource::loadPixmap( "UnknownDocument" ),64,64 );
        }
        g_stringPix.remove( _path );
    }
}


void PIconView::slotRename() {

}

void PIconView::slotBeam() {
    bool isDir;
    QString pa = currentFileName( isDir );
    if ( isDir && pa.isEmpty() )
        return;

    Ir* ir = new Ir( this );
    connect( ir, SIGNAL(done(Ir*)),
             this, SLOT(slotBeamDone(Ir*)));
    ir->send(pa, tr( "Image" ) );

}

void PIconView::slotBeamDone( Ir* ir) {
    delete ir;
}

void PIconView::slotStart() {
    m_view->viewport()->setUpdatesEnabled( false );
}

void PIconView::slotEnd() {
    m_view->arrangeItemsInGrid( );
    m_view->viewport()->setUpdatesEnabled( true );
}

void PIconView::slotShowImage() {

}
void PIconView::slotShowImage( const QString& ) {

}
void PIconView::slotImageInfo() {

}
void PIconView::slotImageInfo( const QString& ) {

}
