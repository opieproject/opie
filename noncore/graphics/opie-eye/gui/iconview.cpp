/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#include "iconview.h"

#include <lib/imagecache.h>
#include <gui/imageinfoui.h>

#include <iface/dirview.h>
#include <iface/dirlister.h>

#include <opie2/oconfig.h>
#include <opie2/okeyconfigwidget.h>
#include <opie2/odebug.h>
#include <opie2/oimagescrollview.h>

#include <qpe/resource.h>
#include <qpe/qpemessagebox.h>
#include <qpe/ir.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include <qiconview.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qtimer.h>
#include <qstyle.h>


using Opie::Core::OKeyConfigItem;

/*
 * The Icons, Request Cache and IconViewItem for the IconView
 */
namespace {
    static QPixmap* _dirPix = 0;
    static QPixmap* _unkPix = 0;
    static QPixmap* _picPix = 0;
    static QPixmap* _emptyPix = 0;
    class IconViewItem : public QIconViewItem {
    public:
        IconViewItem( QIconView*, const QString& path, const QString& name, bool isDir = false);
        QPixmap* pixmap()const;
        QString path()const { return m_path; }
        bool isDir()const { return m_isDir; }
        void setText( const QString& );
        bool textOnly()const{return m_textOnly;}
        void setTextOnly(bool how){m_textOnly=how;}
        /* just for starting recalc of item rect! */
        virtual void setPixmap( const QPixmap & icon, bool recalc, bool redraw = TRUE );
        /* just for starting recalc of item rect! */
        virtual void setPixmap( const QPixmap & icon);

    protected:
        mutable QPixmap* m_pix;

    private:
        QString m_path;
        bool m_isDir : 1;
        bool m_noInfo :1;
        bool m_textOnly:1;
        bool m_NameOnly:1;
        bool m_Pixset:1;
    };
    class TextViewItem : public IconViewItem {
        TextViewItem( QIconView*, const QString& path, const QString& name, bool isDir = false );
        QPixmap *pixmap()const;
        void setText( const QString& );
    };
    class ThumbViewItem : public IconViewItem {
        ThumbViewItem( QIconView*, const QString& path, const QString& name, bool isDir = false );
        QPixmap *pixmap()const;
        void setText( const QString& );
    };


/*
 * If we request an Image or String
 * we add it to the map
 */
    static QMap<QString, IconViewItem*> g_stringInf;
    static QMap<QString, IconViewItem*> g_stringPix;

    IconViewItem::IconViewItem( QIconView* view,const QString& path,
                                const QString& name, bool isDir )
        : QIconViewItem( view, name ), m_path( path ), m_isDir( isDir ),
          m_noInfo( false ),m_textOnly(false),m_Pixset(false)
    {
        if ( isDir && !_dirPix )
            _dirPix = new QPixmap( Resource::loadPixmap("advancedfm/FileBrowser"));
        else if ( !isDir && !_unkPix )
            _unkPix = new QPixmap( Resource::loadPixmap( "UnknownDocument" ) );
    }

    inline void IconViewItem::setPixmap( const QPixmap & icon, bool recalc, bool redraw)
    {
        m_Pixset = true;
        calcRect(text());
    }
    inline void IconViewItem::setPixmap( const QPixmap & icon)
    {
        m_Pixset = true;
        calcRect(text());
    }

    inline QPixmap* IconViewItem::pixmap()const {
//      qWarning(  "Name is " + m_path.right( 15 ) + " rect is %d %d %d %d | %d %d",
//                 rect().x(),rect().y(),rect().width(),rect().height(),
//                 iconView()->contentsX(), iconView()->contentsY());

        if (textOnly()&&!m_isDir) {
            if (!_emptyPix) _emptyPix = new QPixmap(0,0,1);
            return _emptyPix;
        }
        if ( m_isDir )
            return _dirPix;
        else{
            if (!m_noInfo && !g_stringInf.contains( m_path ) ) {
                currentView()->dirLister()->imageInfo( m_path );
                g_stringInf.insert( m_path, const_cast<IconViewItem*>(this));
            }

            m_pix = PPixmapCache::self()->cachedImage( m_path, 64, 64 );
            if (!m_pix && !g_stringPix.contains( m_path )) {
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


/*
 * Set up the GUI.. initialize the slave set up gui
 * and also load a dir
 */
PIconView::PIconView( QWidget* wid, Opie::Core::OConfig* cfg )
    : QVBox( wid ), m_cfg( cfg ), m_updatet( false )
{
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refUp()" );
    }
    m_path = QDir::homeDirPath();
    m_mode = 0;

    QHBox *hbox = new QHBox( this );
    QLabel* lbl = new QLabel( hbox );
    lbl->setText(  tr("View as" ) );

    m_views = new QComboBox( hbox, "View As" );
    connect( m_views, SIGNAL(activated(int)),
             this, SLOT(slotViewChanged(int)) );

    m_view= new QIconView( this );
    connect(m_view, SIGNAL(clicked(QIconViewItem*) ),
            this, SLOT(slotClicked(QIconViewItem*)) );
    connect(m_view, SIGNAL(returnPressed(QIconViewItem*)),
            this, SLOT(slotClicked(QIconViewItem*)) );

    m_view->setArrangement( QIconView::LeftToRight );

    m_mode = cfg->readNumEntry("ListViewMode", 1);

    if (m_mode < 1 || m_mode>3) m_mode = 1;

    m_view->setItemTextPos( QIconView::Right );

    calculateGrid();

    initKeys();

    loadViews();
    slotViewChanged(  m_views->currentItem() );
}

/*
 * Unref the slave and save the keyboard manager
 */
PIconView::~PIconView() {
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refDown()" );
    }
    m_viewManager->save();
    delete m_viewManager;
}

Opie::Core::OKeyConfigManager* PIconView::manager() {
    return m_viewManager;
}


/*
 * init the KeyBoard Shortcuts
 * called from the c'tor
 */
void PIconView::initKeys() {
    Opie::Core::OKeyPair::List lst;
    lst.append( Opie::Core::OKeyPair::upArrowKey() );
    lst.append( Opie::Core::OKeyPair::downArrowKey() );
    lst.append( Opie::Core::OKeyPair::leftArrowKey() );
    lst.append( Opie::Core::OKeyPair::rightArrowKey() );
    lst.append( Opie::Core::OKeyPair::returnKey() );

    m_viewManager = new Opie::Core::OKeyConfigManager(m_cfg, "View-KeyBoard-Config",
                                                    lst, false,this, "keyconfig name" );
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("Beam Current Item") , "beam",
                                                Resource::loadPixmap("beam"), BeamItem,
                                                Opie::Core::OKeyPair(Qt::Key_B, Qt::ShiftButton),
                                                this, SLOT(slotBeam())) );
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("Delete Current Item"), "delete",
                                                Resource::loadPixmap("trash"), DeleteItem,
                                                Opie::Core::OKeyPair(Qt::Key_D, Qt::ShiftButton),
                                                this, SLOT(slotTrash())) );
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("View Current Item"), "view",
                                                Resource::loadPixmap("1to1"), ViewItem,
                                                Opie::Core::OKeyPair(Qt::Key_V, Qt::ShiftButton),
                                                this, SLOT(slotShowImage())));
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("Show Image Info") , "info",
                                                Resource::loadPixmap("DocumentTypeWord"), InfoItem,
                                                Opie::Core::OKeyPair(Qt::Key_I, Qt::ShiftButton ),
                                                this, SLOT(slotImageInfo()) ) );
    m_viewManager->load();
    m_viewManager->handleWidget( m_view );
}


/*
 * change one dir up
 */
void PIconView::slotDirUp()
{
    if (m_path.isEmpty()) return;
    QDir dir( m_path );
    dir.cdUp();
    slotChangeDir( dir.absPath() );
}

/*
 * change the dir
 */
void PIconView::slotChangeDir(const QString& path) {
    if ( !currentView() )
        return;

    PDirLister *lister = currentView()->dirLister();
    if (!lister )
        return;

    /*
     * Say what we want and take what we get
     */
    lister->setStartPath(  path );
    m_path = lister->currentPath();

    m_view->viewport()->setUpdatesEnabled( false );
    m_view->clear();

    /*
     * add files and folders
     */
    addFolders( lister->folders() );
    addFiles( lister->files() );
    m_view->viewport()->setUpdatesEnabled( true );

    // Also invalidate the cache. We can't cancel the operations anyway
    g_stringPix.clear();
    g_stringInf.clear();

    // looks ugly
    static_cast<QMainWindow*>(parent())->setCaption( QObject::tr("%1 - O View", "Name of the dir").arg( m_path ) );
}

/**
 * get the current file name
 * @param isDir see if this is a dir or real file
 */
QString PIconView::currentFileName(bool &isDir)const {
    isDir = false;
    QIconViewItem* _it = m_view->currentItem();
    if ( !_it )
        return QString::null;

    IconViewItem* it = static_cast<IconViewItem*>( _it );
    isDir = it->isDir();
    return it->path();
}

QString PIconView::nextFileName(bool &isDir)const
{
    isDir = false;
    QIconViewItem* _it1 = m_view->currentItem();
    if ( !_it1 )
        return QString::null;
    QIconViewItem* _it = _it1->nextItem();
    if ( !_it )
        return QString::null;
    IconViewItem* it = static_cast<IconViewItem*>( _it );
    isDir = it->isDir();
    return it->path();
}

QString PIconView::prevFileName(bool &isDir)const{
    isDir = false;
    QIconViewItem* _it = m_view->currentItem();
    if ( !_it )
        return QString::null;
    _it = _it->prevItem();
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

/*
 * see what views are available
 */
void PIconView::loadViews() {
    ViewMap::Iterator it;
    ViewMap* map = viewMap();
    for ( it = map->begin(); it != map->end(); ++it )
        m_views->insertItem(  it.key()  );
}

void PIconView::resetView() {
    slotViewChanged(m_views->currentItem());
}

/*
 *swicth view reloadDir and connect signals
 */
void PIconView::slotViewChanged( int i) {
    if (!m_views->count() ) {
        setCurrentView( 0l);
        return;
    }

    PDirView* cur = currentView();
    if (cur) delete cur;
    QString str = m_views->text(i);
    ViewMap* map = viewMap();
    if (!map) {setCurrentView(0l); return;}
    if (map->find(str) == map->end()) {
        owarn << "Key not found" << oendl;
        setCurrentView(0l); return;
    }
    cur = (*(*map)[str])(*m_cfg);
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


/*
 * add files and folders
 */
void PIconView::addFolders(  const QStringList& lst) {
    QStringList::ConstIterator it;
    IconViewItem * _iv;

    for(it=lst.begin(); it != lst.end(); ++it ) {
        _iv = new IconViewItem( m_view, m_path+"/"+(*it), (*it), true );
        if (m_mode==3) _iv->setTextOnly(true);
    }
}

void PIconView::addFiles(  const QStringList& lst) {
    QStringList::ConstIterator it;
    IconViewItem * _iv;
    QPixmap*m_pix = 0;
    QString pre = "";
    if (!m_path.isEmpty()) {
        pre = m_path+"/";
    }
    for (it=lst.begin(); it!= lst.end(); ++it ) {
         m_pix = PPixmapCache::self()->cachedImage( pre+(*it), 64, 64 );
        _iv = new IconViewItem( m_view, pre+(*it), (*it) );
        if (m_mode==3) {
            _iv->setTextOnly(true);
            _iv->setPixmap(QPixmap());
        } else {
            if (m_pix) _iv->setPixmap(*m_pix);
        }
    }

}

/*
 * user clicked on the item. Change dir or view
 */
void PIconView::slotClicked(QIconViewItem* _it) {
    if(!_it )
        return;

    IconViewItem* it = static_cast<IconViewItem*>(_it);
    if( it->isDir() )
        slotChangeDir( it->path() );
    else // view image
        slotShowImage();
}

/*
 * got thumb info add to the cache if items is visible
 * we later need update after processing of slave is done
 */
void PIconView::slotThumbInfo( const QString& _path, const QString& str ) {
    IconViewItem* item = g_stringInf[_path];
    if (!item )
        return;

    if (m_mode == 2) {
        return;
    }
    if ( item->intersects(QRect( m_view->contentsX(),m_view->contentsY(),
                                 m_view->contentsWidth(), m_view->contentsHeight() ) ) )
        m_updatet = true;

    item->setText( str );
    g_stringInf.remove( _path );
}

/*
 * got thumbnail and see if it is visible so we need to update later
 */
void PIconView::slotThumbNail(const QString& _path, const QPixmap &pix) {
    IconViewItem* item = g_stringPix[_path];
    if (!item )
        return;

    if ( item->intersects(QRect( m_view->contentsX(),m_view->contentsY(),
                                 m_view->contentsWidth(), m_view->contentsHeight() ) ) )
        m_updatet = true;

    if (pix.width()>0) {
        PPixmapCache::self()->insertImage( _path, pix, 64, 64 );
        item->setPixmap(pix,true);
    } else {
        PPixmapCache::self()->insertImage( _path, Resource::loadPixmap( "UnknownDocument" ), 64, 64 );
    }
    g_stringPix.remove( _path );
}


/*
 * FIXME rename
 */
void PIconView::slotRename() {

}


/*
 * BEAM the current file
 */
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

/*
 * BEAM done clean up
 */
void PIconView::slotBeamDone( Ir* ir) {
    delete ir;
}

void PIconView::slotStart() {
    m_view->viewport()->setUpdatesEnabled( false );
}

void PIconView::slotEnd() {
    if ( m_updatet )
        m_view->arrangeItemsInGrid( );
    m_view->viewport()->setUpdatesEnabled( true );
    m_updatet = false;
}

void PIconView::slotShowNext()
{
    bool isDir = false;
    QString name = nextFileName(isDir);
    if (name.isEmpty()) return;
    if (isDir) return;
    /* if we got a name we have a next item */
    m_view->setCurrentItem(m_view->currentItem()->nextItem());
    slotShowImage(name);
}

void PIconView::slotShowPrev()
{
    bool isDir = false;
    QString name = prevFileName(isDir);
    if (name.isEmpty()) return;
    if (isDir) return;
    /* if we got a name we have a prev item */
    m_view->setCurrentItem(m_view->currentItem()->prevItem());
    slotShowImage(name);
}

void PIconView::slotShowImage()
{
    bool isDir = false;
    QString name = currentFileName(isDir);
    if (isDir) return;
    slotShowImage( name );
}
void PIconView::slotShowImage( const QString& name) {
    PDirLister *lister = currentView()->dirLister();
    QString r_name = lister->nameToFname(name);
    emit sig_display( r_name );
}
void PIconView::slotImageInfo() {
    bool isDir = false;
    QString name = currentFileName(isDir);
    if (isDir) return;

    slotImageInfo( name );
}

void PIconView::slotImageInfo( const QString& name) {
    PDirLister *lister = currentView()->dirLister();
    QString r_name = lister->nameToFname(name);
    emit sig_showInfo(r_name );
}


void PIconView::slotChangeMode( int mode ) {
    if ( mode >= 1 && mode <= 3 )
        m_mode = mode;

    m_cfg->writeEntry("ListViewMode", m_mode);
    /* performance! */
    m_view->clear();
    calculateGrid();
    slotReloadDir();
}


void PIconView::resizeEvent( QResizeEvent* re ) {
    QVBox::resizeEvent( re );
    calculateGrid();
}


void PIconView::calculateGrid() {
    int dw = QApplication::desktop()->width();
    int viewerWidth = dw-style().scrollBarExtent().width();

    QIconView::ItemTextPos pos;
    switch( m_mode ) {
    case 2:
        pos = QIconView::Bottom;
        break;
    case 3:
    case 1:
    default:
        pos = QIconView::Right;
        break;
    }
    m_view->setItemTextPos( pos );
    switch (m_mode) {
        case 2:
            m_view->setGridX(50);
            m_view->setGridY(20);
            PPixmapCache::self()->setMaxImages(40);
            break;
        case 3:
            m_view->setGridX( fontMetrics().width("testimage.jpg")+20);
            m_view->setGridY(8);
            PPixmapCache::self()->setMaxImages(2);
            break;
        case 1:
        default:
            m_view->setGridX( viewerWidth-3*m_view->spacing());
            m_view->setGridY( fontMetrics().height()*2+40 );
            PPixmapCache::self()->setMaxImages(20);
            break;
    }
}
