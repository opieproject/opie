/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#include "iconview.h"
#include "messagebox.h"

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

const int PIconView::sMAX_ICON_SIZE = 128;
const int PIconView::sMIN_ICON_SIZE = 12;
const int PIconView::sDEF_ICON_SIZE = 64;

/*
 * The Icons, Request Cache and IconViewItem for the IconView
 */
namespace {
    static QPixmap* _dirPix = 0;
    static QPixmap* _unkPix = 0;
    static QPixmap* _cpyPix = 0;
    static QPixmap* _emptyPix = 0;
    class IconViewItem : public QIconViewItem {
    public:
        IconViewItem( QIconView*, const QString& path, const QString& name,int a_iconsize, bool isDir = false);
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
        int m_iconsize;
        void check_pix()const;

    private:
        QString m_path;
        bool m_isDir : 1;
        bool m_noInfo :1;
        bool m_textOnly:1;
        bool m_NameOnly:1;
        bool m_Pixset:1;
    };
    class TextViewItem : public IconViewItem {
        TextViewItem( QIconView*, const QString& path, const QString& name, int a_iconsize , bool isDir = false);
        QPixmap *pixmap()const;
        void setText( const QString& );
    };
    class ThumbViewItem : public IconViewItem {
        ThumbViewItem( QIconView*, const QString& path, const QString& name, int a_iconsize, bool isDir = false );
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
                                const QString& name, int a_iconsize, bool isDir)
        : QIconViewItem( view, name ), m_path( path ), m_isDir( isDir ),
          m_noInfo( false ),m_textOnly(false),m_Pixset(false)
    {
        m_iconsize = a_iconsize;
        if ( isDir ) {
            if (!_dirPix ) {
                _dirPix = new QPixmap( Resource::loadPixmap("advancedfm/FileBrowser"));
            }
        } else {
            if (!_unkPix ) {
                _unkPix = new QPixmap( Resource::loadPixmap( "UnknownDocument" ) );
            }
        }
        check_pix();
    }

    inline void IconViewItem::check_pix()const
    {
        if (_dirPix && _dirPix->width()>m_iconsize) {
            QImage Pix = _dirPix->convertToImage();
            *_dirPix = Pix.smoothScale(m_iconsize,m_iconsize);
        }
        if (!_cpyPix && _unkPix) {
            if (_unkPix->width()>=m_iconsize) {
                QImage Pix = _unkPix->convertToImage();
                _cpyPix = new QPixmap();
                if (_unkPix->width()>m_iconsize) {
                    *_cpyPix = Pix.smoothScale(m_iconsize,m_iconsize);
                } else {
                    _cpyPix->convertFromImage(Pix);
                }

            } else {
                _cpyPix = new QPixmap(m_iconsize,m_iconsize);
                _cpyPix->fill();
                QPainter pa(_cpyPix);
                int offset = (m_iconsize-_unkPix->width())/2;
                int offy = (m_iconsize-_unkPix->height())/2;
                if (offy<0) offy=0;
                pa.drawPixmap(offset,offy,*_unkPix);
                pa.end();
            }
        }
    }

    inline void IconViewItem::setPixmap( const QPixmap & , bool, bool )
    {
        m_Pixset = true;
        calcRect(text());
    }
    inline void IconViewItem::setPixmap( const QPixmap & )
    {
        m_Pixset = true;
        calcRect(text());
    }

    inline QPixmap* IconViewItem::pixmap()const {
//      owarn << "Name is " << m_path.right( 15 ) << " rect is " << rect().x() << " " << rect().y()
//            << " " << rect().width() << " " << rect().height() << " | " << iconView()->contentsX()
//            << " " << iconView()->contentsY() << oendl;

        if (textOnly()&&!m_isDir) {
            if (!_emptyPix) _emptyPix = new QPixmap(0,0,1);
            return _emptyPix;
        }
        if ( m_isDir )
            return _dirPix;
        else{
            if (!m_noInfo && !g_stringInf.contains( m_path ) ) {
                g_stringInf.insert( m_path, const_cast<IconViewItem*>(this));
                currentView()->dirLister()->imageInfo( m_path );
            }

            m_pix = PPixmapCache::self()->cachedImage( m_path, m_iconsize, m_iconsize );
            if (!m_pix && !g_stringPix.contains( m_path )) {
                check_pix();
                g_stringPix.insert( m_path, const_cast<IconViewItem*>(this));
                currentView()->dirLister()->thumbNail( m_path, m_iconsize, m_iconsize);
            }
            return m_pix ? m_pix : _cpyPix;
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
    m_iconsize = 32;
    m_internalReset = false;
    m_customWidget = 0;
    m_setDocCalled = false;

    m_hbox = new QHBox( this );
    QLabel* lbl = new QLabel( m_hbox );
    lbl->setText(  tr("View as" ) );

    m_views = new QComboBox( m_hbox, "View As" );

    m_view= new QIconView( this );
    connect(m_view, SIGNAL(clicked(QIconViewItem*) ),
            this, SLOT(slotClicked(QIconViewItem*)) );
    connect(m_view, SIGNAL(returnPressed(QIconViewItem*)),
            this, SLOT(slotClicked(QIconViewItem*)) );

    m_view->setArrangement( QIconView::LeftToRight );

    m_mode = m_cfg->readNumEntry("ListViewMode", 1);
    if (m_mode < 1 || m_mode>3) m_mode = 1;
    m_view->setItemTextPos( QIconView::Right );
    if (m_mode >1) {
        m_view->setResizeMode(QIconView::Adjust);
    } else {
        m_view->setResizeMode(QIconView::Fixed);
    }
    m_iconsize = m_cfg->readNumEntry("iconsize", 32);
    if (m_iconsize<sMIN_ICON_SIZE)m_iconsize = sMIN_ICON_SIZE;
    if (m_iconsize>sMAX_ICON_SIZE)m_iconsize = sMAX_ICON_SIZE;

    calculateGrid();
    initKeys();
    loadViews();
}

void PIconView::setDoccalled(bool how)
{
    m_setDocCalled = how;
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
    m_viewManager->addKeyConfig( OKeyConfigItem(tr("Start slideshow"), "slideshow",
                                                Resource::loadPixmap("1to1"), SlideItem,
                                                Opie::Core::OKeyPair(Qt::Key_S, Qt::ShiftButton),
                                                this, SLOT(slotStartSlide())));
    m_viewManager->load();
    m_viewManager->handleWidget( m_view );
}


/*
 * change one dir up
 */
void PIconView::slotDirUp()
{
    slotChangeDir( currentView()->dirLister()->dirUp( m_path ) );
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

    // Also invalidate the cache. We can't cancel the operations anyway
    g_stringPix.clear();
    g_stringInf.clear();

    /*
     * add files and folders
     */
    addFolders( lister->folders() );
    addFiles( lister->files() );
    m_view->viewport()->setUpdatesEnabled( true );

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
    if ( isDir || pa.isEmpty() )
        return;

    if (!OMessageBox::confirmDelete( this, tr("the Image"),
                                     pa, tr("Delete Image" )))
        return;


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
    m_internalReset = true;
    // Also invalidate the cache. We can't cancel the operations anyway
    g_stringPix.clear();
    g_stringInf.clear();
    if (m_mode>1) {
        int osize = m_iconsize;
        m_iconsize = m_cfg->readNumEntry("iconsize", 32);
        if (m_iconsize<sMIN_ICON_SIZE)m_iconsize = sMIN_ICON_SIZE;
        if (m_iconsize>sMAX_ICON_SIZE)m_iconsize = sMAX_ICON_SIZE;
        if (osize != m_iconsize) {
            if (_dirPix){
                delete _dirPix;
                _dirPix = 0;
            }
            if (_cpyPix){
                delete _cpyPix;
                _cpyPix = 0;
            }
            calculateGrid();
        }
    } else {
        m_iconsize = sDEF_ICON_SIZE;
    }
    slotViewChanged(m_views->currentItem());
    m_internalReset = false;
}

void PIconView::polish()
{
    QVBox::polish();

    QString lastView = m_cfg->readEntry("LastView","");
    int cc=0;
    for (; cc<m_views->count();++cc) {
        if (m_views->text(cc)==lastView) {
            break;
        }
    }
    if (cc<m_views->count()) {
        m_views->setCurrentItem(cc);
        slotViewChanged(cc);
    } else {
        slotViewChanged(m_views->currentItem());
    }
    connect( m_views, SIGNAL(activated(int)),
             this, SLOT(slotViewChanged(int)) );
}

/*
 *swicth view reloadDir and connect signals
 */
void PIconView::slotViewChanged( int i) {
    if (!m_views->count() ) {
        setCurrentView( 0l);
        return;
    }

    if (m_customWidget) {
        delete m_customWidget;
        m_customWidget = 0;
    }
    PDirView* cur = currentView();
    if (cur) {
        delete cur;
    }
    QString str = m_views->text(i);
    ViewMap* map = viewMap();
    if (!map) {
        setCurrentView(0l);
        return;
    }

    if (map->find(str) == map->end()) {
        owarn << "Key not found" << oendl;
        setCurrentView(0l);
        return;
    }

    m_cfg->writeEntry("LastView",str);
    m_cfg->write();
    cur = (*(*map)[str])(*m_cfg);
    setCurrentView( cur );
    m_customWidget = cur->widget(m_hbox);
    if (m_customWidget) {
        odebug << "Got a widget" << oendl;
        m_customWidget->show();
    }

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
    connect(lis,SIGNAL(sig_reloadDir()),this,SLOT(slotReloadDir()));

    /*  reload now with default Path
     * but only if it isn't a reset like from setupdlg
     */
    if (!m_internalReset) {
        m_path = lis->defaultPath();
    }
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
        _iv = new IconViewItem( m_view, m_path+"/"+(*it), (*it),m_iconsize, true );
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
    QString s = "";
    int pos;
    for (it=lst.begin(); it!= lst.end(); ++it ) {
        s = (*it);
        pos = s.find(char(0));
        m_pix = PPixmapCache::self()->cachedImage( pre+(*it), m_iconsize, m_iconsize );
        if (pos>-1) {
            _iv = new IconViewItem( m_view, s.mid(pos+1), s.left(pos),m_iconsize );
        } else {
            _iv = new IconViewItem( m_view, pre+(*it), (*it),m_iconsize );
        }
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
 * Return was pressed. which is triggered by the keydown
 * handler. The problem is that the key up will be handled
 * by the ImageDisplayer and goes to the next image
 */
void PIconView::slotRetrun( QIconViewItem *_it ) {
    if(!_it )
        return;

    IconViewItem* it = static_cast<IconViewItem*>(_it);
    if( it->isDir() )
        slotChangeDir( it->path() );
    else
        QTimer::singleShot(0, this, SLOT(slotShowImage()) );
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
        if (pix.width()<m_iconsize) {
            QPixmap p(m_iconsize,m_iconsize);
            p.fill();
            QPainter pa(&p);
            int offset = (m_iconsize-pix.width())/2;
            int offy = (m_iconsize-pix.height())/2;
            if (offy<0) offy=0;
            pa.drawPixmap(offset,offy,pix);
            pa.end();
            PPixmapCache::self()->insertImage( _path, p, m_iconsize, m_iconsize );
            item->setPixmap(p,true);
        } else {
            PPixmapCache::self()->insertImage( _path, pix, m_iconsize, m_iconsize );
            item->setPixmap(pix,true);
        }

    } else {
        PPixmapCache::self()->insertImage( _path, Resource::loadPixmap( "UnknownDocument" ), m_iconsize, m_iconsize );
    }
    g_stringPix.remove( _path );
    m_view->arrangeItemsInGrid(true);
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

void PIconView::slotShowLast()
{
    QIconViewItem* last_it = m_view->lastItem();
    if (!last_it) return;
    m_view->setCurrentItem(last_it);
    IconViewItem* it = static_cast<IconViewItem*>( last_it );
    bool isDir = it->isDir();
    QString name = it->path();
    if (!isDir && !name.isEmpty()) {
        slotShowImage(name);
        return;
    }
    bool first_loop = true;
    while(isDir==true) {
        if (!first_loop) {
            m_view->setCurrentItem(m_view->currentItem()->prevItem());
        } else {
            first_loop = false;
        }
        name = prevFileName(isDir);
    }

    if (name.isEmpty()) return;
    /* if we got a name we have a prev item */
    m_view->setCurrentItem(m_view->currentItem()->prevItem());
    slotShowImage(name);
}

bool PIconView::slotShowFirst()
{
    /* stop when reached - otherwise we may get an endless loop */
    QIconViewItem* first_it = m_view->firstItem();
    if (!first_it) return false;
    m_view->setCurrentItem(first_it);
    IconViewItem* it = static_cast<IconViewItem*>( first_it );
    bool isDir = it->isDir();
    QString name = it->path();
    if (!isDir && !name.isEmpty()) {
        slotShowImage(name);
        return false;
    }
    bool first_loop = true;
    while(isDir==true) {
        /* if name is empty isDir is false, too. */
        if (!first_loop) {
            m_view->setCurrentItem(m_view->currentItem()->nextItem());
        } else {
            first_loop = false;
        }
        name = nextFileName(isDir);
    }
    if (name.isEmpty()) return false;
    /* if we got a name we have a next item */
    m_view->setCurrentItem(m_view->currentItem()->nextItem());
    slotShowImage(name);
    return true;
}

void PIconView::slotShowNext()
{
    bool isDir = false;
    QString name = nextFileName(isDir);
    while (isDir==true) {
        m_view->setCurrentItem(m_view->currentItem()->nextItem());
        name = nextFileName(isDir);
    }
    if (name.isEmpty()) {
        slotShowFirst();
        return;
    }
    if (isDir) return;
    /* if we got a name we have a next item */
    m_view->setCurrentItem(m_view->currentItem()->nextItem());
    slotShowImage(name);
}

void PIconView::slotShowPrev()
{
    bool isDir = false;
    QString name = prevFileName(isDir);
    while (isDir==true) {
        /* if name is empty isDir is false, too. */
        m_view->setCurrentItem(m_view->currentItem()->prevItem());
        name = prevFileName(isDir);
    }
    if (name.isEmpty()) {
        slotShowLast();
        return;
    }
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
    emit sig_display(r_name);
}

void PIconView::slotStartSlide() {
    bool isDir = false;
    QString name = currentFileName(isDir);
    if (isDir) {
        if (!slotShowFirst())
            return;
    } else {
        slotShowImage( name );
    }
    int t = m_cfg->readNumEntry("slideshowtimeout", 2);
    emit sig_startslide(t);
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
    if ( mode >= 1 && mode <= 3 ) {
        m_mode = mode;
        m_cfg->writeEntry("ListViewMode", m_mode);
        /* performance! */
        m_view->clear();
        if (m_mode >1) {
            m_view->setResizeMode(QIconView::Adjust);
        } else {
            m_view->setResizeMode(QIconView::Fixed);
        }
        if (m_mode==1) {
            m_iconsize = sDEF_ICON_SIZE;
        } else {
            m_iconsize = m_cfg->readNumEntry("iconsize", 32);
            if (m_iconsize<sMIN_ICON_SIZE)m_iconsize = sMIN_ICON_SIZE;
            if (m_iconsize>sMAX_ICON_SIZE)m_iconsize = sMAX_ICON_SIZE;
        }
        if (_dirPix){
            delete _dirPix;
            _dirPix = 0;
        }
        if (_cpyPix){
            delete _cpyPix;
            _cpyPix = 0;
        }
        calculateGrid();
        slotReloadDir();
    }
}


void PIconView::resizeEvent( QResizeEvent* re ) {
    calculateGrid(re);
    QVBox::resizeEvent( re );
    //calculateGrid();
}


void PIconView::calculateGrid(QResizeEvent* re)
{
    int viewerWidth;
    if (re) {
        viewerWidth=re->size().width();
    } else {
        int dw = QApplication::desktop()->width();
        viewerWidth = dw-style().scrollBarExtent().width();
    }

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
    int cache = 0;
    m_view->setItemTextPos( pos );
    switch (m_mode) {
        case 2:
            m_view->setSpacing(2);
            m_view->setGridX(m_iconsize);
            m_view->setGridY(-1);
            cache = (int)((double)sDEF_ICON_SIZE/(double)m_iconsize*80.0);
            odebug << "cache size: " << cache << oendl;
            PPixmapCache::self()->setMaxImages(cache);
            break;
        case 3:
            m_view->setSpacing(10);
            m_view->setGridX( fontMetrics().width("testimage.jpg")+20);
            m_view->setGridY(8);
            PPixmapCache::self()->setMaxImages(2);
            break;
        case 1:
        default:
            m_view->setSpacing(10);
            m_view->setGridX( viewerWidth-3*m_view->spacing());
            m_view->setGridY( fontMetrics().height()*2+40 );
            PPixmapCache::self()->setMaxImages(20);
            break;
    }
}
