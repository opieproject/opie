/*
                             This file is part of the Opie Project
              =.             (C) 2000-2002 Trolltech AS
            .=l.             (C) 2002-2005 The Opie Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
    :`=1 )Y*s>-.--   :       the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
    :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#include "launcherview.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qpe/config.h>
#include <qtopia/qpeapplication.h>
#include <qtopia/private/categories.h>
#include <qtopia/categoryselect.h>
#include <qtopia/mimetype.h>
using namespace Opie::Core;

#include <qpe/qcopenvelope_qws.h>

/* QT */
#include <qtimer.h>
#include <qfileinfo.h>
#include <qiconview.h>
#include <qobjectlist.h>


// These define how the busy icon is animated and highlighted
#define BRIGHTEN_BUSY_ICON
//#define ALPHA_FADE_BUSY_ICON
//#define USE_ANIMATED_BUSY_ICON_OVERLAY
#define BOUNCE_BUSY_ICON

typedef QMap<QString,QPixmap>::Iterator pixiter;

class BgPixmap
{
public:
    BgPixmap( const QPixmap &p ) : pm(p), ref(1) {}
    QPixmap pm;
    int ref;
};


static QMap<QString,BgPixmap*> *bgCache = 0;

static void cleanup_cache()
{
    QMap<QString,BgPixmap*>::Iterator it = bgCache->begin();
    while ( it != bgCache->end() ) {
    QMap<QString,BgPixmap*>::Iterator curr = it;
    ++it;
    delete (*curr);
    bgCache->remove( curr );
    }
    delete bgCache;
    bgCache = 0;
}


class LauncherItem : public QIconViewItem
{
public:
    enum iconstate_t {
        BASE_ICON,
        WAITING_ICON,
        EYE_ICON
    };

    LauncherItem( QIconView *parent, AppLnk* applnk, bool bigIcon=TRUE );
    ~LauncherItem();

    AppLnk *appLnk() const { return app; }
    AppLnk *takeAppLnk() { AppLnk* r=app; app=0; return r; }

    void animateIcon();
    void resetIcon();
    bool isEyeImage()const{return m_EyeImage;}

    virtual int compare ( QIconViewItem * i ) const;
    void paintItem( QPainter *p, const QColorGroup &cg );

    void setBusyIndicatorType ( BusyIndicatorType t ) { busyType = t; }
    void setEyePixmap(const QPixmap&aIcon);
    virtual QPixmap*pixmap()const;

protected:
    bool isBigIcon;
    int iteration;
    AppLnk* app;

private:
    void paintAnimatedIcon( QPainter *p );
    BusyIndicatorType busyType;
    int psize;
    bool m_EyeImage;
    iconstate_t m_EyeImageSet;
};

static bool s_IgnoreNextPix = false;

LauncherItem::LauncherItem( QIconView *parent, AppLnk *applnk, bool bigIcon )
    : QIconViewItem( parent, applnk->name(),
           bigIcon ? applnk->bigPixmap() :applnk->pixmap() ),
    isBigIcon( bigIcon ),
    iteration(0),
    app(applnk), // Takes ownership
    psize( (bigIcon ? applnk->bigPixmap().width() :applnk->pixmap().width() ) ),
    m_EyeImage(false),
    m_EyeImageSet(BASE_ICON)
{
    if (applnk->type().lower().startsWith("image/") && applnk->exec().contains("opie-eye",false)) {
        m_EyeImage = true;
        QMap<QString,QPixmap>::Iterator it = LauncherIconView::sm_EyeCache->find(applnk->file());
        if (it != LauncherIconView::sm_EyeCache->end()) {
            m_EyeImageSet = EYE_ICON;
            setPixmap(*it);
        }
    }
}

LauncherItem::~LauncherItem()
{
    LauncherIconView* liv = (LauncherIconView*)iconView();
    if ( liv->busyItem() == this )
        liv->setBusy(FALSE);
    delete app;
}

QPixmap*LauncherItem::pixmap()const
{
    if (m_EyeImage && m_EyeImageSet == BASE_ICON && s_IgnoreNextPix==false) {
        LauncherIconView* liv = (LauncherIconView*)iconView();
        liv->requestEyePix(this);
    }
    return QIconViewItem::pixmap();
}

int LauncherItem::compare ( QIconViewItem * i ) const
{
    LauncherIconView* view = (LauncherIconView*)iconView();
    return view->compare(app,((LauncherItem *)i)->appLnk());
}

void LauncherItem::paintItem( QPainter *p, const QColorGroup &cg )
{
    LauncherIconView* liv = (LauncherIconView*)iconView();
    QBrush oldBrush( liv->itemTextBackground() );
    QColorGroup mycg( cg );
    if ( liv->currentItem() == this ) {
        liv->setItemTextBackground( cg.brush( QColorGroup::Highlight ) );
        mycg.setColor( QColorGroup::Text, cg.color( QColorGroup::HighlightedText ) );
    }

    QIconViewItem::paintItem(p,mycg);

    // Paint animation overlay
    if ( liv->busyItem() == this )
        paintAnimatedIcon(p);

    if ( liv->currentItem() == this )
        liv->setItemTextBackground( oldBrush );
}

void LauncherItem::paintAnimatedIcon( QPainter *p )
{
    LauncherIconView* liv = (LauncherIconView*)iconView();
    int pic = iteration % 16;
    int w = pixmap()->width(), h = pixmap()->height();
    QPixmap dblBuf( w, h + 4 );
    QPainter p2( &dblBuf );
    int x1, y1;
    if ( liv->itemTextPos() == QIconView::Bottom ) {
    x1 = x() + (width() - w) / 2 - liv->contentsX();
    y1 = y() - liv->contentsY();
    } else {
    x1 = x() - liv->contentsX();
    y1 = y() + (height() - h) / 2 - liv->contentsY();
    }
    y1 -= 2;
    p2.translate(-x1,-y1);
    liv->drawBackground( &p2, QRect(x1,y1,w,h+4) );
    int bounceY = 2;
#ifdef BOUNCE_BUSY_ICON
    if ( busyType == BIT_Animated ) {
       bounceY = 4 - ((iteration+2)%8);
       bounceY = bounceY < 0 ? -bounceY : bounceY;
    }
#endif
    p2.drawPixmap( x1, y1 + bounceY, *pixmap() );
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    p2.drawPixmap( x1, y1 + bounceY, liv->busyPixmap(), w * pic, 0, w, h );
#else
    Q_UNUSED( pic )
#endif
    p->drawPixmap( x1, y1, dblBuf );
}

void LauncherItem::animateIcon()
{
    LauncherIconView* liv = (LauncherIconView*)iconView();

    if ( liv->busyItem() != this || !app )
    return;

    // Highlight the icon
    if ( iteration == 0 ) {
        QPixmap src;
        pixiter it;
        if (isEyeImage() && (it=LauncherIconView::sm_EyeCache->find(appLnk()->file()))!=LauncherIconView::sm_EyeCache->end()) {
            src = (*it);
        } else {
            src = ((isBigIcon ? app->bigPixmap() : app->pixmap()));
        }
        QImage img = src.convertToImage();
        QRgb *rgb;
        int count;
        if ( img.depth() == 32 ) {
            rgb = (QRgb*)img.bits();
            count = img.bytesPerLine()/sizeof(QRgb)*img.height();
        } else {
            rgb = img.colorTable();
            count = img.numColors();
        }
        for ( int r = 0; r < count; r++, rgb++ ) {
#if defined(BRIGHTEN_BUSY_ICON)
            QColor c(*rgb);
            int h, s, v;
            c.hsv(&h,&s,&v);
            c.setHsv(h,QMAX(s-24,0),QMIN(v+48,255));
            *rgb = qRgba(c.red(),c.green(),c.blue(),qAlpha(*rgb));
#elif defined(ALPHA_FADE_BUSY_ICON)
            *rgb = qRgba(qRed(*rgb),qGreen(*rgb),qBlue(*rgb),qAlpha(*rgb)/2);
#endif
        }
        src.convertFromImage( img );
        setPixmap( src );
    }

    iteration++;

    // Paint animation overlay
    QPainter p( liv->viewport() );
    paintAnimatedIcon( &p );
}

void LauncherItem::resetIcon()
{
    iteration = 0;
    if (isEyeImage()) {
        QMap<QString,QPixmap>::Iterator it = LauncherIconView::sm_EyeCache->find(appLnk()->file());
        if (it != LauncherIconView::sm_EyeCache->end()) {
            setPixmap(*it);
            return;
        }
    }
    setPixmap(isBigIcon ? app->bigPixmap() : app->pixmap());
}

void LauncherItem::setEyePixmap(const QPixmap&aIcon)
{
    if (!isEyeImage()) return;
    setPixmap(aIcon);
    m_EyeImageSet = EYE_ICON;
}

//===========================================================================
// Implemantation of LauncherIconview start
//===========================================================================

QMap<QString,QPixmap>* LauncherIconView::sm_EyeCache=0;

LauncherIconView::LauncherIconView( QWidget* parent, const char* name )
    : QIconView(parent,name),tf(""),cf(0),bsy(0),busyTimer(0),bigIcns(TRUE),bgColor(white),numColumns(0)
{
    m_EyeCallBack = 0;
    if (!sm_EyeCache) sm_EyeCache = new QMap<QString,QPixmap>();
    sortmeth = Name;
    hidden.setAutoDelete(TRUE);
    ike = FALSE;
    calculateGrid( Bottom );
    connect(&m_eyeTimer,SIGNAL(timeout()),this,SLOT(stopEyeTimer()));
    Config config( "Launcher" );
    config.setGroup( "GUI" );
    setStaticBackgroundPicture( config.readBoolEntry( "StaticBackground", true ) );
}

LauncherIconView::~LauncherIconView()
{
    odebug << "LauncherIconView::~LauncherIconView()" << oendl;
#if 0 // debuggery
    QListIterator<AppLnk> it(hidden);
    AppLnk* l;
    while ((l=it.current())) {
        ++it;
        //odebug << "" << l << ": hidden (should remove)" << oendl;
    }
#endif
}

void LauncherIconView::unsetPalette()
{
    s_IgnoreNextPix = true;
    QIconView::unsetPalette();
    s_IgnoreNextPix = false;
}

void LauncherIconView::setPalette(const QPalette & palette)
{
    s_IgnoreNextPix = true;
    QIconView::setPalette(palette);
    s_IgnoreNextPix = false;
}

void LauncherIconView::setStaticBackgroundPicture( bool enable )
{
    staticBackground = enable;
    if ( staticBackground )
    {
        setStaticBackground( true );
        verticalScrollBar()->setTracking( false );
    }
    else
    {
        setStaticBackground( false );
        verticalScrollBar()->setTracking( true );
    }
}

int LauncherIconView::compare(const AppLnk* a, const AppLnk* b)
{
    switch (sortmeth) {
    case Name:
        return a->name().lower().compare(b->name().lower());
    case Date: {
        QFileInfo fa(a->linkFileKnown() ? a->linkFile() : a->file());
        QFileInfo fb(b->linkFileKnown() ? b->linkFile() : b->file());
        return fa.lastModified().secsTo(fb.lastModified());
    }
    case Type:
        return a->type().compare(b->type());
    }
    return 0;
}

void LauncherIconView::setSortMethod( SortMethod m )
{
    if ( sortmeth != m ) {
        sortmeth = m;
        sort();
    }
}

void LauncherIconView::setCategoryFilter( int catfilter, bool resort )
{
        if ( catfilter == -2 )
            cf = 0;
        else
            cf = catfilter;
        hideOrShowItems(resort);
}

void LauncherIconView::setTypeFilter(const QString& typefilter, bool resort)
{
    tf = QRegExp(typefilter,FALSE,TRUE);
    hideOrShowItems(resort);
}

void LauncherIconView::setItemTextPos( ItemTextPos pos )
{
    calculateGrid( pos );
    QIconView::setItemTextPos( pos );
}

void LauncherIconView::drawBackground( QPainter *p, const QRect &r )
{
    if ( bgPixmap.isNull() )
    {
        p->fillRect( r, bgColor );
    }
    else
    {
        if ( staticBackground )
        {
            p->drawTiledPixmap( r, bgPixmap, QPoint( r.x() % bgPixmap.width(), r.y() % bgPixmap.height() ) );
        }
        else
        {
        p->drawTiledPixmap( r, bgPixmap, QPoint( (r.x() + contentsX()) % bgPixmap.width(),
                                                 (r.y() + contentsY()) % bgPixmap.height() ) );
        }
    }
}

void LauncherIconView::addCatsAndMimes(AppLnk* app)
{
    //  QStringList c = app->categories();
    //  for (QStringList::ConstIterator cit=c.begin(); cit!=c.end(); ++cit) {
    //      cats.replace(*cit,(void*)1);
    //  }
    QString maj=app->type();
    int sl=maj.find('/');
    if (sl>=0) {
        QString k;
        k = maj.left(12) == "application/" ? maj : maj.left(sl);
        mimes.replace(k,(void*)1);
    }
}

void LauncherIconView::setBusy(bool on)
{
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    if ( busyPix.isNull() ) {
        int size = ( bigIcns ) ? AppLnk::bigIconSize() : AppLnk::smallIconSize();
        busyPix.convertFromImage( OResource::loadImage( "busy", OResource::NoScale ).smoothScale( size * 16, size ) );
    }
#endif

    if ( on ) {
        busyTimer = startTimer( 100 );
    } else {
        if ( busyTimer ) {
            killTimer( busyTimer );
        busyTimer = 0;
        }
    }

    LauncherItem *c = on ? (LauncherItem*)currentItem() : 0;

    if ( bsy != c ) {
        LauncherItem *oldBusy = bsy;
        bsy = c;
        if ( oldBusy )  {
        oldBusy->resetIcon();
                    }
        if ( bsy )   {
                                bsy->setBusyIndicatorType( busyType ) ;
        bsy->animateIcon();
                    }
    }
}

void LauncherIconView::clear()
{
    mimes.clear();
    cats.clear();
    QIconView::clear();
    hidden.clear();
}

QStringList LauncherIconView::mimeTypes() const
{
    QStringList r;
    QDictIterator<void> it(mimes);
    while (it.current()) {
        r.append(it.currentKey());
        ++it;
    }
    r.sort();
    return r;
}

LauncherItem*LauncherIconView::findDocItem(const QString&fname)
{
    LauncherItem* item = (LauncherItem*)firstItem();
    while (item) {
        if (item->appLnk()->file()==fname) {
            break;
        }
        item = (LauncherItem*)item->nextItem();
    }
    return item;
}

void LauncherIconView::setEyePixmap(const QPixmap&aPixmap,const QString&aFile,int width)
{
    int s = ( bigIcns ) ? AppLnk::bigIconSize() : AppLnk::smallIconSize();
    if (s!=width) return;
    LauncherItem*item = findDocItem(aFile);
    if (!item||!item->isEyeImage()) return;
    (*sm_EyeCache)[aFile]=aPixmap;
    item->setEyePixmap(aPixmap);
}

void LauncherIconView::checkCallback()
{
    if (!m_EyeCallBack) {
        m_EyeCallBack = new LauncherThumbReceiver();
        connect(m_EyeCallBack,SIGNAL(sig_Thumbnail(const QPixmap&,const QString&,int)),
            this,SLOT(setEyePixmap(const QPixmap&,const QString&,int)));
        m_eyeTimer.changeInterval(600000);
    }
}

void LauncherIconView::addCheckItem(AppLnk* app)
{
    LauncherItem*item = new LauncherItem( this, app, bigIcns );
    if (item->isEyeImage()) {
        checkCallback();
    }
}

void LauncherIconView::requestEyePix(const LauncherItem*item)
{
    if (!item) return;
    if (item->isEyeImage()) {
        checkCallback();
        int s = ( bigIcns ) ? AppLnk::bigIconSize() : AppLnk::smallIconSize();
        m_EyeCallBack->requestThumb(item->appLnk()->file(),s,s);
    }
}

void LauncherIconView::stopEyeTimer()
{
    if (m_EyeCallBack) {
        disconnect(m_EyeCallBack,SIGNAL(sig_Thumbnail(const QPixmap&,const QString&,int)),
            this,SLOT(setEyePixmap(const QPixmap&,const QString&,int)));
        delete m_EyeCallBack;
        m_EyeCallBack=0;
    }
    m_eyeTimer.stop();
}

void LauncherIconView::updateCategoriesAndMimeTypes()
{
    mimes.clear();
    cats.clear();
    LauncherItem* item = (LauncherItem*)firstItem();
    while (item) {
        addCatsAndMimes(item->appLnk());
        item = (LauncherItem*)item->nextItem();
    }
    QListIterator<AppLnk> it(hidden);
    AppLnk* l;
    while ((l=it.current())) {
        addCatsAndMimes(l);
        ++it;
    }
}

void LauncherIconView::hideOrShowItems(bool resort)
{
    viewport()->setUpdatesEnabled( FALSE );
    hidden.setAutoDelete(FALSE);
    QList<AppLnk> links=hidden;
    hidden.clear();
    hidden.setAutoDelete(TRUE);
    LauncherItem* item = (LauncherItem*)firstItem();
    while (item) {
        links.append(item->takeAppLnk());
        item = (LauncherItem*)item->nextItem();
    }
    clear();
    QListIterator<AppLnk> it(links);
    AppLnk* l;
    while ((l=it.current())) {
        addItem(l,FALSE);
        ++it;
    }
    if ( resort && !autoArrange() )
    sort();
    viewport()->setUpdatesEnabled( TRUE );
}

bool LauncherIconView::removeLink(const QString& linkfile,bool removeCache)
{
    LauncherItem* item = (LauncherItem*)firstItem();
    AppLnk* l;
    bool did = FALSE;
    DocLnk dl(linkfile);
    while (item) {
        l = item->appLnk();
        LauncherItem *nextItem = (LauncherItem *)item->nextItem();
        if (  l->linkFileKnown() && l->linkFile() == linkfile || l->fileKnown() &&
                 ( l->file() == linkfile || dl.isValid() && dl.file() == l->file() ) ) {
            if (removeCache) sm_EyeCache->remove(l->file());
            delete item;
            did = TRUE;
        }
        item = nextItem;
    }
    QListIterator<AppLnk> it(hidden);
    while ((l=it.current())) {
        ++it;
        if ( l->linkFileKnown() && l->linkFile() == linkfile
                || l->file() == linkfile
                || dl.isValid() && dl.file() == l->file() ) {
            hidden.removeRef(l);
           did = TRUE;
        }
    }
    return did;
}

void LauncherIconView::addItem(AppLnk* app, bool resort)
{
    addCatsAndMimes(app);
    if ( (tf.isEmpty() || tf.match(app->type()) >= 0)
     && (cf == 0 || app->categories().contains(cf)
         || cf == -1 && app->categories().count() == 0 ) ) {
        addCheckItem(app);
    } else {
        hidden.append(app);
    }
    if ( resort ){
        sort();
    }
}

void LauncherIconView::changeItem(const AppLnk&old,AppLnk*nlink)
{
    QString oldfile = old.file();
    QString newfile = nlink->file();

    if (newfile != oldfile) {
        QMap<QString,QPixmap>::Iterator it = sm_EyeCache->find(oldfile);
        if (it != sm_EyeCache->end()) {
            (*sm_EyeCache)[newfile]=(*it);
        }
        removeLink(old.linkFile());
    } else {
        removeLink(old.linkFile(),false);
    }
    addItem(nlink,false);
}

void LauncherIconView::timerEvent( QTimerEvent *te )
{
    if ( te->timerId() == busyTimer ) {
        if ( bsy )
        bsy->animateIcon();
    } else {
        QIconView::timerEvent( te );
    }
}

void LauncherIconView::setBigIcons( bool bi )
{
    sm_EyeCache->clear();
    bigIcns = bi;
#ifdef USE_ANIMATED_BUSY_ICON_OVERLAY
    busyPix.resize(0,0);
#endif
}

QIconViewItem* LauncherIconView::busyItem() const
{
    return bsy;
}

void LauncherIconView::setBusyIndicatorType ( BusyIndicatorType t ) { busyType = t; }

void LauncherIconView::calculateGrid( ItemTextPos pos )
{
        int dw = QApplication::desktop()->width();
        int viewerWidth = dw-style().scrollBarExtent().width();
        if ( pos == Bottom ) {
            if( !numColumns ) {
                if ( viewerWidth <= 200 ) numColumns = 2;
                else if ( viewerWidth >= 400 ) numColumns = viewerWidth/96;
                else numColumns = 3;
            }
            setSpacing( 4 );
            setGridX( (viewerWidth-(numColumns+1)*spacing())/numColumns );
            setGridY( fontMetrics().height()*2+24 );
        } else {
            if( !numColumns ) {
                if ( viewerWidth < 150 ) numColumns = 1;
                else if ( viewerWidth >= 400 ) numColumns = viewerWidth/150;
                else numColumns = 2;
            }
            setSpacing( 2 );
            setGridX( (viewerWidth-(numColumns+1)*spacing())/numColumns );
            setGridY( fontMetrics().height()+2 );
        }
}

void LauncherIconView::styleChange( QStyle &old )
{
    QIconView::styleChange( old );
    calculateGrid( itemTextPos() );
}

void LauncherIconView::keyPressEvent(QKeyEvent* e)
{
    ike = TRUE;
    if ( e->key() == Key_F33 /* OK button */ || e->key() == Key_Space ) {
        if ( (e->state() & ShiftButton) )
            emit mouseButtonPressed(ShiftButton, currentItem(), QPoint() );
         else
            returnPressed(currentItem());
    }

    QIconView::keyPressEvent(e);
    ike = FALSE;
}

//===========================================================================
// Implemantation of LauncherIconview end
//===========================================================================


//===========================================================================
LauncherView::LauncherView( QWidget* parent, const char* name, WFlags fl )
    : QVBox( parent, name, fl )
{
    catmb = 0;
    icons = new LauncherIconView( this );
    setFocusProxy(icons);
    QPEApplication::setStylusOperation( icons->viewport(), QPEApplication::RightOnHold );

    icons->setItemsMovable( FALSE );
    icons->setAutoArrange( TRUE );
    icons->setSorting( TRUE );
    icons->setFrameStyle( QFrame::NoFrame );
    icons->setMargin( 0 );
    icons->setSelectionMode( QIconView::NoSelection );
    icons->setBackgroundMode( PaletteBase );
    icons->setResizeMode( QIconView::Adjust );
    vmode = (ViewMode)-1;
    setViewMode( Icon );

    connect( icons, SIGNAL(mouseButtonClicked(int,QIconViewItem*,const QPoint&)),
           SLOT(itemClicked(int,QIconViewItem*)) );
    connect( icons, SIGNAL(selectionChanged()),
           SLOT(selectionChanged()) );
    connect( icons, SIGNAL(returnPressed(QIconViewItem*)),
           SLOT(returnPressed(QIconViewItem*)) );
    connect( icons, SIGNAL(mouseButtonPressed(int,QIconViewItem*,const QPoint&)),
           SLOT(itemPressed(int,QIconViewItem*)) );

    tools = 0;
    setBackgroundType( Ruled, QString::null );
}

LauncherView::~LauncherView()
{
    if ( bgCache && bgCache->contains( bgName ) )
    (*bgCache)[bgName]->ref--;
}


bool LauncherView::bsy=FALSE;

void LauncherView::setBusy(bool on)
{
    icons->setBusy(on);
}

void LauncherView::setBusyIndicatorType( const QString& type ) {
    if ( type. lower ( ) == "animated" )
    icons->setBusyIndicatorType(  BIT_Animated  ) ;
    else
    icons->setBusyIndicatorType(  BIT_Normal  ) ;
}

void LauncherView::hideIcons()
{
    icons->hide();
}

void LauncherView::setToolsEnabled(bool y)
{
    if ( !y != !tools ) {
        if ( y ) {
            tools = new QHBox(this);
            // Type filter
            typemb = new QComboBox(tools);
            QSizePolicy p = typemb->sizePolicy();
            p.setHorData(QSizePolicy::Expanding);
            typemb->setSizePolicy(p);
            // Category filter
            updateTools();
            tools->show();
        } else {
            delete tools;
            tools = 0;
        }
    }
}

void LauncherView::updateTools()
{
    disconnect( typemb, SIGNAL(activated(int)),
            this, SLOT(showType(int)) );
    if ( catmb ) {
        disconnect( catmb, SIGNAL(signalSelected(int)),this,SLOT(showCategory(int)));
    }

    // ### I want to remove this
    icons->updateCategoriesAndMimeTypes();

    QString prev;

    // Type filter
    QStringList types;
    typelist = icons->mimeTypes();
    for (QStringList::ConstIterator it = typelist.begin(); it!=typelist.end(); ++it) {
        QString t = *it;
        if ( t.left(12) == "application/" ) {
            MimeType mt(t);
            const AppLnk* app = mt.application();
            if ( app )
                t = app->name();
            else
                t = t.mid(12);
        } else {
            t[0] = t[0].upper();
        }
        types += t;
    }
    types << tr("All types");
    prev = typemb->currentText();
    typemb->clear();
    typemb->insertStringList(types);
    for (int i=0; i<typemb->count(); i++) {
        if ( typemb->text(i) == prev ) {
            typemb->setCurrentItem(i);
            break;
        }
    }
    if ( prev.isNull() ) {
        typemb->setCurrentItem(typemb->count()-1);
    }

    int pcat = catmb ? catmb->currentCategory() : -2;
    if ( !catmb ) {
        catmb = new CategorySelect(tools);
    } else if (pcat!=-2) {

    }
    Categories cats( 0 );
    cats.load( categoryFileName() );
    QArray<int> vl( 0 );
    catmb->setCategories( vl, "Document View", // No tr
        tr("Document View") );
    catmb->setRemoveCategoryEdit( TRUE );
    catmb->setAllCategories( TRUE );
    catmb->setCurrentCategory(pcat);

    // if type has changed we need to redisplay
    if ( typemb->currentText() != prev )
        showType( typemb->currentItem() );

    connect(typemb, SIGNAL(activated(int)), this, SLOT(showType(int)));
    connect(catmb, SIGNAL(signalSelected(int)), this, SLOT(showCategory(int)));
}

void LauncherView::sortBy(int s)
{
    icons->setSortMethod((LauncherIconView::SortMethod)s);
}

void LauncherView::showType(int t)
{
    if ( t >= (int)typelist.count() ) {
        icons->setTypeFilter("",TRUE);
    } else {
        QString ty = typelist[t];
        if ( !ty.contains('/') )
            ty += "/*";
        icons->setTypeFilter(ty,TRUE);
    }
}

void LauncherView::showCategory( int c )
{
    icons->setCategoryFilter( c, TRUE );
}

void LauncherView::setViewMode( ViewMode m )
{
    odebug << "LauncherView::setViewMode( ViewMode m )" << oendl;
    if ( vmode != m ) {
        bool bigIcons = m == Icon;
        icons->viewport()->setUpdatesEnabled( FALSE );
        icons->setBigIcons( bigIcons );
        switch ( m ) {
            case List:
            icons->setItemTextPos( QIconView::Right );
        break;
        case Icon:
            icons->setItemTextPos( QIconView::Bottom );
        break;
        }
        icons->hideOrShowItems( FALSE );
        icons->viewport()->setUpdatesEnabled( TRUE );
        vmode = m;
    }
}

//
// User images may require scaling.
//
QImage LauncherView::loadBackgroundImage(QString &bgName)
{
    QImageIO imgio;
    QSize   ds = qApp->desktop()->size();   // should be launcher, not desktop
    bool    further_scaling = TRUE;

    imgio.setFileName( bgName );
    imgio.setParameters("GetHeaderInformation");

    if (imgio.read() == FALSE) {
    return imgio.image();
    }

    if (imgio.image().width() < ds.width() &&
        imgio.image().height() < ds.height()) {
    further_scaling = FALSE;
    }

    if (!imgio.image().bits()) {
    //
    // Scale and load.  Note we don't scale up.
    //
    QString param( "Scale( %1, %2, ScaleMin )" ); // No tr
    imgio.setParameters(further_scaling ?
        param.arg(ds.width()).arg(ds.height()).latin1() :
        "");
    imgio.read();
    } else {
    if (further_scaling) {
        int t1 = imgio.image().width() * ds.height();
        int t2 = imgio.image().height() * ds.width();
        int dsth = ds.height();
        int dstw = ds.width();

        if (t1 > t2) {
        dsth = t2 / imgio.image().width();
        } else {
        dstw = t1 / imgio.image().height();
        }

        //
        // Loader didn't scale for us.  Do it manually.
        //
        return imgio.image().smoothScale(dstw, dsth);
    }
    }

    return imgio.image();
}

void LauncherView::setBackgroundType( BackgroundType t, const QString &val )
{
    if ( !bgCache ) {
    bgCache = new QMap<QString,BgPixmap*>;
    qAddPostRoutine( cleanup_cache );
    }

    if ( bgCache->contains( bgName ) )
    (*bgCache)[bgName]->ref--;
    bgName = "";

    QPixmap bg;

    switch ( t ) {
    case Ruled: {
        bgName = QString("Ruled_%1").arg(colorGroup().background().name()); // No tr
        if ( bgCache->contains( bgName ) ) {
        (*bgCache)[bgName]->ref++;
        bg = (*bgCache)[bgName]->pm;
        } else {
        bg.resize( width(), 9 );
        QPainter painter( &bg );
        for ( int i = 0; i < 3; i++ ) {
            painter.setPen( white );
            painter.drawLine( 0, i*3, width()-1, i*3 );
            painter.drawLine( 0, i*3+1, width()-1, i*3+1 );
            painter.setPen( colorGroup().background().light(105) );
            painter.drawLine( 0, i*3+2, width()-1, i*3+2 );
        }
        painter.end();
        bgCache->insert( bgName, new BgPixmap(bg) );
        }
        break;
    }

    case Image:
        if (!val.isEmpty()) {
        bgName = val;
        if ( bgCache->contains( bgName ) ) {
            (*bgCache)[bgName]->ref++;
            bg = (*bgCache)[bgName]->pm;
        } else {
            QString imgFile = bgName;
            bool tile = FALSE;
            if ( imgFile[0]!='/' || !QFile::exists(imgFile) ) {
            imgFile = OResource::findPixmap( imgFile );
            tile = TRUE;
            }
            QImage img = loadBackgroundImage(imgFile);


            if ( img.depth() == 1 )
            img = img.convertDepth(8);
            img.setAlphaBuffer(FALSE);
            bg.convertFromImage(img);
            bgCache->insert( bgName, new BgPixmap(bg) );
        }
        }
        break;

    case SolidColor:
    default:
        break;
    }

    const QObjectList *list = queryList( "QWidget", 0, FALSE );
    QObject *obj;
    for ( QObjectListIt it( *list ); (obj=it.current()); ++it ) {
    if ( obj->isWidgetType() ) {
        QWidget *w = (QWidget*)obj;
        w->setBackgroundPixmap( bg );
        if ( bgName.isEmpty() ) {
        // Solid Color
        if ( val.isEmpty() )
            w->setBackgroundColor( colorGroup().base() );
        else
            w->setBackgroundColor( val );
        } else {
        // Ruled or Image pixmap
        w->setBackgroundOrigin( ParentOrigin );
        }
    }
    }
    delete list;

    bgType = t;
    icons->viewport()->update();

    QTimer::singleShot( 1000, this, SLOT(flushBgCache()) );
}

void LauncherView::setColNumber( int num )
{
    icons->setColNumber( num );
}

void LauncherIconView::setColNumber( int num )
{
    numColumns = num;
    calculateGrid( Bottom );
}

void LauncherView::setTextColor( const QColor &tc )
{
    textCol = tc;
    QColorGroup cg = icons->colorGroup();
    cg.setColor( QColorGroup::Text, tc );
    icons->setPalette( QPalette(cg,cg,cg) );
    icons->viewport()->update();
}

void LauncherView::setViewFont( const QFont &f )
{
    icons->setFont( f );
    icons->hideOrShowItems( FALSE );
}

void LauncherView::clearViewFont()
{
    icons->unsetFont();
    icons->hideOrShowItems( FALSE );
}

void LauncherView::resizeEvent(QResizeEvent *e)
{
//  qDebug("LauncherView resize event");
    QVBox::resizeEvent( e );
// commented out for launcherview and qt/e 2.3.8 problems, probably needs real fixing somewhere...
//    if ( e->size().width() != e->oldSize().width() )
    sort();
}

void LauncherView::selectionChanged()
{
    QIconViewItem* item = icons->currentItem();
    if ( item && item->isSelected() ) {
    AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
    if ( icons->inKeyEvent() ) // not for mouse press
        emit clicked( appLnk );
    item->setSelected(FALSE);
    }
}

void LauncherView::returnPressed( QIconViewItem *item )
{
    if ( item ) {
    AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
    emit clicked( appLnk );
    }
}

void LauncherView::itemClicked( int btn, QIconViewItem *item )
{
    if ( item ) {
    AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
    if ( btn == LeftButton ) {
        // Make sure it's the item we execute that gets highlighted
        icons->setCurrentItem( item );
        emit clicked( appLnk );
    }
        item->setSelected(FALSE);
    }
}

void LauncherView::itemPressed( int btn, QIconViewItem *item )
{
    if ( item ) {
    AppLnk *appLnk = ((LauncherItem *)item)->appLnk();
    if ( btn == RightButton )
        emit rightPressed( appLnk );
    else if ( btn == ShiftButton )
        emit rightPressed( appLnk );
    item->setSelected(FALSE);
    }
}

void LauncherView::removeAllItems()
{
    odebug << "LauncherView::removeAllItems()" << oendl;
    if (LauncherIconView::sm_EyeCache) LauncherIconView::sm_EyeCache->clear();
    icons->clear();
}

bool LauncherView::removeLink(const QString& linkfile)
{
    return icons->removeLink(linkfile);
}

void LauncherView::addItem(AppLnk* app, bool resort)
{
    icons->addItem(app,resort);
}

void LauncherView::changeItem(const AppLnk&old,AppLnk*nlink)
{
    icons->changeItem(old,nlink);
}

void LauncherView::setSortEnabled( bool v )
{
    icons->setSorting( v );
    if ( v )
    sort();
}

void LauncherView::setUpdatesEnabled( bool u )
{
    icons->setUpdatesEnabled( u );
}

void LauncherView::sort()
{
    icons->sort();
}

void LauncherView::paletteChange( const QPalette &p )
{
    icons->unsetPalette();
    QVBox::paletteChange( p );
    if ( bgType == Ruled )
    setBackgroundType( Ruled, QString::null );
    QColorGroup cg = icons->colorGroup();
    cg.setColor( QColorGroup::Text, textCol );
    icons->setPalette( QPalette(cg,cg,cg) );
}

void LauncherView::fontChanged(const QFont&)
{
    odebug << "LauncherView::fontChanged()" << oendl;
    icons->hideOrShowItems( FALSE );
}

void LauncherView::relayout(void)
{
    icons->hideOrShowItems(FALSE);
}

void LauncherView::flushBgCache()
{
    if ( !bgCache )
    return;
    // remove unreferenced backgrounds.
    QMap<QString,BgPixmap*>::Iterator it = bgCache->begin();
    while ( it != bgCache->end() ) {
    QMap<QString,BgPixmap*>::Iterator curr = it;
    ++it;
    if ( (*curr)->ref == 0 ) {
        delete (*curr);
        bgCache->remove( curr );
    }
    }
}

/*
 * Launcherthumbnail handling for image files
 */

/* special image handling - based on opie eye */
QDataStream &operator>>( QDataStream& s, PixmapInfo& inf ) {
    s >> inf.file >> inf.pixmap >> inf.width >> inf.height;
    return s;
}

QDataStream &operator<<( QDataStream& s, const PixmapInfo& inf) {
    return s << inf.file  << inf.width << inf.height;
}

LauncherThumbReceiver::LauncherThumbReceiver()
    :QObject(),requestTimer(this)
{

    connect(&requestTimer,SIGNAL(timeout()),SLOT(sendRequest()));
    QCopChannel * chan = new QCopChannel( "QPE/opie-eye",this );
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&)),
            this, SLOT(recieve(const QCString&,const QByteArray&)) );

    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refUp()" );
    }
}

LauncherThumbReceiver::~LauncherThumbReceiver()
{
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refDown()" );
    }
}

void LauncherThumbReceiver::recieve( const QCString&str, const QByteArray&at )
{
    PixmapInfos pixinfos;
    QDataStream stream( at, IO_ReadOnly );

    /* we are just interested in thumbmails */
    if ( str == "pixmapsHandled(PixmapList)" )
        stream >> pixinfos;

    for ( PixmapInfos::Iterator it = pixinfos.begin(); it != pixinfos.end(); ++it ) {
        emit sig_Thumbnail((*it).pixmap,(*it).file,(*it).width);
    }
}

void LauncherThumbReceiver::requestThumb(const QString&file,int width,int height)
{
    PixmapInfo rItem;
    rItem.file = file;
    rItem.width = width;
    rItem.height = height;
    m_inThumbNail.append(rItem);
    if (!requestTimer.isActive()) {
        requestTimer.start(100,true);
    }
}

void LauncherThumbReceiver::sendRequest()
{
    if (m_inThumbNail.count()>0) {
        QCopEnvelope env("QPE/opie-eye_slave", "pixmapInfos(PixmapInfos)" );
        env << m_inThumbNail;
        m_inThumbNail.clear();
    }
}
